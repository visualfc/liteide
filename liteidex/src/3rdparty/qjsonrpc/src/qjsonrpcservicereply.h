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
#ifndef QJSONRPCSERVICEREPLY_H
#define QJSONRPCSERVICEREPLY_H

#include <QObject>
#include <QNetworkReply>

#include "qjsonrpcglobal.h"
#include "qjsonrpcmessage.h"

class QJsonRpcServiceReplyPrivate;
class QJSONRPC_EXPORT QJsonRpcServiceReply : public QObject
{
    Q_OBJECT
public:
    explicit QJsonRpcServiceReply(QObject *parent = 0);
    virtual ~QJsonRpcServiceReply();

    QJsonRpcMessage request() const;
    QJsonRpcMessage response() const;

Q_SIGNALS:
    void finished();

protected:
    Q_DECLARE_PRIVATE(QJsonRpcServiceReply)
    Q_DISABLE_COPY(QJsonRpcServiceReply)
    QJsonRpcServiceReply(QJsonRpcServiceReplyPrivate &dd, QObject *parent = 0);
    friend class QJsonRpcSocketPrivate;
    friend class QJsonRpcSocket;

#if !defined(USE_QT_PRIVATE_HEADERS)
    QScopedPointer<QJsonRpcServiceReplyPrivate> d_ptr;
#endif

};

#endif // QJSONRPCSERVICEREPLY_H
