/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: liteeditorapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef __LITEEDITORAPI_H__
#define __LITEEDITORAPI_H__

#include "liteapi/liteapi.h"
#include <QTextCursor>
#include <QCompleter>
#include <QStandardItem>

namespace LiteApi {

class IWordApi
{
public:
    virtual ~IWordApi() {}
    virtual QString mimeType() const = 0;
    virtual QStringList apiFiles() const = 0;
    virtual bool loadApi() = 0;
    virtual QStringList wordList() const = 0;
    virtual QStringList expList() const = 0;
    virtual void appendExp(const QStringList &list) = 0;
};

class IWordApiManager : public IManager
{
    Q_OBJECT
public:
    IWordApiManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addWordApi(IWordApi *wordApi) = 0;
    virtual void removeWordApi(IWordApi *wordApi) = 0;
    virtual IWordApi *findWordApi(const QString &mimeType) = 0;
    virtual QList<IWordApi*> wordApiList() const = 0;
};

class ISnippet
{
public:
    virtual ~ISnippet() {}
    virtual QString trigger() const = 0;
    virtual void setTrigger(const QString &trigger) = 0;
    virtual QString content() const = 0;
    virtual void setContent(const QString &content) = 0;
};

class ISnippetList
{
public:
    virtual ~ISnippetList() {}
    virtual QString mimeType() const = 0;
    virtual bool load() = 0;
    virtual QList<ISnippet*> findSnippet(const QString &trigger, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const = 0;
    virtual QList<ISnippet*> snippetList() const = 0;
};

class ISnippetsManager : public IManager
{
public:
    ISnippetsManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addSnippetList(ISnippetList *snippets) = 0;
    virtual void removeSnippetList(ISnippetList *snippets) = 0;
    virtual ISnippetList *findSnippetList(const QString &mimeType) = 0;
    virtual QList<ISnippetList*> allSnippetList() const = 0;
};

class ICompleter : public QObject
{
    Q_OBJECT
public:
    ICompleter(QObject *parent): QObject(parent) {}
    virtual QCompleter *completer() const = 0;
    virtual QStandardItem *findRoot(const QString &name) = 0;
    virtual void clearChildItem(QStandardItem *root) = 0;
    virtual void appendChildItem(QStandardItem *root,QString name,const QString &kind, const QString &info,const QIcon &icon, bool temp) = 0;
    virtual bool appendItem(const QString &name,const QIcon &icon, bool temp) = 0;
    virtual bool appendItemEx(const QString &name,const QString &kind, const QString &info,const QIcon &icon, bool temp) = 0;
    virtual void appendItems(QStringList items, const QString &kind, const QString &info,const QIcon &icon, bool temp) = 0;
    virtual void clearItemChilds(const QString &name) = 0;
    virtual void clearTemp() = 0;
    virtual void clear() = 0;
    virtual void show() = 0;
    virtual void setSearchSeparator(bool b) = 0;
    virtual bool searchSeparator() const = 0;
signals:
    void prefixChanged(QTextCursor,QString);
    void wordCompleted(const QString &func, const QString &args);
};

// priopity by type value
class IEditorMarkTypeManager : public IManager
{
    Q_OBJECT
public:
    IEditorMarkTypeManager(QObject *parent = 0) : IManager(parent) {}
    virtual void registerMark(int type, const QIcon &icon) = 0;
    virtual QList<int> markTypeList() const = 0;
};

class IEditorMark : public QObject
{
    Q_OBJECT
public:
    IEditorMark(QObject *parent) : QObject(parent) {}
    virtual void addMark(int line, int type) = 0;
    virtual void removeMark(int line, int type) = 0;
    virtual QList<int> markList(int type) const = 0;
    virtual QList<int> lineTypeList(int line) const = 0;
signals:
    void markChanged();
};

} //namespace LiteApi


#endif //__LITEEDITORAPI_H__

