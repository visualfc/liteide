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

#include <QDebug>

#if QT_VERSION >= 0x050000
#   include <QJsonDocument>
#else
#   include "json/qjsondocument.h"
#endif

#include "qjsonrpcmessage.h"

class QJsonRpcMessagePrivate : public QSharedData
{
public:
    QJsonRpcMessagePrivate();
    ~QJsonRpcMessagePrivate();
    QJsonRpcMessagePrivate(const QJsonRpcMessagePrivate &other);

    void initializeWithObject(const QJsonObject &message);
    static QJsonRpcMessage createBasicRequest(const QString &method, const QJsonArray &params);
    static QJsonRpcMessage createBasicRequest(const QString &method,
                                              const QJsonObject &namedParameters);

    QJsonRpcMessage::Type type;
    QScopedPointer<QJsonObject> object;

    static int uniqueRequestCounter;
};

int QJsonRpcMessagePrivate::uniqueRequestCounter = 0;

QJsonRpcMessagePrivate::QJsonRpcMessagePrivate()
    : type(QJsonRpcMessage::Invalid),
      object(0)
{
}

QJsonRpcMessagePrivate::QJsonRpcMessagePrivate(const QJsonRpcMessagePrivate &other)
    : QSharedData(other),
      type(other.type),
      object(other.object ? new QJsonObject(*other.object) : 0)
{
}

void QJsonRpcMessagePrivate::initializeWithObject(const QJsonObject &message)
{
    object.reset(new QJsonObject(message));
    if (message.contains(QLatin1String("id"))) {
        if (message.contains(QLatin1String("result")) ||
            message.contains(QLatin1String("error"))) {
            if (message.contains(QLatin1String("error")) &&
                !message.value(QLatin1String("error")).isNull())
                type = QJsonRpcMessage::Error;
            else
                type = QJsonRpcMessage::Response;
        } else if (message.contains(QLatin1String("method"))) {
            type = QJsonRpcMessage::Request;
        }
    } else {
        if (message.contains(QLatin1String("method")))
            type = QJsonRpcMessage::Notification;
    }
}

QJsonRpcMessagePrivate::~QJsonRpcMessagePrivate()
{
}

QJsonRpcMessage::QJsonRpcMessage()
    : d(new QJsonRpcMessagePrivate)
{
    d->object.reset(new QJsonObject);
}

QJsonRpcMessage::QJsonRpcMessage(const QJsonRpcMessage &other)
    : d(other.d)
{
}

QJsonRpcMessage::~QJsonRpcMessage()
{
}

QJsonRpcMessage &QJsonRpcMessage::operator=(const QJsonRpcMessage &other)
{
    d = other.d;
    return *this;
}

bool QJsonRpcMessage::operator==(const QJsonRpcMessage &message) const
{
    if (message.d == d)
        return true;

    if (message.type() == type()) {
        if (message.type() == QJsonRpcMessage::Error) {
            return (message.errorCode() == errorCode() &&
                    message.errorMessage() == errorMessage() &&
                    message.errorData() == errorData());
        } else {
            if (message.type() == QJsonRpcMessage::Notification) {
                return (message.method() == method() &&
                        message.params() == params());
            } else {
                return (message.id() == id() &&
                        message.method() == method() &&
                        message.params() == params());
            }
        }
    }

    return false;
}

QJsonRpcMessage QJsonRpcMessage::fromJson(const QByteArray &message)
{
    QJsonRpcMessage result;
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(message, &error);
    if (error.error != QJsonParseError::NoError) {
        qJsonRpcDebug() << Q_FUNC_INFO << error.errorString();
        return result;
    }

    if (!document.isObject()) {
        qJsonRpcDebug() << Q_FUNC_INFO << "invalid message: " << message;
        return result;
    }

    result.d->initializeWithObject(document.object());
    return result;
}

QJsonRpcMessage QJsonRpcMessage::fromObject(const QJsonObject &message)
{
    QJsonRpcMessage result;
    result.d->initializeWithObject(message);
    return result;
}

QJsonObject QJsonRpcMessage::toObject() const
{
    if (d->object)
        return QJsonObject(*d->object);
    return QJsonObject();
}

QByteArray QJsonRpcMessage::toJson() const
{
    if (d->object) {
        QJsonDocument doc(*d->object);
        return doc.toJson();
    }

    return QByteArray();
}

