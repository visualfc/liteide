/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: webviewhtmlwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef WEBVIEWHTMLWIDGET_H
#define WEBVIEWHTMLWIDGET_H

#include "liteapi/litehtml.h"

using namespace LiteApi;
class QWebView;
class QWebPage;

class WebViewHtmlWidget : public IHtmlWidget
{
    Q_OBJECT
public:
    WebViewHtmlWidget(QObject *parent);
    ~WebViewHtmlWidget();
public:
    virtual QWidget *widget() const;
    virtual QString className() const;
    virtual void setSearchPaths(const QStringList &paths);
    virtual void setHtml(const QString &html, const QUrl &url);
    virtual QUrl url() const;
    virtual void clear();
    virtual void scrollToAnchor(const QString &anchor);
    virtual void setScrollBarValue(Qt::Orientation orientation, int value);
    virtual int scrollBarValue(Qt::Orientation orientation) const;
    virtual int scrollBarMinimum(Qt::Orientation orientation) const;
    virtual int scrollBarMaximum(Qt::Orientation orientation) const;
    virtual QString selectedText() const;
    virtual bool findText(const QString & exp, QTextDocument::FindFlags options = 0 );
public slots:
#ifndef QT_NO_PRINTER
    virtual void print(QPrinter *printer);
#endif
protected slots:
    void webLinkHovered(const QString &link, const QString &title, const QString &textContent);
protected:
    QWebView *m_widget;
};

class WebPageHtmlDocument : public IHtmlDocument
{
    Q_OBJECT
public:
    WebPageHtmlDocument(QObject *parent);
    virtual ~WebPageHtmlDocument();
public:
    virtual void setHtml(const QString &html, const QUrl &url);
public slots:
#ifndef QT_NO_PRINTER
    virtual void print(QPrinter *printer);
#endif
    virtual QString	toHtml () const;
    virtual QString	toPlainText () const;
protected:
    QWebPage *m_doc;
};

class WebViewHtmlWidgetFactory : public IHtmlWidgetFactory
{
    Q_OBJECT
public:
    WebViewHtmlWidgetFactory(QObject *parent = 0);
    virtual QString className() const;
    virtual IHtmlWidget *create(QObject *parent);
    virtual IHtmlDocument  *createDocument(QObject *parent);
};


#endif // WEBVIEWHTMLWIDGET_H
