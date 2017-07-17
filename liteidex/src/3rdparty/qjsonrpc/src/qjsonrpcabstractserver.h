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
#ifndef QJSONRPCABSTRACTSERVER_H
#define QJSONRPCABSTRACTSERVER_H

#include "qjsonrpcserviceprovider.h"
#include "qjsonrpcglobal.h"

class QJsonArray;
class QJsonRpcMessage;
class QJsonRpcAbstractServerPrivate;
class QJSONRPC_EXPORT QJsonRpcAbstractServer : public QJsonRpcServiceProvider
{
public:
    virtual ~QJsonRpcAbstractServer();
    virtual int connectedClientCount() const = 0;

// Q_SIGNALS:
    virtual void clientConnected() = 0;
    virtual void clientDisconnected() = 0;

// public Q_SLOTS:
    virtual void notifyConnectedClients(const QJsonRpcMessage &message) = 0;
    virtual void notifyConnectedClients(const QString &method, const QJsonArray &params) = 0;

};

#endif