bool QJsonRpcMessage::isValid() const
{
    return d->type != QJsonRpcMessage::Invalid;
}

QJsonRpcMessage::Type QJsonRpcMessage::type() const
{
    return d->type;
}

QJsonRpcMessage QJsonRpcMessagePrivate::createBasicRequest(const QString &method, const QJsonArray &params)
{
    QJsonRpcMessage request;
    request.d->object->insert(QLatin1String("jsonrpc"), QLatin1String("2.0"));
    request.d->object->insert(QLatin1String("method"), method);
    //if (!params.isEmpty())
        request.d->object->insert(QLatin1String("params"), params);
    return request;
}

QJsonRpcMessage QJsonRpcMessagePrivate::createBasicRequest(const QString &method,
                                                           const QJsonObject &namedParameters)
{
    QJsonRpcMessage request;
    request.d->object->insert(QLatin1String("jsonrpc"), QLatin1String("2.0"));
    request.d->object->insert(QLatin1String("method"), method);
    //if (!namedParameters.isEmpty())
        request.d->object->insert(QLatin1String("params"), namedParameters);
    return request;
}

QJsonRpcMessage QJsonRpcMessage::createRequest(const QString &method, const QJsonArray &params)
{
    QJsonRpcMessage request = QJsonRpcMessagePrivate::createBasicRequest(method, params);
    request.d->type = QJsonRpcMessage::Request;
    QJsonRpcMessagePrivate::uniqueRequestCounter++;
    request.d->object->insert(QLatin1String("id"), QJsonRpcMessagePrivate::uniqueRequestCounter);
    return request;
}

QJsonRpcMessage QJsonRpcMessage::createRequest(const QString &method, const QJsonValue &param)
{
    QJsonArray params;
    params.append(param);
    return createRequest(method, params);
}

QJsonRpcMessage QJsonRpcMessage::createRequest(const QString &method,
                                               const QJsonObject &namedParameters)
{
    QJsonRpcMessage request =
        QJsonRpcMessagePrivate::createBasicRequest(method, namedParameters);
    request.d->type = QJsonRpcMessage::Request;
    QJsonRpcMessagePrivate::uniqueRequestCounter++;
    request.d->object->insert(QLatin1String("id"), QJsonRpcMessagePrivate::uniqueRequestCounter);
    return request;
}

QJsonRpcMessage QJsonRpcMessage::createNotification(const QString &method, const QJsonArray &params)
{
    QJsonRpcMessage notification = QJsonRpcMessagePrivate::createBasicRequest(method, params);
    notification.d->type = QJsonRpcMessage::Notification;
    return notification;
}

QJsonRpcMessage QJsonRpcMessage::createNotification(const QString &method, const QJsonValue &param)
{
    QJsonArray params;
    params.append(param);
    return createNotification(method, params);
}

QJsonRpcMessage QJsonRpcMessage::createNotification(const QString &method,
                                                    const QJsonObject &namedParameters)
{
    QJsonRpcMessage notification =
        QJsonRpcMessagePrivate::createBasicRequest(method, namedParameters);
    notification.d->type = QJsonRpcMessage::Notification;
    return notification;
}

QJsonRpcMessage QJsonRpcMessage::createResponse(const QJsonValue &result) const
{
    QJsonRpcMessage response;
    if (d->object->contains(QLatin1String("id"))) {
        QJsonObject *object = response.d->object.data();
        object->insert(QLatin1String("jsonrpc"), QLatin1String("2.0"));
        object->insert(QLatin1String("id"), d->object->value(QLatin1String("id")));
        object->insert(QLatin1String("result"), result);
        response.d->type = QJsonRpcMessage::Response;
    }

    return response;
}

QJsonRpcMessage QJsonRpcMessage::createErrorResponse(QJsonRpc::ErrorCode code,
                                                     const QString &message,
                                                     const QJsonValue &data) const
{
    QJsonRpcMessage response;
    QJsonObject error;
    error.insert(QLatin1String("code"), code);
    if (!message.isEmpty())
        error.insert(QLatin1String("message"), message);
    if (!data.isUndefined())
        error.insert(QLatin1String("data"), data);

    response.d->type = QJsonRpcMessage::Error;
    QJsonObject *object = response.d->object.data();
    object->insert(QLatin1String("jsonrpc"), QLatin1String("2.0"));
    if (d->object->contains(QLatin1String("id")))
        object->insert(QLatin1String("id"), d->object->value(QLatin1String("id")));
    else
        object->insert(QLatin1String("id"), 0);
    object->insert(QLatin1String("error"), error);
    return response;
}

