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
#ifndef QJSONRPCLOCALSERVER_H
#define QJSONRPCLOCALSERVER_H

#include <QLocalServer>
#include "qjsonrpcabstractserver.h"

class QJsonRpcLocalServerPrivate;
class QJSONRPC_EXPORT QJsonRpcLocalServer : public QLocalServer, public QJsonRpcAbstractServer
{
    Q_OBJECT
public:
    explicit QJsonRpcLocalServer(QObject *parent = 0);
    ~QJsonRpcLocalServer();

    virtual int connectedClientCount() const;

    // reimp
    bool addService(QJsonRpcService *service);
    bool removeService(QJsonRpcService *service);

Q_SIGNALS:
    void clientConnected();
    void clientDisconnected();

public Q_SLOTS:
    void notifyConnectedClients(const QJsonRpcMessage &message);
    void notifyConnectedClients(const QString &method, const QJsonArray &params);

protected:
    virtual void incomingConnection(quintptr socketDescriptor);

private Q_SLOTS:
    void _q_clientDisconnected();
    void _q_processMessage(const QJsonRpcMessage &message);

private:
    Q_DECLARE_PRIVATE(QJsonRpcLocalServer)
    Q_DISABLE_COPY(QJsonRpcLocalServer)
#if !defined(USE_QT_PRIVATE_HEADERS)
    QScopedPointer<QJsonRpcLocalServerPrivate> d_ptr;
#endif
};

#endif
