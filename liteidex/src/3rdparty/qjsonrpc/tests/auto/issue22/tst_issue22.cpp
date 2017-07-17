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
#include <QTcpSocket>
#include <QScopedPointer>

#include <QtCore/QEventLoop>
#include <QtCore/QVariant>
#include <QtTest/QtTest>

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#else
#include "json/qjsondocument.h"
#endif

#include "qjsonrpcabstractserver.h"
#include "qjsonrpctcpserver.h"
#include "qjsonrpcsocket.h"
#include "qjsonrpcmessage.h"
#include "signalspy.h"

class TestIssue22: public QObject
{
    Q_OBJECT
public:
    TestIssue22();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testIssue21();

private:
    QThread serverThread;
    QScopedPointer<QJsonRpcTcpServer> tcpServer;
    quint16 tcpServerPort;
};

TestIssue22::TestIssue22()
{
    tcpServerPort = quint16(8118 + qrand() % 1000);
}

void TestIssue22::initTestCase()
{
    serverThread.start();
}

void TestIssue22::cleanupTestCase()
{
    serverThread.quit();
    QVERIFY(serverThread.wait());
}

void TestIssue22::init()
{
    tcpServer.reset(new QJsonRpcTcpServer);
    QVERIFY(tcpServer->listen(QHostAddress::LocalHost, tcpServerPort));
    tcpServer->moveToThread(&serverThread);
}

void TestIssue22::cleanup()
{
    tcpServer->close();
}

class Issue21Service : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", "service")
public:
    Issue21Service(QObject *parent = 0) : QJsonRpcService(parent) {}

public Q_SLOTS:
    QString fastMethod(const QString &name)
    {
        QEventLoop loop;
        QTimer::singleShot(150, &loop, SLOT(quit()));
        loop.exec();

        return QString("fast %1").arg(name);
    }

    QString slowMethod(const QString &name)
    {
        QEventLoop loop;
        QTimer::singleShot(2000, &loop, SLOT(quit()));
        loop.exec();

        return QString("slow %1").arg(name);
    }
};

class TestClientRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    TestClientRunnable(bool slow, quint16 port)
        : m_slow(slow), m_port(port)
    {
        setAutoDelete(true);
    }

    virtual void run() {
        QTcpSocket socket;
        socket.connectToHost(QHostAddress::LocalHost, m_port);
        QVERIFY(socket.waitForConnected());

        // run tests
        QJsonRpcSocket client(&socket);
        QJsonRpcMessage request = m_slow ?
            QJsonRpcMessage::createRequest("service.slowMethod", QLatin1String("slow")) :
            QJsonRpcMessage::createRequest("service.fastMethod", QLatin1String("fast"));
        QJsonRpcMessage response = client.sendMessageBlocking(request);
        Q_EMIT messageReceived(request, response);
        socket.disconnectFromHost();
    }

Q_SIGNALS:
    void messageReceived(const QJsonRpcMessage &request, const QJsonRpcMessage &response);

private:
    bool m_slow;
    quint16 m_port;
    QJsonRpcMessage m_message;

};

void TestIssue22::testIssue21()
{
    QVERIFY(tcpServer->addService(new Issue21Service));

    TestClientRunnable *fastClient = new TestClientRunnable(false, tcpServerPort);
    SignalSpy fastClientSpy(fastClient, SIGNAL(messageReceived(QJsonRpcMessage, QJsonRpcMessage)));
    TestClientRunnable *slowClient = new TestClientRunnable(true, tcpServerPort);
    SignalSpy slowClientSpy(slowClient, SIGNAL(messageReceived(QJsonRpcMessage, QJsonRpcMessage)));

    QThreadPool::globalInstance()->setMaxThreadCount(10);
    QThreadPool::globalInstance()->start(fastClient);
    QThreadPool::globalInstance()->start(slowClient);

    fastClientSpy.wait();
    slowClientSpy.wait();
    QCOMPARE(fastClientSpy.length(), slowClientSpy.length());

    QList<QVariant> fastClientPair = fastClientSpy.first();
    QJsonRpcMessage fastRequest = fastClientPair.at(0).value<QJsonRpcMessage>();
    QJsonRpcMessage fastResponse = fastClientPair.at(1).value<QJsonRpcMessage>();
    QCOMPARE(fastRequest.id(), fastResponse.id());

    QList<QVariant> slowClientPair = slowClientSpy.first();
    QJsonRpcMessage slowRequest = slowClientPair.at(0).value<QJsonRpcMessage>();
    QJsonRpcMessage slowResponse = slowClientPair.at(1).value<QJsonRpcMessage>();
    QCOMPARE(slowRequest.id(), slowResponse.id());
}

QTEST_MAIN(TestIssue22)
#include "tst_issue22.moc"
