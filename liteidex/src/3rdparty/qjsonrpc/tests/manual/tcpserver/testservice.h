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
#ifndef TESTSERVICE_H
#define TESTSERVICE_H

#include "qjsonrpcservice.h"

class TestService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", "agent")
public:
    TestService(QObject *parent = 0);

public Q_SLOTS:
    void testMethod();
    void testMethodWithParams(const QString &first, bool second, double third);
    void testMethodWithVariantParams(const QString &first, bool second, double third, const QVariant &fourth);
    QString testMethodWithParamsAndReturnValue(const QString &name);
    void testMethodWithDefaultParameter(const QString &first, const QString &second = QString());
    void testNotifyConnectedClients(const QString &message);

};


#endif
