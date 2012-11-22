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
// Module: litehtml.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-22
// $Id: litehtml.h,v 1.0 2012-11-22 visualfc Exp $

#ifndef LITEHTML_H
#define LITEHTML_H

#include <QWidget>
#include <QUrl>

namespace LiteApi {

class IHtmlWidget : public QObject
{
    Q_OBJECT
public:
    IHtmlWidget(QObject *parent) :QObject(parent){}
    virtual ~IHtmlWidget() {}
public:
    virtual QWidget *widget() = 0;
    virtual QString className() const = 0;
    virtual void setHtml(const QString &html, const QUrl &url) = 0;
    virtual void setMarkdown(const QByteArray &data, const QUrl &url) = 0;
    virtual void setText(const QString &text, const QUrl &url) = 0;
    virtual void clear() = 0;
    virtual void scroolToAnchor(const QString &anchor) = 0;
    virtual QPoint scrollPos() const = 0;
    virtual void setScrollPos(const QPoint &pos) = 0;
signals:
    void linkClicked(const QUrl & url);
    void linkHovered(const QUrl & url);
};

class IHtmlWidgetFactory : public QObject
{
    Q_OBJECT
public:
    IHtmlWidgetFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QString className() const = 0;
    virtual IHtmlWidget *create(QObject *parent) = 0;
};

// QTextBrowser and QWebView
class IHtmlWidgetManager : public QObject
{
    Q_OBJECT
public:
    IHtmlWidgetManager(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList classNameList() const = 0;
    virtual void addFactory(IHtmlWidgetFactory *factory) = 0;
    virtual IHtmlWidget *create(QObject *parent, const QString &className = "") = 0;
};

} //namespace LiteApi

#endif // LITEHTML_H
