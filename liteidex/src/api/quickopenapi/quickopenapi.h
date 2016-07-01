/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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

#ifndef __QUICKOPENAPI_H__
#define __QUICKOPENAPI_H__

#include "liteapi/liteapi.h"

namespace LiteApi {

class IQuickOpen : public QObject
{
    Q_OBJECT
public:
    IQuickOpen(QObject *parent = 0) : QObject(parent) {}
    virtual QString id() const = 0;
    virtual QAbstractItemModel *model() const = 0;
    virtual void updateModel() = 0;
    virtual QModelIndex filter(const QString &text) = 0;
    virtual bool selected(const QString &text, const QModelIndex &index) = 0;
};

class IQuickOpenManager : public IManager
{
    Q_OBJECT
public:
    IQuickOpenManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addFilter(const QString &sym, IQuickOpen *filter) = 0;
    virtual void removeFilter(IQuickOpen *filter) = 0;
    virtual QList<IQuickOpen*> filterList() const = 0;
    virtual void setCurrentFilter(IQuickOpen *filter) = 0;
    virtual IQuickOpen *currentFilter() const = 0;
    virtual QModelIndex currentIndex() const = 0;
signals:
    void currentFilterChanged(IQuickOpen *filter);
};

inline IQuickOpenManager *getQuickOpenManager(LiteApi::IApplication* app)
{
    return LiteApi::findExtensionObject<IQuickOpenManager*>(app,"LiteApi.IQuickOpenManager");
}

} //namespace LiteApi


#endif //__QUICKOPENAPI_H__

