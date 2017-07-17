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
#include <QLocalSocket>
#include <QEventLoop>
#include <QTimer>
#include <QScriptContext>

#include "qjsonrpcsocket.h"
#include "qjsonrpcservice.h"
#include "qjsonrpcservicereply.h"
#include "interface.h"

QJsonRpcServiceSocketPrototype::QJsonRpcServiceSocketPrototype(QObject *parent)
    : QObject(parent),
      m_socket(0)
{    
}

QJsonRpcServiceSocketPrototype::~QJsonRpcServiceSocketPrototype()
{
}

void QJsonRpcServiceSocketPrototype::connectToLocalService(const QString &service)
{
    if (m_socket) {
        context()->throwError("already connected to local service");
        return;
    }

    QLocalSocket *localSocket = new QLocalSocket(this);
    localSocket->connectToServer(service);
    if (!localSocket->waitForConnected()) {
        context()->throwError("could not connect to local sevice: " + service);
        localSocket->deleteLater();
        return;
    }

    m_socket = new QJsonRpcSocket(localSocket, this);
}

QVariant QJsonRpcServiceSocketPrototype::invokeRemoteMethod(const QString &method,
                                                         const QVariant &param1, const QVariant &param2, const QVariant &param3,
                                                         const QVariant &param4, const QVariant &param5, const QVariant &param6,
                                                         const QVariant &param7, const QVariant &param8, const QVariant &param9,
                                                         const QVariant &param10)
{
    QVariantList params;
    if (param1.isValid()) params.append(param1);
    if (param2.isValid()) params.append(param2);
    if (param3.isValid()) params.append(param3);
    if (param4.isValid()) params.append(param4);
    if (param5.isValid()) params.append(param5);
    if (param6.isValid()) params.append(param6);
    if (param7.isValid()) params.append(param7);
    if (param8.isValid()) params.append(param8);
    if (param9.isValid()) params.append(param9);
    if (param10.isValid()) params.append(param10);

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest(method, QJsonArray::fromVariantList(params));
    QJsonRpcServiceReply *reply = m_socket->sendMessage(request);
    QEventLoop loop;
    connect(m_socket, SIGNAL(messageReceived(QJsonRpcMessage)), &loop, SLOT(quit()));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->response().type() == QJsonRpcMessage::Invalid) {
        context()->throwError("request timed out");
        return QVariant();
    }
    return reply->response().result().toVariant();
}
