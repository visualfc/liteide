/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: htmlpreview.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-21
// $Id: htmlpreview.cpp,v 1.0 2012-11-21 visualfc Exp $

#include "htmlpreview.h"
#include "sundown/mdtohtml.h"
#include <QTextBrowser>
#include <QScrollBar>
#include <QAction>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

HtmlPreview::HtmlPreview(LiteApi::IApplication *app,QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_browser = new QTextBrowser;
    m_curEditor = 0;
    m_toolAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::RightDockWidgetArea,
                                                  m_browser,
                                                  QString("MarkdownView"),
                                                  QString(tr("Markdown View")),
                                                  true);
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_toolAct,SIGNAL(toggled(bool)),this,SLOT(triggered(bool)));
}

void HtmlPreview::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!m_toolAct->isChecked()) {
        return;
    }

    if (m_curEditor != 0) {
        m_curEditor->disconnect(this);
    }
    if (editor && editor->mimeType() == "text/x-markdown") {
        LiteApi::ITextEditor *ed = LiteApi::getTextEditor(editor);
        if (ed) {
            m_curEditor = ed;
            connect(m_curEditor,SIGNAL(contentsChanged()),this,SLOT(editorHtmlPrivew()));
            editorHtmlPrivew();
        }
    } else {
        m_curEditor = 0;
        m_browser->clear();
    }
}

void HtmlPreview::editorHtmlPrivew()
{
    if (!m_curEditor) {
        return;
    }

    QByteArray data = m_curEditor->utf8Data();
    if (m_lastData == data) {
        return;
    }
    m_lastData = data;

    QByteArray html = mdtohtml(m_lastData);

    int v = m_browser->verticalScrollBar()->value();

    m_browser->setHtml(html);

    m_browser->verticalScrollBar()->setValue(v);
}

void HtmlPreview::triggered(bool b)
{
    if (b) {
        currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    } else if (m_curEditor){
        m_curEditor->disconnect(this);
    }
}
