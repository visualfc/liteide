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
#include <QLocalServer>
#include <QFile>
#include <QDir>

#include "qjsonrpclocalserver.h"
#include "testservice.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QString serviceName = QDir::temp().absoluteFilePath("testservice");
    if (QFile::exists(serviceName)) {
        if (!QFile::remove(serviceName)) {
            qDebug() << "couldn't delete temporary service";
            return -1;
        }
    }

    QJsonRpcLocalServer rpcServer;
    rpcServer.addService(new TestService);
    if (!rpcServer.listen(serviceName)) {
        qDebug() << "could not start server: " << rpcServer.errorString();
        return -1;
    }

    return app.exec();
}
