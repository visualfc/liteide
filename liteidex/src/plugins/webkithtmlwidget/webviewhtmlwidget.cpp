/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
    m_widget->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(m_widget,SIGNAL(linkClicked(QUrl)),this,SIGNAL(linkClicked(QUrl)));
    connect(m_widget->page(),SIGNAL(linkHovered(QString,QString,QString)),this,SLOT(webLinkHovered(QString,QString,QString)));
    connect(m_widget->page(),SIGNAL(loadFinished(bool)),this,SIGNAL(loadFinished(bool)));
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

QUrl WebViewHtmlWidget::url() const
{
    return m_widget->url();
}

void WebViewHtmlWidget::clear()
{
    m_widget->setHtml("");
}

void WebViewHtmlWidget::scroolToAnchor(const QString &anchor)
{
    m_widget->page()->mainFrame()->scrollToAnchor(anchor);
}

void WebViewHtmlWidget::setScrollBarValue(Qt::Orientation orientation, int value)
{
    m_widget->page()->mainFrame()->setScrollBarValue(orientation,value);
}

int WebViewHtmlWidget::scrollBarValue(Qt::Orientation orientation) const
{
    return m_widget->page()->mainFrame()->scrollBarValue(orientation);
}

int WebViewHtmlWidget::scrollBarMinimum(Qt::Orientation orientation) const
{
    return m_widget->page()->mainFrame()->scrollBarMinimum(orientation);
}

int WebViewHtmlWidget::scrollBarMaximum(Qt::Orientation orientation) const
{
    return m_widget->page()->mainFrame()->scrollBarMaximum(orientation);
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

#ifndef QT_NO_PRINTER
void WebViewHtmlWidget::print(QPrinter *printer)
{
    m_widget->print(printer);
}
#endif

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

IHtmlDocument *WebViewHtmlWidgetFactory::createDocument(QObject *parent)
{
    return new WebPageHtmlDocument(parent);
}


WebPageHtmlDocument::WebPageHtmlDocument(QObject *parent) :
    IHtmlDocument(parent)
{
    m_doc = new QWebPage(this);
    connect(m_doc->mainFrame(),SIGNAL(loadFinished(bool)),this,SIGNAL(loadFinished(bool)));
}

WebPageHtmlDocument::~WebPageHtmlDocument()
{
    delete m_doc;
}

void WebPageHtmlDocument::setHtml(const QString &html, const QUrl &url)
{
    m_doc->mainFrame()->setHtml(html,url);
}

#ifndef QT_NO_PRINTER
void WebPageHtmlDocument::print(QPrinter *printer)
{
    m_doc->mainFrame()->print(printer);
}
#endif

QString WebPageHtmlDocument::toHtml() const
{
    return m_doc->mainFrame()->toHtml();
}

QString WebPageHtmlDocument::toPlainText() const
{
    return m_doc->mainFrame()->toPlainText();
}
