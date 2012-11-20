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
// Module: goproxy.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-17
// $Id: goproxy.h,v 1.0 2012-11-17 visualfc Exp $

#ifndef GOPROXY_H
#define GOPROXY_H

#include "liteapi/liteapi.h"

class GoProxy : public LiteApi::IGoProxy
{
    Q_OBJECT
public:
    explicit GoProxy(QObject *parent = 0);
    virtual bool isValid() const;
    static bool hasProxy();
public slots:
    virtual void call(const QByteArray &id, const QByteArray &args = QByteArray());
public:
    void callback(char *id, char *reply, int len, int err);
};

#endif // GOPROXY_H
