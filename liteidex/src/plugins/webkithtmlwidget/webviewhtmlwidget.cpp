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
// Module: webviewhtmlwidget.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-22
// $Id: webviewhtmlwidget.cpp,v 1.0 2012-11-22 visualfc Exp $

#include "webviewhtmlwidget.h"

#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

WebViewHtmlWidget::WebViewHtmlWidget(QObject *parent) :
    IHtmlWidget(parent)
{
    m_widget = new QWebView;
    connect(m_widget,SIGNAL(linkClicked(QUrl)),this,SIGNAL(linkClicked(QUrl)));
    connect(m_widget->page(),SIGNAL(linkHovered(QString,QString,QString)),this,SLOT(webLinkHovered(QString,QString,QString)));
}

QWidget *WebViewHtmlWidget::widget() const
{
    return m_widget;
}

QString WebViewHtmlWidget::className() const
{
    return "QWebView";
}

void WebViewHtmlWidget::setSerachPaths(const QStringList &paths)
{
}

void WebViewHtmlWidget::setHtml(const QString &html, const QUrl &url)
{
    m_widget->setHtml(html,url);
}

void WebViewHtmlWidget::setMarkdown(const QByteArray &data, const QUrl &url)
{
}

void WebViewHtmlWidget::setText(const QString &text, const QUrl &url)
{

}

void WebViewHtmlWidget::clear()
{
    m_widget->setHtml("");
}

void WebViewHtmlWidget::scroolToAnchor(const QString &anchor)
{
    m_widget->page()->mainFrame()->scrollToAnchor(anchor);
}

QPoint WebViewHtmlWidget::scrollPos() const
{
    return m_widget->page()->mainFrame()->scrollPosition();
}

void WebViewHtmlWidget::setScrollPos(const QPoint &pos)
{
    m_widget->page()->mainFrame()->setScrollPosition(pos);
}

QString WebViewHtmlWidget::selectedText() const
{
    return m_widget->selectedText();
}

bool WebViewHtmlWidget::findText(const QString &exp, QTextDocument::FindFlags options)
{
    QWebPage::FindFlags flag = QWebPage::HighlightAllOccurrences;
    if ( (options & QTextDocument::FindBackward) == QTextDocument::FindBackward) {
        flag |= QWebPage::FindBackward;
    }
    if ( (options & QTextDocument::FindCaseSensitively) == QTextDocument::FindCaseSensitively ) {
        flag |= QWebPage::FindCaseSensitively;
    }
    return m_widget->findText(exp,flag);
}

void WebViewHtmlWidget::webLinkHovered(const QString &link, const QString &title, const QString &textContent)
{
    emit linkHovered(QUrl(link));
}

WebViewHtmlWidgetFactory::WebViewHtmlWidgetFactory(QObject *parent) :
    IHtmlWidgetFactory(parent)
{
}

QString WebViewHtmlWidgetFactory::className() const
{
    return "QWebView";
}

IHtmlWidget *WebViewHtmlWidgetFactory::create(QObject *parent)
{
    return new WebViewHtmlWidget(parent);
}
