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
// Module: quickopenapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef QUICKOPENAPI_H
#define QUICKOPENAPI_H

#include "liteapi/liteapi.h"

class QTreeView;
class QLineEdit;
namespace LiteApi {

class IQuickOpen : public QObject
{
    Q_OBJECT
public:
    IQuickOpen(QObject *parent = 0) : QObject(parent) {}
    virtual QString id() const = 0;
    virtual QString info() const = 0;
    virtual QString placeholderText() const = 0;
    virtual void activate() = 0;
    virtual QAbstractItemModel *model() const = 0;
    virtual QModelIndex rootIndex() const = 0;
    virtual void updateModel() = 0;
    virtual QModelIndex filterChanged(const QString &text) = 0;
    virtual void indexChanged(const QModelIndex &index) = 0;
    virtual bool selected(const QString &text, const QModelIndex &index) = 0;
    virtual void cancel() = 0;
};

class IQuickOpenFolder : public IQuickOpen
{
    Q_OBJECT
public:
    IQuickOpenFolder(QObject *parent = 0) : IQuickOpen(parent) {}
    virtual void setFolder(const QString &folder) = 0;
    virtual void setPlaceholderText(const QString &text) = 0;
};

class IQuickOpenFileSystem : public IQuickOpen
{
public:
    IQuickOpenFileSystem(QObject *parent = 0) : IQuickOpen(parent) {}
    virtual void setRootPath(const QString &root) = 0;
    virtual void setPlaceholderText(const QString &text) = 0;
    virtual QModelIndex indexForPath(const QString &indexForPath) const = 0;
    virtual QString pathForIndex(const QModelIndex &index) const = 0;
};

class IQuickOpenAdapter : public QObject
{
    Q_OBJECT
public:
    IQuickOpenAdapter(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList mimeTypes() const = 0;
    virtual IQuickOpen *load(const QString &mimeType) = 0;
};

class IQuickOpenMimeType : public LiteApi::IQuickOpen
{
public:
    IQuickOpenMimeType(QObject *parent) : LiteApi::IQuickOpen(parent) {}
    virtual void addAdapter(LiteApi::IQuickOpenAdapter *factory) = 0;
    virtual void setId(const QString &id) = 0;
    virtual void setInfo(const QString &info) = 0;
    virtual void setNoFoundMessage(const QString &message) = 0;
};

class IQuickOpenManager : public IManager
{
    Q_OBJECT
public:
    IQuickOpenManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addFilter(const QString &sym, IQuickOpen *filter) = 0;
    virtual void removeFilter(IQuickOpen *filter) = 0;
    virtual QList<IQuickOpen*> filterList() const = 0;
    virtual QMap<QString,IQuickOpen*> symFilterMap() const = 0;
    virtual void setCurrentFilter(IQuickOpen *filter) = 0;
    virtual IQuickOpen *currentFilter() const = 0;
    virtual QModelIndex currentIndex() const = 0;
    virtual void showById(const QString &id) = 0;
    virtual void showBySymbol(const QString &sym) = 0;
    virtual IQuickOpen *findById(const QString &id) = 0;
    virtual IQuickOpen *findBySymbol(const QString &sym) = 0;
    virtual QWidget *widget() const = 0;
    virtual QTreeView *modelView() const = 0;
    virtual QLineEdit *lineEdit() const = 0;
    virtual void setTempToolBar(QToolBar *tooBar) = 0;
public:
    virtual void showPopup(QPoint *pos = 0) = 0;
    virtual void hidePopup() = 0;
public:
    virtual IQuickOpenMimeType *registerQuickOpenMimeType(const QString &sym) = 0;
signals:
    void currentFilterChanged(IQuickOpen *filter);
};

inline IQuickOpenManager *getQuickOpenManager(LiteApi::IApplication* app)
{
    return LiteApi::findExtensionObject<IQuickOpenManager*>(app,"LiteApi.IQuickOpenManager");
}

inline IQuickOpenFolder *getQuickOpenFolder(LiteApi::IQuickOpenManager *mgr)
{
    return (IQuickOpenFolder*)mgr->findById("quickopen/folder");
}

inline IQuickOpenFileSystem *getQuickOpenFileSystem(LiteApi::IQuickOpenManager *mgr)
{
    return (IQuickOpenFileSystem*)mgr->findById("quickopen/filesystem");
}


} //namespace LiteApi


#endif //QUICKOPENAPI_H

