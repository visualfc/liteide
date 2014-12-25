/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: golangtextlexer.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangtextlexer.h"
#include "cplusplus/BackwardsScanner.h"
#include "cplusplus/SimpleLexer.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

using namespace CPlusPlus;

GolangTextLexer::GolangTextLexer(QObject *parent)
    : LiteApi::ITextLexer(parent)
{

}

bool GolangTextLexer::isInComment(const QTextCursor &cursor) const
{
    return isInCommentHelper(cursor);
}

bool GolangTextLexer::isInString(const QTextCursor &cursor) const
{
    Token token;
    if (isInCommentHelper(cursor, &token))
        return false;

    if (token.isStringLiteral() || token.isCharLiteral()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();
        if (pos <= token.end())
            return true;
    }
    return false;
}

bool GolangTextLexer::isInEmptyString(const QTextCursor &cursor) const
{
    Token token;
    if (isInCommentHelper(cursor, &token))
        return false;

    if (token.isStringLiteral() || token.isCharLiteral()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();
        if (pos <= token.end()) {
            return token.length() == 2;
        }
    }
    return false;
}

bool GolangTextLexer::isEndOfString(const QTextCursor &cursor) const
{
    Token token;
    if (isInCommentHelper(cursor, &token))
        return false;

    if (token.isStringLiteral() || token.isCharLiteral()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();
        return (token.end()-pos == 1);
    }
    return false;
}

bool GolangTextLexer::isInStringOrComment(const QTextCursor &cursor) const
{
    Token token;

    if (isInCommentHelper(cursor, &token))
        return true;

    if (token.isStringLiteral() || token.isCharLiteral()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();
        if (pos <= token.end())
            return true;
    }
    return false;
}

bool GolangTextLexer::isLangSupport() const
{
    return true;
}

bool GolangTextLexer::isInImportHelper(const QTextCursor &cursor) const
{
    const int blockNumber = cursor.block().blockNumber();
    QTextBlock block = cursor.document()->firstBlock();
    int pos1 = -1;
    while (block.isValid()) {
        QString text = block.text().trimmed();
        if (text.startsWith("/*")) {
            block = block.next();
            while(block.isValid()) {
                if (block.text().endsWith("*/")) {
                    break;
                }
                block = block.next();
            }
            if (!block.isValid()) {
                break;
            }
        } else if (text.startsWith("var")) {
            break;
        } else if (text.startsWith("func")) {
            break;
        } else if (text.startsWith("package ")) {
            pos1 = block.position()+block.length();
        } else if (pos1 != -1 && text.startsWith("import (")) {
            block = block.next();
            while(block.isValid()) {
                QString text = block.text().trimmed();
                if (text.startsWith(")")) {
                    break;
                }
                //skip
                if (text.startsWith("/*")) {
                    block = block.next();
                    while(block.isValid()) {
                        if (block.text().endsWith("*/")) {
                            break;
                        }
                        block = block.next();
                    }
                    if (!block.isValid()) {
                        break;
                    }
                }
                if (text.startsWith("//")) {
                    block = block.next();
                    continue;
                }
                if (block.blockNumber() == blockNumber) {
                    return true;
                }
                block = block.next();
            }
        } else if (pos1 != -1 && text.startsWith("import ")) {
            if (block.blockNumber() == blockNumber) {
                return true;
            }
        }
        block = block.next();
    }
    return false;
}

bool GolangTextLexer::isInImport(const QTextCursor &cursor) const
{
    Token token;
    if (isInCommentHelper(cursor, &token))
        return false;
    if (token.isStringLiteral() || token.isCharLiteral()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();
        QString tk = cursor.block().text().mid(token.begin(),token.length());
        if (tk.endsWith('\"') || tk.endsWith('`')) {
            if (pos > token.begin() && pos < token.end()) {
                return isInImportHelper(cursor);
            }
        } else {
            if (pos > token.begin()) {
                return isInImportHelper(cursor);
            }
        }
    }
    return false;
}

bool GolangTextLexer::isCanAutoCompleter(const QTextCursor &cursor) const
{
    return !isInStringOrComment(cursor);
}

int GolangTextLexer::startOfFunctionCall(const QTextCursor &cursor) const
{
    LanguageFeatures features;
    features.golangEnable = true;

    BackwardsScanner scanner(features,cursor);

    int index = scanner.startToken();
    forever {
        const Token &tk = scanner[index - 1];
        if (tk.is(T_EOF_SYMBOL)) {
            break;
        } else if (tk.is(T_LPAREN)) {
            return scanner.startPosition() + tk.begin();
        } else if (tk.is(T_RPAREN)) {
            int matchingBrace = scanner.startOfMatchingBrace(index);

            if (matchingBrace == index) // If no matching brace found
                return -1;

            index = matchingBrace;
        } else {
            --index;
        }
    }
    return -1;
}

QString GolangTextLexer::fetchFunctionTip(const QString &func, const QString &kind, const QString &info)
{
    if (kind != "func" || info.startsWith("func()")) {
        return QString();
    }
    return func+" "+info;
}

bool GolangTextLexer::fetchFunctionArgs(const QString &str, int &argnr, int &parcount)
{
    LanguageFeatures features;
    features.golangEnable = true;
    argnr = 0;
    parcount = 0;
    SimpleLexer tokenize;
    tokenize.setLanguageFeatures(features);
    QList<Token> tokens = tokenize(str);
    for (int i = 0; i < tokens.count(); ++i) {
        const Token &tk = tokens.at(i);
        if (tk.is(T_LPAREN))
            ++parcount;
        else if (tk.is(T_RPAREN))
            --parcount;
        else if (! parcount && tk.is(T_COMMA))
            ++argnr;
    }
    return true;
}

bool GolangTextLexer::isInCommentHelper(const QTextCursor &cursor, Token *retToken) const
{
    LanguageFeatures features;
    features.golangEnable = true;

    SimpleLexer tokenize;
    tokenize.setLanguageFeatures(features);

    const int prevState = BackwardsScanner::previousBlockState(cursor.block()) & 0xFF;
    const QList<Token> tokens = tokenize(cursor.block().text(), prevState);

    const unsigned pos = cursor.selectionEnd() - cursor.block().position();

    if (tokens.isEmpty() || pos < tokens.first().begin())
        return prevState > 0;

    if (pos >= tokens.last().end()) {
        const Token tk = tokens.last();
        if (tk.is(T_CPP_COMMENT) || tk.is(T_CPP_DOXY_COMMENT))
            return true;
        if (tk.is(T_RAW_STRING_LITERAL) && (cursor.block().userState() & 0xFF) ) {
            if (retToken)
                *retToken = tk;
            return false;
        }
        if (tk.isStringLiteral() || tk.isCharLiteral()) {
            if (retToken)
                *retToken = tk;
            return false;
        }
        return tk.isComment() && (cursor.block().userState() & 0xFF);
    }

    Token tk = tokenAtPosition(tokens, pos);

    if (retToken)
        *retToken = tk;

    return tk.isComment();
}

const Token GolangTextLexer::tokenAtPosition(const QList<Token> &tokens, const unsigned pos) const
{
    for (int i = tokens.size() - 1; i >= 0; --i) {
        const Token tk = tokens.at(i);
        if (pos >= tk.begin() && pos < tk.end())
            return tk;
    }
    return Token();
}
