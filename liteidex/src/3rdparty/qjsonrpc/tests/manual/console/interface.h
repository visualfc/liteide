/*
 * Copyright (C) 2012-2013 Matt Broadstone
 * Contact: http://bitbucket.org/devonit/qjsonrpc
 *
 * This file is part of the QJsonRpc Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */
#ifndef QJSONRPCSERVICESCRIPT_H
#define QJSONRPCSERVICESCRIPT_H

#include <QObject>
#include <QVariant>
#include <QScriptClass>
#include <QScriptable>

class QJsonRpcSocket;
class QJsonRpcServiceSocketPrototype : public QObject,
                                       protected QScriptable
{
    Q_OBJECT
public:
    QJsonRpcServiceSocketPrototype(QObject *parent = 0);
    ~QJsonRpcServiceSocketPrototype();

public Q_SLOTS:
    void connectToLocalService(const QString &service);
    QVariant invokeRemoteMethod(const QString &method, const QVariant &arg1 = QVariant(),
                                const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                const QVariant &arg10 = QVariant());

private:
    QJsonRpcSocket *m_socket;

};

#endif

