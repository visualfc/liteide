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
#include "memory.h"

LiteWordCompleter::LiteWordCompleter(QObject *parent) :
    LiteCompleter(parent),
    m_icon(QIcon("icon:liteeditor/images/findword.png"))
{

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
        if (prefix.indexOf(this->separator()) >= 0) {
            return;
        }
    }

    bool isSep = false;
    if (prefix.startsWith("@.")) {
        isSep = true;
        prefix = prefix.mid(1);
    }

    QTextCursor tc = m_editor->textCursor();
    QTextDocument *doc = m_editor->document();
    int maxNumber = tc.blockNumber();
    int blockNumber = tc.blockNumber();
    QTextBlock block = doc->firstBlock();
    if (maxNumber < 500) {
        maxNumber = 500;
    } else {
        int firstNumber = maxNumber-500;
        if (firstNumber > 0) {
            block = doc->findBlockByNumber(firstNumber);
        }
    }
    QRegExp rx("([\\w\\-\\_\\.]+)");
    Qt::CaseSensitivity cs = m_completer->caseSensitivity();
    int count = 0;
    while (block.isValid()) {
        if (block.blockNumber() >= maxNumber) {
            break;
        }
        if (block.blockNumber() == blockNumber) {
            block = block.next();
            continue;
        }
        QString line = block.text().trimmed();
        if (!line.isEmpty())  {
             int pos = 0;
             while ((pos = rx.indexIn(line, pos)) != -1) {
                 QString cap = rx.cap(1);
                 if (cap.length() < 20 && cap.startsWith(prefix,cs)) {
                     if (isSep) {
                         cap = "@"+cap;
                     }
                     count++;
                     appendItem(cap,m_icon,true);
                 }
                 pos += rx.matchedLength();
             }
        }
        block = block.next();
    }
    if (count > 0) {
        this->updateCompleterModel();
        this->showPopup();
    }
}
