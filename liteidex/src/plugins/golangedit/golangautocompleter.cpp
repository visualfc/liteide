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
// Module: golangautocompleter.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangautocompleter.h"
#include "cplusplus/SimpleLexer.h"
#include "cplusplus/BackwardsScanner.h"
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

GolangAutoCompleter::GolangAutoCompleter()
{
}

bool GolangAutoCompleter::contextAllowsElectricCharacters(const QTextCursor &cursor) const
{
    Token token;

    if (isInCommentHelper(cursor, &token))
        return false;

    if (token.is(T_RAW_STRING_LITERAL)) {
        //return false;
    }
    if (token.isStringLiteral() || token.isCharLiteral()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();
        if (pos <= token.end())
            return false;
    }
    return true;
}

bool GolangAutoCompleter::isInComment(const QTextCursor &cursor) const
{
    return isInCommentHelper(cursor);
}

bool GolangAutoCompleter::isInCommentHelper(const QTextCursor &cursor, Token *retToken) const
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
        return tk.isComment() && (cursor.block().userState() & 0xFF);
    }

    Token tk = tokenAtPosition(tokens, pos);

    if (retToken)
        *retToken = tk;

    return tk.isComment();
}

const Token GolangAutoCompleter::tokenAtPosition(const QList<Token> &tokens, const unsigned pos) const
{
    for (int i = tokens.size() - 1; i >= 0; --i) {
        const Token tk = tokens.at(i);
        if (pos >= tk.begin() && pos < tk.end())
            return tk;
    }
    return Token();
}
