/*
 * Copyright (C) 2012-2014 Matt Broadstone
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
#ifndef QJSONRPCSERVICEPROVIDER_H
#define QJSONRPCSERVICEPROVIDER_H

#include "qjsonrpcglobal.h"

class QJsonRpcMessage;
class QJsonRpcService;
class QJsonRpcAbstractSocket;
class QJsonRpcServiceProviderPrivate;
class QJSONRPC_EXPORT QJsonRpcServiceProvider
{
public:
    ~QJsonRpcServiceProvider();
    virtual bool addService(QJsonRpcService *service);
    virtual bool removeService(QJsonRpcService *service);

protected:
    QJsonRpcServiceProvider();
    void processMessage(QJsonRpcAbstractSocket *socket, const QJsonRpcMessage &message);

private:
    QScopedPointer<QJsonRpcServiceProviderPrivate> d;

};

#endif // QJSONRPCSERVICEPROVIDER_H
