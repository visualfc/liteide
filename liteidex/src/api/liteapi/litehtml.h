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
// Module: litehtml.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEHTML_H
#define LITEHTML_H

#include <QWidget>
#include <QUrl>
#include <QTextDocument>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

namespace LiteApi {

class IHtmlWidget : public QObject
{
    Q_OBJECT
public:
    IHtmlWidget(QObject *parent) :QObject(parent){}
    virtual ~IHtmlWidget() {}
public:
    virtual QWidget *widget() const = 0;
    virtual QString className() const = 0;
    virtual void setSearchPaths(const QStringList &paths) = 0;
    virtual void setHtml(const QString &html, const QUrl &url) = 0;
    virtual QUrl url() const = 0;
    virtual void clear() = 0;
    virtual void scrollToAnchor(const QString &anchor) = 0;
    virtual void setScrollBarValue(Qt::Orientation orientation, int value) = 0;
    virtual int scrollBarValue(Qt::Orientation orientation) const = 0;
    virtual int scrollBarMinimum(Qt::Orientation orientation) const = 0;
    virtual int scrollBarMaximum(Qt::Orientation orientation) const = 0;
    virtual QString selectedText() const = 0;
    virtual bool findText(const QString & exp, QTextDocument::FindFlags options = 0 ) = 0;
public slots:
#ifndef QT_NO_PRINTER
    virtual void print(QPrinter *printer) = 0;
#endif
signals:
    void contentsSizeChanged();
    void loadFinished(bool);
    void anchorChanged(const QString & anchor);
    void linkClicked(const QUrl & url);
    void linkHovered(const QUrl & url);
};

//html document util
class IHtmlDocument : public QObject
{
    Q_OBJECT
public:
    IHtmlDocument(QObject *parent) : QObject(parent){}
    virtual ~IHtmlDocument() {}
public:
    virtual void setHtml(const QString &html, const QUrl &url) = 0;
public slots:
#ifndef QT_NO_PRINTER
    virtual void print(QPrinter *printer) = 0;
#endif
    virtual QString	toHtml () const = 0;
    virtual QString	toPlainText () const = 0;
signals:
    void loadFinished(bool);
};

class IHtmlWidgetFactory : public QObject
{
    Q_OBJECT
public:
    IHtmlWidgetFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QString className() const = 0;
    virtual IHtmlWidget *create(QObject *parent) = 0;
    virtual IHtmlDocument  *createDocument(QObject *parent) = 0;
};

// QTextBrowser and QWebView
class IHtmlWidgetManager : public QObject
{
    Q_OBJECT
public:
    IHtmlWidgetManager(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList classNameList() const = 0;
    virtual void addFactory(IHtmlWidgetFactory *factory) = 0;
    virtual QList<IHtmlWidgetFactory*> factoryList() const = 0;
    virtual bool setDefaultClassName(const QString &className) = 0;
    virtual QString defaultClassName() const = 0;
    virtual IHtmlWidget *create(QObject *parent) = 0;
    virtual IHtmlWidget *createByName(QObject *parent, const QString &className) = 0;
    virtual IHtmlDocument *createDocument(QObject *parent) = 0;
    virtual IHtmlDocument *createDocumentByName(QObject *parent, const QString &className) = 0;
};

} //namespace LiteApi

#endif // LITEHTML_H
