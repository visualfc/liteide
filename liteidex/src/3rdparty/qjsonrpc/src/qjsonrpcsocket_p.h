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
#ifndef QJSONRPCSOCKET_P_H
#define QJSONRPCSOCKET_P_H

#include <QPointer>
#include <QHash>
#include <QIODevice>

#include "qjsonrpcsocket.h"
#include "qjsonrpcmessage.h"
#include "qjsonrpcglobal.h"

#if defined(USE_QT_PRIVATE_HEADERS)
#include <private/qobject_p.h>

class QJSONRPC_EXPORT QJsonRpcAbstractSocketPrivate : public QObjectPrivate
#else
class QJSONRPC_EXPORT QJsonRpcAbstractSocketPrivate
#endif
{
public:
    QJsonRpcAbstractSocketPrivate()
        : defaultRequestTimeout(DEFAULT_MSECS_REQUEST_TIMEOUT)
    {}

    int defaultRequestTimeout;

#if !defined(USE_QT_PRIVATE_HEADERS)
    virtual ~QJsonRpcAbstractSocketPrivate() {}
#endif
};

class QJsonRpcServiceReply;
class QJSONRPC_EXPORT QJsonRpcSocketPrivate : public QJsonRpcAbstractSocketPrivate
{
public:
    QJsonRpcSocketPrivate(QJsonRpcSocket *socket)
        : q_ptr(socket)
    {}

#if !defined(USE_QT_PRIVATE_HEADERS)
    virtual ~QJsonRpcSocketPrivate() {}
#endif

    // slots
    virtual void _q_processIncomingData();

    int findJsonDocumentEnd(const QByteArray &jsonData);
    void writeData(const QJsonRpcMessage &message);

    QPointer<QIODevice> device;
    QByteArray buffer;
    QHash<int, QPointer<QJsonRpcServiceReply> > replies;

    QJsonRpcSocket * const q_ptr;
    Q_DECLARE_PUBLIC(QJsonRpcSocket)
};

#endif
