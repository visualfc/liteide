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
#ifndef QJSONRPCMESSAGE_H
#define QJSONRPCMESSAGE_H

#include <QSharedDataPointer>
#include <QMetaType>

#if QT_VERSION >= 0x050000
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#else
#include "json/qjsonvalue.h"
#include "json/qjsonobject.h"
#include "json/qjsonarray.h"
#endif

#include "qjsonrpcglobal.h"

class QJsonRpcMessagePrivate;
class QJSONRPC_EXPORT QJsonRpcMessage
{
public:
    QJsonRpcMessage();
    QJsonRpcMessage(const QJsonRpcMessage &other);
    QJsonRpcMessage &operator=(const QJsonRpcMessage &other);
    ~QJsonRpcMessage();

#if QT_VERSION >= 0x050000
    inline void swap(QJsonRpcMessage &other) { qSwap(d, other.d); }
#endif

    enum Type {
        Invalid,
        Request,
        Response,
        Notification,
        Error
    };

    static QJsonRpcMessage createRequest(const QString &method,
                                         const QJsonArray &params = QJsonArray());
    static QJsonRpcMessage createRequest(const QString &method, const QJsonValue &param);
    static QJsonRpcMessage createRequest(const QString &method, const QJsonObject &namedParameters);

    static QJsonRpcMessage createNotification(const QString &method,
                                              const QJsonArray &params = QJsonArray());
    static QJsonRpcMessage createNotification(const QString &method, const QJsonValue &param);
    static QJsonRpcMessage createNotification(const QString &method,
                                              const QJsonObject &namedParameters);

    QJsonRpcMessage createResponse(const QJsonValue &result) const;
    QJsonRpcMessage createErrorResponse(QJsonRpc::ErrorCode code,
                                        const QString &message = QString(),
                                        const QJsonValue &data = QJsonValue()) const;

    QJsonRpcMessage::Type type() const;
    bool isValid() const;
    int id() const;

    // request
    QString method() const;
    QJsonValue params() const;

    // response
    QJsonValue result() const;

    // error
    int errorCode() const;
    QString errorMessage() const;
    QJsonValue errorData() const;

    QJsonObject toObject() const;
    static QJsonRpcMessage fromObject(const QJsonObject &object);

    QByteArray toJson() const;
    static QJsonRpcMessage fromJson(const QByteArray &data);

    bool operator==(const QJsonRpcMessage &message) const;
    inline bool operator!=(const QJsonRpcMessage &message) const { return !(operator==(message)); }

private:
    friend class QJsonRpcMessagePrivate;
    QSharedDataPointer<QJsonRpcMessagePrivate> d;

#if QT_VERSION < 0x050000
public:
    typedef QSharedDataPointer<QJsonRpcMessagePrivate> DataPtr;
    inline DataPtr &data_ptr() { return d; }

    // internal
    bool isDetached() const;
#endif
};

QJSONRPC_EXPORT QDebug operator<<(QDebug, const QJsonRpcMessage &);
Q_DECLARE_METATYPE(QJsonRpcMessage)

#if QT_VERSION < 0x050000
Q_DECLARE_TYPEINFO(QJsonRpcMessage, Q_MOVABLE_TYPE);
#endif
Q_DECLARE_SHARED(QJsonRpcMessage)

#endif
