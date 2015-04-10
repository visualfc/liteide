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
// Module: replacedocument.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "replacedocument.h"
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include "memory.h"

ReplaceDocument::ReplaceDocument(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent), m_liteApp(app), m_document(0)
{
}

ReplaceDocument::~ReplaceDocument()
{
    if (m_document) {
        delete m_document;
    }
}

QList<Find::SearchResultItem> ReplaceDocument::replace(const QString &fileName, const QString &text, const QList<Find::SearchResultItem> &items)
{
    QTextCursor cursor;
    bool crlf = false;
    QTextDocument *doc = fileDocument(fileName,cursor,crlf);
    if (!doc) {
        return QList<Find::SearchResultItem>();
    }
    cursor.movePosition(QTextCursor::Start);
    cursor.beginEditBlock();
    QList<Find::SearchResultItem> update_items;
    QTextBlock block = cursor.block();
    int offset = 0;
    foreach(Find::SearchResultItem item, items) {
        if (!block.isValid()) {
            break;
        }
        while (block.blockNumber() < item.lineNumber-1 ) {
            block = block.next();
            offset = 0;
            if (!block.isValid()) {
                break;
            }
        }
        cursor.setPosition(block.position());
        cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,offset+item.textMarkPos);
        cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,item.textMarkLength);
        cursor.removeSelectedText();
        cursor.insertText(text);
        item.textMarkPos -= offset;
        item.text.replace(item.textMarkPos,item.textMarkLength,text);
        offset += text.length()-item.textMarkLength;
        item.textMarkLength = text.length();
        update_items.push_back(item);
    }
    cursor.endEditBlock();
    if (m_document) {
        QFile f(fileName);
        if (!f.open(QFile::WriteOnly)) {
            return QList<Find::SearchResultItem>();
        }
        QString text = m_document->toPlainText();
        if (crlf) {
            text.replace(QLatin1Char('\n'), QLatin1String("\r\n"));
        }
        f.write(text.toUtf8());
    }
    return update_items;
}

QTextDocument *ReplaceDocument::fileDocument(const QString &fileName, QTextCursor &cursor, bool &crlf)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,true);
    if (editor) {
        QPlainTextEdit *ed = LiteApi::getPlainTextEdit(editor);
        if (ed) {
            cursor = ed->textCursor();
            return ed->document();
        }
    }
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        QString text = QString::fromUtf8(data);
        int lf = text.indexOf('\n');
        if (lf <= 0) {
            crlf = false;
        } else {
            lf = text.indexOf(QRegExp("[^\r]\n"),lf-1);
            if (lf >= 0) {
                crlf = false;
            } else {
                crlf = true;
            }
        }
        m_document = new QTextDocument(text);
        cursor = QTextCursor(m_document);
        return m_document;
    }
    return 0;
}
