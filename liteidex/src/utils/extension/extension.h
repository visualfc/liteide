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
// Module: extension.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: extension.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef EXTENSION_H
#define EXTENSION_H

#include "liteapi/liteobj.h"

class Extension : public LiteApi::IExtension
{
public:
    virtual void addObject(const QString &meta, QObject *obj);
    virtual void removeObject(const QString &meta);
    virtual QObject *findObject(const QString &meta) const;
    virtual QStringList objectMetaList() const;
protected:
    QHash<QString,QObject*> m_metaObjectHash;
};

#endif //EXTENSION_H
