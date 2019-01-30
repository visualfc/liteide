/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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

#ifndef GOPROXY_H
#define GOPROXY_H

#include "liteapi/liteapi.h"
#include <QMap>

class GoProxy : public LiteApi::IGoProxy
{
    Q_OBJECT
public:
    explicit GoProxy(QObject *parent = 0);
    static bool hasProxy();
    virtual bool isValid() const;
    virtual bool isRunning() const;
    virtual QByteArray commandId() const;
    virtual void writeStdin(const QByteArray &data);
public slots:
    virtual void call(const QByteArray &id, const QByteArray &args = QByteArray());
public:
    void callback(char *id, int id_size, char *reply, int reply_size, int err);
protected:
    bool m_isRuning;
    QByteArray m_id;
};

#endif // GOPROXY_H