int QJsonRpcMessage::id() const
{
    if (d->type == QJsonRpcMessage::Notification || !d->object)
        return -1;

    const QJsonValue &value = d->object->value(QLatin1String("id"));
    if (value.isString())
        return value.toString().toInt();
#if QT_VERSION >= 0x050200
    return value.toInt();
#else
    return value.toDouble();
#endif
}

QString QJsonRpcMessage::method() const
{
    if (d->type == QJsonRpcMessage::Response || !d->object)
        return QString();

    return d->object->value(QLatin1String("method")).toString();
}

QJsonValue QJsonRpcMessage::params() const
{
    if (d->type == QJsonRpcMessage::Response || d->type == QJsonRpcMessage::Error)
        return QJsonValue(QJsonValue::Undefined);
    if (!d->object)
        return QJsonValue(QJsonValue::Undefined);

    return d->object->value(QLatin1String("params"));
}

QJsonValue QJsonRpcMessage::result() const
{
    if (d->type != QJsonRpcMessage::Response || !d->object)
        return QJsonValue(QJsonValue::Undefined);

    return d->object->value(QLatin1String("result"));
}

int QJsonRpcMessage::errorCode() const
{
    if (d->type != QJsonRpcMessage::Error || !d->object)
        return 0;

    QJsonObject error =
        d->object->value(QLatin1String("error")).toObject();
    const QJsonValue &value = error.value(QLatin1String("code"));
    if (value.isString())
        return value.toString().toInt();
#if QT_VERSION >= 0x050200
    return value.toInt();
#else
    return value.toDouble();
#endif
}

QString QJsonRpcMessage::errorMessage() const
{
    if (d->type != QJsonRpcMessage::Error || !d->object)
        return QString();

    QJsonObject error =
        d->object->value(QLatin1String("error")).toObject();
    return error.value(QLatin1String("message")).toString();
}

QJsonValue QJsonRpcMessage::errorData() const
{
    if (d->type != QJsonRpcMessage::Error || !d->object)
        return QJsonValue(QJsonValue::Undefined);

    QJsonObject error =
        d->object->value(QLatin1String("error")).toObject();
    return error.value(QLatin1String("data"));
}

#if QT_VERSION < 0x050000
bool QJsonRpcMessage::isDetached() const
{
    return d && d->ref == 1;
}
#endif

static QDebug operator<<(QDebug dbg, QJsonRpcMessage::Type type)
{
    switch (type) {
    case QJsonRpcMessage::Request:
        return dbg << "QJsonRpcMessage::Request";
    case QJsonRpcMessage::Response:
        return dbg << "QJsonRpcMessage::Response";
    case QJsonRpcMessage::Notification:
        return dbg << "QJsonRpcMessage::Notification";
    case QJsonRpcMessage::Error:
        return dbg << "QJsonRpcMessage::Error";
    default:
        return dbg << "QJsonRpcMessage::Invalid";
    }
}

QDebug operator<<(QDebug dbg, const QJsonRpcMessage &msg)
{
    dbg.nospace() << "QJsonRpcMessage(type=" << msg.type();
    if (msg.type() != QJsonRpcMessage::Notification) {
        dbg.nospace() << ", id=" << msg.id();
    }

    if (msg.type() == QJsonRpcMessage::Request ||
        msg.type() == QJsonRpcMessage::Notification) {
        dbg.nospace() << ", method=" << msg.method()
                      << ", params=" << msg.params();
    } else if (msg.type() == QJsonRpcMessage::Response) {
        dbg.nospace() << ", result=" << msg.result();
    } else if (msg.type() == QJsonRpcMessage::Error) {
        dbg.nospace() << ", code=" << msg.errorCode()
                      << ", message=" << msg.errorMessage()
                      << ", data=" << msg.errorData();
    }
    dbg.nospace() << ")";
    return dbg.space();
}
