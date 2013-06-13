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
// Module: litettyapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef __LITETTYAPI_H__
#define __LITETTYAPI_H__

#include "liteapi/liteapi.h"
#include <QProcessEnvironment>
#include <QDir>

namespace LiteApi {

class ITty : public QObject
{
    Q_OBJECT
public:
    ITty(QObject *parent = 0): QObject(parent) {}
    virtual QString serverName() const = 0;
    virtual QString errorString() const = 0;
    virtual bool listen() = 0;
    virtual void shutdown() = 0;
signals:
    void byteDelivery(const QByteArray &data);
};

class ILiteTty : public QObject
{
public:
    ILiteTty(QObject *parent) : QObject(parent) { }
    virtual ITty* createTty(QObject *parent) const = 0;
};

inline ILiteTty *getLiteTty(LiteApi::IApplication* app)
{
    return LiteApi::findExtensionObject<ILiteTty*>(app,"LiteApi.ILiteTty");
}

inline ITty *createTty(LiteApi::IApplication *app,QObject *parent)
{
    ILiteTty *liteTty = getLiteTty(app);
    if (liteTty) {
        return liteTty->createTty(parent);
    }
    return 0;
}

} //namespace LiteApi


#endif //__LITETTYAPI_H__

