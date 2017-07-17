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
#include <QCoreApplication>
#include <QStringList>
#include <QLocalSocket>
#include <QTcpSocket>
#include <QScopedPointer>
#include <QUrl>
#include <QHostAddress>

#include "qjsonrpcsocket.h"
#include "qjsonrpcservice.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    QString appName = args.takeFirst();
    bool notification = args.contains("-n");
    if (notification)
        args.removeAll("-n");

    if (args.size() < 2) {
        qDebug("usage: %s [-n] <service> <method> <arguments>", appName.toLocal8Bit().data());
        return -1;
    }

    // try to process socket
    QIODevice *device = 0;
    QScopedPointer<QIODevice> devicePtr(device);
    QString service = args.takeFirst();
    QUrl serviceUrl = QUrl::fromUserInput(service);
    QHostAddress serviceAddress(serviceUrl.host());
    if (serviceAddress.isNull()) {
        QLocalSocket *localSocket = new QLocalSocket;
        device = localSocket;
        localSocket->connectToServer(service);
        if (!localSocket->waitForConnected(5000)) {
            qDebug("could not connect to service: %s", service.toLocal8Bit().data());
            return -1;
        }
    } else {
        QTcpSocket *tcpSocket = new QTcpSocket;
        device = tcpSocket;
        int servicePort = serviceUrl.port() ? serviceUrl.port() : 5555;
        tcpSocket->connectToHost(serviceAddress, servicePort);
        if (!tcpSocket->waitForConnected(5000)) {
            qDebug("could not connect to host at %s:%d", serviceUrl.host().toLocal8Bit().data(),
                   servicePort);
            return -1;
        }
    }

    QJsonRpcSocket socket(device);
    QString method = args.takeFirst();
    QVariantList arguments;
    foreach (QString arg, args)
        arguments.append(arg);

    QJsonRpcMessage request = notification ?
        QJsonRpcMessage::createNotification(method, QJsonArray::fromVariantList(arguments)) :
        QJsonRpcMessage::createRequest(method, QJsonArray::fromVariantList(arguments));

    qDebug() << request.toJson();

    QJsonRpcMessage response = socket.sendMessageBlocking(request, 5000);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());
        return -1;
    }

    qDebug() << response.result();
}
