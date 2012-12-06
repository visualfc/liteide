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
// Module: textbrowserhtmlwidget.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-22
// $Id: textbrowserhtmlwidget.h,v 1.0 2012-11-22 visualfc Exp $

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
    virtual void setSerachPaths(const QStringList &paths);
    virtual void setHtml(const QString &html, const QUrl &url);
    virtual QUrl url() const;
    virtual void clear();
    virtual void scroolToAnchor(const QString &anchor);
    virtual void setScrollBarValue(Qt::Orientation orientation, int value) ;
    virtual int scrollBarValue(Qt::Orientation orientation) const;
    virtual int scrollBarMinimum(Qt::Orientation orientation) const;
    virtual int scrollBarMaximum(Qt::Orientation orientation) const;
    virtual QString selectedText() const;
    virtual bool findText(const QString & exp, QTextDocument::FindFlags options = 0 );
signals:
    void linkClicked(const QUrl & url);
    void linkHovered(const QUrl & url);
protected:
    QTextBrowser *m_widget;
    QUrl          m_url;
};

class TextBrowserHtmlWidgetFactory : public IHtmlWidgetFactory
{
    Q_OBJECT
public:
    TextBrowserHtmlWidgetFactory(QObject *parent = 0);
    virtual QString className() const;
    virtual IHtmlWidget *create(QObject *parent);
};

#endif // TEXTBROWSERHTMLWIDGET_H
