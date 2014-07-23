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
// Module: golangtextlexer.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGTEXTLEXER_H
#define GOLANGTEXTLEXER_H

#include "liteeditorapi/liteeditorapi.h"
#include "cplusplus/Token.h"
#include "functiontooltip.h"

class GolangTextLexer : public LiteApi::ITextLexer
{
public:
    GolangTextLexer(LiteApi::ITextEditor *editor);
    ~GolangTextLexer();
    virtual bool isInComment(const QTextCursor &cursor) const;
    virtual bool isInString(const QTextCursor &cursor) const;
    virtual bool isCanCodeCompleter(const QTextCursor &cursor) const;
    virtual bool isCanAutoCompleter(const QTextCursor &cursor) const;
    virtual int startOfFunctionCall(const QTextCursor &cursor) const;
    virtual void showToolTip(int startPosition, const QString &args);
protected:
    bool isInCommentHelper(const QTextCursor &cursor, CPlusPlus::Token *retToken = 0) const;
    const CPlusPlus::Token tokenAtPosition(const QList<CPlusPlus::Token> &tokens, const unsigned pos) const;
protected:
    FunctionTooltip  *m_fnTip;
};

#endif // GOLANGTEXTLEXER_H
