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
#ifndef QJSONRPCTCPSERVER_H
#define QJSONRPCTCPSERVER_H

#include <QTcpServer>
#include "qjsonrpcabstractserver.h"

class QJsonRpcTcpServerPrivate;
class QJSONRPC_EXPORT QJsonRpcTcpServer : public QTcpServer, public QJsonRpcAbstractServer
{
    Q_OBJECT
public:
    explicit QJsonRpcTcpServer(QObject *parent = 0);
    ~QJsonRpcTcpServer();

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
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    virtual void incomingConnection(qintptr socketDescriptor);
#else
    virtual void incomingConnection(int socketDescriptor);
#endif

private Q_SLOTS:
    void _q_clientDisconnected();
    void _q_processMessage(const QJsonRpcMessage &message);

private:
    Q_DECLARE_PRIVATE(QJsonRpcTcpServer)
    Q_DISABLE_COPY(QJsonRpcTcpServer)
#if !defined(USE_QT_PRIVATE_HEADERS)
    QScopedPointer<QJsonRpcTcpServerPrivate> d_ptr;
#endif
};

#endif
