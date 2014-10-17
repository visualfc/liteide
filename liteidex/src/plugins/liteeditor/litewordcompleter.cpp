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
// Module: litewordcompleter.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litewordcompleter.h"
#include "codecompleter.h"
#include <QPlainTextEdit>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextDocument>
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


LiteWordCompleter::LiteWordCompleter(QObject *parent) :
    LiteCompleter(parent),
    m_icon(QIcon("icon:liteeditor/images/findword.png"))
{
    m_completer->setSeparator(".");
}

QString LiteWordCompleter::textUnderCursor(QTextCursor tc) const
{
    QString text = tc.block().text().left(tc.positionInBlock());
    if (text.isEmpty()) {
        return QString();
    }
    static QRegExp reg("[a-zA-Z_]+[a-zA-Z0-9_\\.@]*$");
    int index = reg.indexIn(text);
    if (index < 0) {
        return QString();
    }
    return text.right(reg.matchedLength());
}

void LiteWordCompleter::completionPrefixChanged(QString prefix,bool force)
{
    LiteCompleter::completionPrefixChanged(prefix,force);

    if (m_bExternalMode) {
        return;
    }

    if (!m_editor) {
        return;
    }
    if (!m_bSearchSeparator) {
        if (prefix.indexOf(".") >= 0) {
            return;
        }
    }

    QTextCursor tc = m_editor->textCursor();
    int startPosition = tc.position() - prefix.size();
    tc.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);

    QTextDocument::FindFlags flags = QTextDocument::FindCaseSensitively;
    while (1) {
        tc = tc.document()->find(prefix, tc.position(), flags);
        if (tc.isNull())
            break;
        QTextCursor sel = tc;
        sel.select(QTextCursor::WordUnderCursor);
        QString found = textUnderCursor(sel);
        if (found.startsWith(prefix)
                && !m_wordSet.contains(found)
                && tc.anchor() != startPosition) {
            m_wordSet.insert(found);
            appendItem(found,m_icon,true);
        }
        tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
    }
}
