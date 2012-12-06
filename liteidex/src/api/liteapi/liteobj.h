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
// Module: liteobj.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteobj.h,v 1.0 2011-4-22 visualfc Exp $

#ifndef __LITEOBJ_H__
#define __LITEOBJ_H__

#include <QObject>
#include <QStringList>
#include <QHash>

namespace LiteApi {

class IObject;
class IExtension
{
public:
    virtual ~IExtension() {}
    virtual void addObject(const QString &meta, QObject *obj) = 0;
    virtual void removeObject(const QString &meta) = 0;
    virtual QObject *findObject(const QString &meta) const = 0;
    virtual QStringList objectMetaList() const = 0;
};

class IFactory
{
public:
    virtual ~IFactory() {}
    virtual IObject *newObject(const QString &meta);
};

class IObject : public QObject
{
public:
    IObject(QObject *parent = 0) : QObject(parent) {}
    virtual ~IObject() {}
    virtual IExtension *extension() { return 0; }
    virtual IFactory *factory() { return 0; }
};

template <typename T>
inline T findExtensionObject(IObject *obj, const QString & meta)
{
    IExtension *ext = obj->extension();
    if (!ext) {
        return 0;
    }
    QObject *t = ext->findObject(meta);
    if (!t) {
        return 0;
    }
    return static_cast<T>(t);
}


template <typename T>
inline T findExtensionObject(IExtension *ext, const QString & meta)
{
    QObject *t = ext->findObject(meta);
    if (!t) {
        return 0;
    }
    return static_cast<T>(t);
}

} //LiteApi

#endif // __LITEOBJ_H__
