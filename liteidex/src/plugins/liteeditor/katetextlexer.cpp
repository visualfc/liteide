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
// Module: katetextlexer.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "katetextlexer.h"
#include "qtc_texteditor/basetextdocumentlayout.h"
#include "qtc_texteditor/syntaxhighlighter.h"

#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

KateTextLexer::KateTextLexer(QObject *parent)
    : LiteApi::ITextLexer(parent)
{
}

bool KateTextLexer::isLangSupport() const
{
    return false;
}

bool KateTextLexer::isInComment(const QTextCursor &cursor) const
{
    int pos = cursor.positionInBlock();
    TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::userData(cursor.block());
    foreach(TextEditor::SyntaxToken token, data->tokens()) {
        if ((token.id == TextEditor::SyntaxHighlighter::Comment) &&
                (pos >= token.offset) && (pos <= (token.offset+token.count))) {
            return true;
        }
    }
    return false;
}

bool KateTextLexer::isInString(const QTextCursor &cursor) const
{
    int pos = cursor.positionInBlock();
    TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::userData(cursor.block());
    foreach(TextEditor::SyntaxToken token, data->tokens()) {
        if ((token.id == TextEditor::SyntaxHighlighter::String) &&
                (pos >= token.offset) && (pos < token.offset+token.count)) {
            return true;
        }
    }
    return false;
}

bool KateTextLexer::isInEmptyString(const QTextCursor &cursor) const
{
    int pos = cursor.positionInBlock();
    TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::userData(cursor.block());
    foreach(TextEditor::SyntaxToken token, data->tokens()) {
        if ((token.id == TextEditor::SyntaxHighlighter::String) &&
                (pos >= token.offset) && (pos < token.offset+token.count) &&
                (token.count == 2)) {
            return true;
        }
    }
    return false;
}

bool KateTextLexer::isEndOfString(const QTextCursor &cursor) const
{
    int pos = cursor.positionInBlock();
    TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::userData(cursor.block());
    foreach(TextEditor::SyntaxToken token, data->tokens()) {
        if ((token.id == TextEditor::SyntaxHighlighter::String) &&
                (pos == (token.offset+token.count-1)) ) {
            return true;
        }
    }
    return false;
}

bool KateTextLexer::isInStringOrComment(const QTextCursor &cursor) const
{
    return isInString(cursor) || isInComment(cursor);
}

bool KateTextLexer::isCanAutoCompleter(const QTextCursor &cursor) const
{
    return !isInStringOrComment(cursor);
}

bool KateTextLexer::isInImport(const QTextCursor &/*cursor*/) const
{
    return false;
}

int KateTextLexer::startOfFunctionCall(const QTextCursor &/*cursor*/) const
{
    return -1;
}

QString KateTextLexer::fetchFunctionTip(const QString &/*func*/, const QString &/*kind*/, const QString &/*info*/)
{
    return QString();
}

bool KateTextLexer::fetchFunctionArgs(const QString &/*str*/, int &/*argnr*/, int &/*parcount*/)
{
    return false;
}
