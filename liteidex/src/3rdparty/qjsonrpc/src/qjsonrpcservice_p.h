/*
 * Copyright (C) 2012-2013 Matt Broadstone
 * Copyright (C) 2013 Fargier Sylvain
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
#ifndef QJSONRPCSERVICE_P_H
#define QJSONRPCSERVICE_P_H

#include <QHash>
#include <QPointer>
#include <QVarLengthArray>
#include <QStringList>

#include "qjsonrpcservice.h"

class QJsonRpcAbstractSocket;
class QJsonRpcServiceRequestPrivate : public QSharedData
{
public:
    QJsonRpcMessage request;
    QPointer<QJsonRpcAbstractSocket> socket;
};

class QJsonRpcService;
#if defined(USE_QT_PRIVATE_HEADERS)
#include <private/qobject_p.h>

class QJsonRpcServicePrivate : public QObjectPrivate
#else
class QJsonRpcServicePrivate
#endif
{
public:
    QJsonRpcServicePrivate(QJsonRpcService *parent)
        : delayedResponse(false),
          q_ptr(parent)
    {
    }

    void cacheInvokableInfo();
    static int qjsonRpcMessageType;
    static int convertVariantTypeToJSType(int type);
    static QJsonValue convertReturnValue(QVariant &returnValue);

    struct ParameterInfo
    {
        ParameterInfo(const QString &name = QString(), int type = 0, bool out = false);

        int type;
        int jsType;
        QString name;
        bool out;
    };

    struct MethodInfo
    {
        MethodInfo();
        MethodInfo(const QMetaMethod &method);

        QVarLengthArray<ParameterInfo> parameters;
        int returnType;
        bool valid;
        bool hasOut;
    };

    QHash<int, MethodInfo > methodInfoHash;
    QHash<QByteArray, QList<int> > invokableMethodHash;
    QJsonRpcServiceRequest currentRequest;
    bool delayedResponse;

    QJsonRpcService * const q_ptr;
    Q_DECLARE_PUBLIC(QJsonRpcService)
};

#endif
