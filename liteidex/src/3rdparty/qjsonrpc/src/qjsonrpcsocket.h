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
#ifndef QJSONRPCSOCKET_H
#define QJSONRPCSOCKET_H

#include <QObject>
#include <QIODevice>

#include "qjsonrpcabstractserver.h"
#include "qjsonrpcservice.h"
#include "qjsonrpcmessage.h"
#include "qjsonrpcglobal.h"

#define DEFAULT_MSECS_REQUEST_TIMEOUT (30000)

class QJsonRpcServiceReply;
class QJsonRpcAbstractSocketPrivate;
class QJSONRPC_EXPORT QJsonRpcAbstractSocket : public QObject
{
    Q_OBJECT
public:
    explicit QJsonRpcAbstractSocket(QObject *parent = 0);
    ~QJsonRpcAbstractSocket();

    virtual bool isValid() const;
    void setDefaultRequestTimeout(int msecs);
    int getDefaultRequestTimeout() const;

Q_SIGNALS:
    void messageReceived(const QJsonRpcMessage &message);

public Q_SLOTS:
    virtual void notify(const QJsonRpcMessage &message) = 0;
    virtual QJsonRpcMessage sendMessageBlocking(const QJsonRpcMessage &message, int msecs = DEFAULT_MSECS_REQUEST_TIMEOUT);
    virtual QJsonRpcServiceReply *sendMessage(const QJsonRpcMessage &message);
    virtual QJsonRpcMessage invokeRemoteMethodBlocking(const QString &method, int msecs, const QVariant &arg1 = QVariant(),
                                               const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                               const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                               const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                               const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                               const QVariant &arg10 = QVariant());
    virtual QJsonRpcMessage invokeRemoteMethodBlocking(const QString &method, const QVariant &arg1 = QVariant(),
                                               const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                               const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                               const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                               const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                               const QVariant &arg10 = QVariant());
    virtual QJsonRpcServiceReply *invokeRemoteMethod(const QString &method, const QVariant &arg1 = QVariant(),
                                             const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                             const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                             const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                             const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                             const QVariant &arg10 = QVariant());
protected:
    QJsonRpcAbstractSocket(QJsonRpcAbstractSocketPrivate &dd, QObject *parent = 0);

#if !defined(USE_QT_PRIVATE_HEADERS)
    QScopedPointer<QJsonRpcAbstractSocketPrivate> d_ptr;
#endif

private:
    Q_DECLARE_PRIVATE(QJsonRpcAbstractSocket)
    Q_DISABLE_COPY(QJsonRpcAbstractSocket)

};

class QJsonRpcSocketPrivate;
class QJSONRPC_EXPORT QJsonRpcSocket : public QJsonRpcAbstractSocket
{
    Q_OBJECT
public:
    explicit QJsonRpcSocket(QIODevice *device, QObject *parent = 0);
    ~QJsonRpcSocket();

    virtual bool isValid() const;

public Q_SLOTS:
    virtual void notify(const QJsonRpcMessage &message);
    virtual QJsonRpcMessage sendMessageBlocking(const QJsonRpcMessage &message, int msecs = DEFAULT_MSECS_REQUEST_TIMEOUT);
    virtual QJsonRpcServiceReply *sendMessage(const QJsonRpcMessage &message);
    QJsonRpcMessage invokeRemoteMethodBlocking(const QString &method, int msecs, const QVariant &arg1 = QVariant(),
                                               const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                               const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                               const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                               const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                               const QVariant &arg10 = QVariant());
    QJsonRpcMessage invokeRemoteMethodBlocking(const QString &method, const QVariant &arg1 = QVariant(),
                                               const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                               const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                               const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                               const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                               const QVariant &arg10 = QVariant());
    QJsonRpcServiceReply *invokeRemoteMethod(const QString &method, const QVariant &arg1 = QVariant(),
                                             const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                             const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                             const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                             const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                             const QVariant &arg10 = QVariant());

protected:
    QJsonRpcSocket(QJsonRpcSocketPrivate &dd, QObject *parent);
    virtual void processRequestMessage(const QJsonRpcMessage &message);

private:
    Q_DECLARE_PRIVATE(QJsonRpcSocket)
    Q_DISABLE_COPY(QJsonRpcSocket)
    Q_PRIVATE_SLOT(d_func(), void _q_processIncomingData())

#if !defined(USE_QT_PRIVATE_HEADERS)
    QScopedPointer<QJsonRpcSocketPrivate> d_ptr;
#endif
};

class QJSONRPC_EXPORT QJsonRpcServiceSocket : public QJsonRpcSocket, public QJsonRpcServiceProvider
{
    Q_OBJECT
public:
    explicit QJsonRpcServiceSocket(QIODevice *device, QObject *parent = 0);
    ~QJsonRpcServiceSocket();

private:
    virtual void processRequestMessage(const QJsonRpcMessage &message);

};

#endif
