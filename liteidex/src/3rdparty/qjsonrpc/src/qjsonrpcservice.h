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
#ifndef QJSONRPCSERVICE_H
#define QJSONRPCSERVICE_H

#include <QVariant>
#include <QPointer>

#include "qjsonrpcmessage.h"

class QJsonRpcAbstractSocket;
class QJsonRpcServiceRequestPrivate;
class QJSONRPC_EXPORT QJsonRpcServiceRequest
{
public:
    QJsonRpcServiceRequest();
    QJsonRpcServiceRequest(const QJsonRpcServiceRequest &other);
    QJsonRpcServiceRequest(const QJsonRpcMessage &request, QJsonRpcAbstractSocket *socket);
    QJsonRpcServiceRequest &operator=(const QJsonRpcServiceRequest &other);
    ~QJsonRpcServiceRequest();

    bool isValid() const;
    QJsonRpcMessage request() const;
    QJsonRpcAbstractSocket *socket() const;

    bool respond(const QJsonRpcMessage &response);
    bool respond(QVariant returnValue);

private:
    QSharedDataPointer<QJsonRpcServiceRequestPrivate> d;
};

class QJsonRpcServiceProvider;
class QJsonRpcServicePrivate;
class QJSONRPC_EXPORT QJsonRpcService : public QObject
{
    Q_OBJECT
public:
    explicit QJsonRpcService(QObject *parent = 0);
    ~QJsonRpcService();

Q_SIGNALS:
    void result(const QJsonRpcMessage &result);
    void notifyConnectedClients(const QJsonRpcMessage &message);
    void notifyConnectedClients(const QString &method, const QJsonArray &params = QJsonArray());

protected:
    QJsonRpcServiceRequest currentRequest() const;
    void beginDelayedResponse();

protected Q_SLOTS:
    QJsonRpcMessage dispatch(const QJsonRpcMessage &request);

private:
    Q_DISABLE_COPY(QJsonRpcService)
    Q_DECLARE_PRIVATE(QJsonRpcService)
    friend class QJsonRpcServiceProvider;

#if !defined(USE_QT_PRIVATE_HEADERS)
    QScopedPointer<QJsonRpcServicePrivate> d_ptr;
#endif

};

#endif

