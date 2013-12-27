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
// Module: textbrowserhtmlwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TEXTBROWSERHTMLWIDGET_H
#define TEXTBROWSERHTMLWIDGET_H

#include "liteapi/litehtml.h"

using namespace LiteApi;

class QTextBrowser;

class TextBrowserHtmlWidget : public IHtmlWidget
{
    Q_OBJECT
public:
    TextBrowserHtmlWidget(QObject *parent);
public:
    virtual QWidget *widget() const;
    virtual QString className() const;
    virtual void setSearchPaths(const QStringList &paths);
    virtual void setHtml(const QString &html, const QUrl &url);
    virtual QUrl url() const;
    virtual void clear();
    virtual void scrollToAnchor(const QString &anchor);
    virtual void setScrollBarValue(Qt::Orientation orientation, int value) ;
    virtual int scrollBarValue(Qt::Orientation orientation) const;
    virtual int scrollBarMinimum(Qt::Orientation orientation) const;
    virtual int scrollBarMaximum(Qt::Orientation orientation) const;
    virtual QString selectedText() const;
    virtual bool findText(const QString & exp, QTextDocument::FindFlags options = 0 );
public slots:
#ifndef QT_NO_PRINTER
    virtual void print(QPrinter *printer);
#endif
protected:
    QTextBrowser *m_widget;
    QUrl          m_url;
};

class TextBrowserHtmlDocument : public IHtmlDocument
{
    Q_OBJECT
public:
    TextBrowserHtmlDocument(QObject *parent);
    virtual ~TextBrowserHtmlDocument();
public:
    virtual void setHtml(const QString &html, const QUrl &url);
public slots:
#ifndef QT_NO_PRINTER
    virtual void print(QPrinter *printer);
#endif
    virtual QString	toHtml () const;
    virtual QString	toPlainText () const;
protected:
    QTextBrowser *m_doc;
};

class TextBrowserHtmlWidgetFactory : public IHtmlWidgetFactory
{
    Q_OBJECT
public:
    TextBrowserHtmlWidgetFactory(QObject *parent = 0);
    virtual QString className() const;
    virtual IHtmlWidget *create(QObject *parent);
    virtual IHtmlDocument  *createDocument(QObject *parent);
};

#endif // TEXTBROWSERHTMLWIDGET_H
