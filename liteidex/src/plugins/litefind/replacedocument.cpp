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
// Module: replacedocument.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "replacedocument.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

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

bool ReplaceDocument::replace(const QString &fileName, const QString &text, const QList<Find::SearchResultItem> &items)
{
    QTextDocument *doc = fileDocument(fileName);
    if (!doc) {
        return false;
    }
    return false;
}

QTextDocument *ReplaceDocument::fileDocument(const QString &fileName)
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,true);
    if (editor) {
        QPlainTextEdit *ed = LiteApi::getPlainTextEdit(editor);
        if (ed) {
            m_cursor = ed->textCursor();
            return ed->document();
        }
    }
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        m_document = new QTextDocument(QString::fromUtf8(data));
        m_cursor = QTextCursor(m_document);
        return m_document;
    }
    return 0;
}
