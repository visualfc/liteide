/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
#include "functiontooltip.h"
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

GolangTextLexer::GolangTextLexer(LiteApi::IApplication *app, LiteApi::ITextEditor *editor)
{
     m_fnTip = new FunctionTooltip(app,editor,this,20);
}

GolangTextLexer::~GolangTextLexer()
{
    delete m_fnTip;
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

bool GolangTextLexer::isCanCodeCompleter(const QTextCursor &cursor) const
{
    return !isInStringOrComment(cursor);
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

void GolangTextLexer::showToolTip(int startPosition, const QString &func, const QString &kind, const QString &info)
{
    if (kind != "func") {
        return;
    }
    if (info.startsWith("func()")) {
        return;
    }
    m_fnTip->showFunctionHint(startPosition,func+" "+info);
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
