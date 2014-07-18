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
// Module: golangautocompleter.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGAUTOCOMPLETER_H
#define GOLANGAUTOCOMPLETER_H

#include "qtc_texteditor/autocompleter.h"
#include "cplusplus/Token.h"

class GolangAutoCompleter : public TextEditor::AutoCompleter
{
public:
    GolangAutoCompleter();
    virtual bool contextAllowsElectricCharacters(const QTextCursor &cursor) const;
    virtual bool isInComment(const QTextCursor &cursor) const;
private:
    bool isInCommentHelper(const QTextCursor &cursor, CPlusPlus::Token *retToken = 0) const;
    const CPlusPlus::Token tokenAtPosition(const QList<CPlusPlus::Token> &tokens, const unsigned pos) const;
};

#endif // GOLANGAUTOCOMPLETER_H
