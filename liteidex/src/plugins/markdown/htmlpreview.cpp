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
#include <QScrollBar>
#include <QAction>
#include <QFileInfo>
#include <QTextCodec>
#include <QFile>
#include <QUrl>
#include <QVBoxLayout>

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
    m_curEditor = 0;
    m_widget = new QWidget;
    m_htmlWidget = 0;
    m_toolAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::RightDockWidgetArea,
                                                  m_widget,
                                                  QString("HtmlPreview"),
                                                  QString(tr("Html Preview")),
                                                  true);
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_toolAct,SIGNAL(toggled(bool)),this,SLOT(triggered(bool)));
}

void HtmlPreview::appLoaded()
{
    m_htmlWidget = m_liteApp->htmlWidgetManager()->create(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_htmlWidget->widget());
    m_widget->setLayout(layout);
    loadHeadData(m_liteApp->resourcePath()+"/markdown/style.css");
}

static QByteArray head1 =
"<html>"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>"
"<style type=\"text/css\">";

static QByteArray head2 =
"</style>"
"</head>"
"<body>";

static QByteArray end =
"</body>"
"</html>";

void HtmlPreview::loadHeadData(const QString &css)
{
    QFile f(css);
    if (!f.open(QFile::ReadOnly)) {
        m_head = head1+head2;
        return;
    }
    m_head = head1+f.readAll()+head2;
}

void HtmlPreview::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!m_toolAct->isChecked()) {
        return;
    }

    if (m_curEditor != 0) {
        m_curEditor->disconnect(this);
    }
    if (editor &&
        ( (editor->mimeType() == "text/x-markdown") ||
            (editor->mimeType() == "text/html")) )  {
        LiteApi::ITextEditor *ed = LiteApi::getTextEditor(editor);
        if (ed) {
            m_curEditor = ed;
            connect(m_curEditor,SIGNAL(contentsChanged()),this,SLOT(editorHtmlPrivew()));
            editorHtmlPrivew();
        }
        QPlainTextEdit *pte = LiteApi::getPlainTextEdit(editor);
        if (pte) {
            pte->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        }
    } else {
        m_curEditor = 0;
        m_lastData.clear();
        if (m_htmlWidget) {
            m_htmlWidget->clear();
        }
    }
}

void HtmlPreview::editorHtmlPrivew()
{
    if (!m_curEditor || !m_htmlWidget) {
        return;
    }

    QByteArray data = m_curEditor->utf8Data();
    if (m_lastData == data) {
        return;
    }
    m_lastData = data;

    QPoint pos = m_htmlWidget->scrollPos();
    if (m_curEditor->mimeType() == "text/html") {
        QTextCodec *codec = QTextCodec::codecForHtml(data,QTextCodec::codecForName("utf-8"));
        m_htmlWidget->setHtml(codec->toUnicode(data),QUrl::fromLocalFile(m_curEditor->filePath()));
    } else {
        QString html = QString::fromUtf8(mdtohtml(data));
        m_htmlWidget->setHtml(m_head+html+end,QUrl::fromLocalFile(m_curEditor->filePath()));
    }
    m_htmlWidget->setScrollPos(pos);
}

void HtmlPreview::triggered(bool b)
{
    if (b) {
        currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    } else if (m_curEditor){
        m_lastData.clear();
        m_curEditor->disconnect(this);
    }
}
