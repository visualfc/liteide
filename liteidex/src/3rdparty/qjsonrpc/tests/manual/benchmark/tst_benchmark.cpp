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
#include <QScopedPointer>

#include <QtCore/QEventLoop>
#include <QtCore/QVariant>
#include <QtTest/QtTest>
#include <QElapsedTimer>
#include <QThread>

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#else
#include "json/qjsondocument.h"
#endif

#include "qjsonrpcabstractserver.h"
#include "qjsonrpcsocket.h"
#include "qjsonrpcservice.h"
#include "qjsonrpcmessage.h"

class TestBenchmark: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void simple();
    void namedParameters();

};

class TestService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", "service")
public:
    TestService(QObject *parent = 0) : QJsonRpcService(parent)
    {}

    QJsonRpcMessage testDispatch(const QJsonRpcMessage &message) {
        return QJsonRpcService::dispatch(message);
    }

public Q_SLOTS:
    QString singleParam(int i) const { return QString::number(i); }
    QString singleParam(const QString &string) const { return string; }
    QString singleParam(const QVariant &k) const { return k.toString(); }

    QString namedParams(int integer, const QString &string, double doub)
    {
        (void) integer;
        (void) doub;

        return string;
    }
};

class TestServiceProvider : public QJsonRpcServiceProvider
{
public:
    TestServiceProvider() {}
};

void TestBenchmark::simple()
{
    TestServiceProvider provider;
    TestService service;
    provider.addService(&service);

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.singleParam", QString("test"));
    QBENCHMARK {
        QJsonRpcMessage response = service.testDispatch(request);
        QVERIFY(response.type() != QJsonRpcMessage::Error);
    }
}

void TestBenchmark::namedParameters()
{
    TestServiceProvider provider;
    TestService service;
    provider.addService(&service);

    QJsonObject obj;
    obj["integer"] = 1;
    obj["string"] = QLatin1String("str");
    obj["doub"] = 1.2;
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.namedParams", obj);

    QBENCHMARK {
        QJsonRpcMessage response = service.testDispatch(request);
        QVERIFY(response.type() != QJsonRpcMessage::Error);
    }
}

QTEST_MAIN(TestBenchmark)
#include "tst_benchmark.moc"

