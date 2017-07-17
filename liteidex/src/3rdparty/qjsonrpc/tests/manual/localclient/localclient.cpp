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
#include <QDir>

#include "qjsonrpcsocket.h"
#include "qjsonrpcservice.h"
#include "qjsonrpcservicereply.h"
#include "localclient.h"

LocalClient::LocalClient(QObject *parent)
    : QObject(parent),
      m_client(0)
{
}

void LocalClient::run()
{
    QLocalSocket *socket = new QLocalSocket(this);
    QString serviceName = QDir::temp().absoluteFilePath("testservice");
    socket->connectToServer(serviceName);
    if (!socket->waitForConnected()) {
        qDebug() << "could not connect to server: " << socket->errorString();
        return;
    }

    m_client = new QJsonRpcSocket(socket, this);
    QJsonRpcServiceReply *reply = m_client->invokeRemoteMethod("agent.testMethod");
    connect(reply, SIGNAL(finished()), this, SLOT(processResponse()));

    reply = m_client->invokeRemoteMethod("agent.testMethodWithParams", "one", false, 10);
    connect(reply, SIGNAL(finished()), this, SLOT(processResponse()));

    reply = m_client->invokeRemoteMethod("agent.testMethodWithVariantParams", "one", false, 10, QVariant(2.5));
    connect(reply, SIGNAL(finished()), this, SLOT(processResponse()));

    reply = m_client->invokeRemoteMethod("agent.testMethodWithParamsAndReturnValue", "matt");
    connect(reply, SIGNAL(finished()), this, SLOT(processResponse()));
}

void LocalClient::processResponse()
{
    QJsonRpcServiceReply *reply = static_cast<QJsonRpcServiceReply *>(sender());
    if (!reply) {
        qDebug() << "invalid response received";
        return;
    }

    qDebug() << "response received: " << reply->response();
}
