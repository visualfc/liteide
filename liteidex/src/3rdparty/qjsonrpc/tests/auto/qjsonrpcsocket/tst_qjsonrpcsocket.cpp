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
#include <QLocalServer>
#include <QLocalSocket>

#include <QtCore/QEventLoop>
#include <QtCore/QVariant>
#include <QtTest/QtTest>

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#else
#include "json/qjsondocument.h"
#endif

#include "qjsonrpcservice_p.h"
#include "qjsonrpcservice.h"
#include "qjsonrpcmessage.h"
#include "qjsonrpcservicereply.h"
#include "qjsonrpcsocket_p.h"
#include "qjsonrpcsocket.h"

class QBufferBackedQJsonRpcSocketPrivate : public QJsonRpcSocketPrivate
{
public:
    QBufferBackedQJsonRpcSocketPrivate(QBuffer *b, QJsonRpcSocket *q)
        : QJsonRpcSocketPrivate(q),
          buffer(b)
    {
        device = b;
    }

    virtual void _q_processIncomingData() {
        buffer->seek(0);
        QJsonRpcSocketPrivate::_q_processIncomingData();
    }

    QBuffer *buffer;

};

class QBufferBackedQJsonRpcSocket : public QJsonRpcSocket
{
    Q_OBJECT
public:
    QBufferBackedQJsonRpcSocket(QBuffer *buffer, QObject *parent = 0)
        : QJsonRpcSocket(*new QBufferBackedQJsonRpcSocketPrivate(buffer, this), parent)
    {
    }
};

class TestQJsonRpcSocket: public QObject
{
    Q_OBJECT
public:
    enum InvokeType {
        SendMessage,
        InvokeRemoteMethod
    };

private Q_SLOTS:
    void initTestCase_data();
    void noParameters();
    void multiParameter();
    void notification();
    void response();
    void delayedMessageReceive();

private:
    // benchmark parsing speed
    void jsonParsingBenchmark();
};
Q_DECLARE_METATYPE(TestQJsonRpcSocket::InvokeType)

void TestQJsonRpcSocket::initTestCase_data()
{
    QTest::addColumn<InvokeType>("invokeType");
    QTest::newRow("sendMessage") << SendMessage;
    QTest::newRow("invokeRemoteMethod") << InvokeRemoteMethod;
}

void TestQJsonRpcSocket::noParameters()
{
    QFETCH_GLOBAL(InvokeType, invokeType);

    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QJsonRpcSocket serviceSocket(&buffer, this);
    QSignalSpy spyMessageReceived(&serviceSocket,
                                  SIGNAL(messageReceived(QJsonRpcMessage)));
    QVERIFY(serviceSocket.isValid());

    QJsonRpcMessage request;
    QScopedPointer<QJsonRpcServiceReply> reply;
    if (invokeType == SendMessage) {
        request = QJsonRpcMessage::createRequest(QString("test.noParam"));
        reply.reset(serviceSocket.sendMessage(request));
    } else if (invokeType == InvokeRemoteMethod) {
        reply.reset(serviceSocket.invokeRemoteMethod("test.noParam"));
        request = reply->request();
    }

    QJsonRpcMessage bufferMessage = QJsonRpcMessage::fromJson(buffer.data());
    QCOMPARE(request.id(), bufferMessage.id());
    QCOMPARE(request.type(), bufferMessage.type());
    QCOMPARE(request.method(), bufferMessage.method());
    QCOMPARE(request.params(), bufferMessage.params());
    QCOMPARE(spyMessageReceived.count(), 0);
}

void TestQJsonRpcSocket::multiParameter()
{
    QFETCH_GLOBAL(InvokeType, invokeType);

    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QJsonRpcSocket serviceSocket(&buffer, this);
    QSignalSpy spyMessageReceived(&serviceSocket,
                                  SIGNAL(messageReceived(QJsonRpcMessage)));
    QVERIFY(serviceSocket.isValid());

    QJsonArray params;
    params.append(false);
    params.append(true);
    QJsonRpcMessage request;
    QScopedPointer<QJsonRpcServiceReply> reply;
    if (invokeType == SendMessage) {
        request = QJsonRpcMessage::createRequest("test.multiParam", params);
        reply.reset(serviceSocket.sendMessage(request));
    } else if (invokeType == InvokeRemoteMethod) {
#if QT_VERSION <= 0x050000
        QVariant paramsVariant = QVariant::fromValue(params);
        reply.reset(serviceSocket.invokeRemoteMethod("test.multiParam", paramsVariant));
#else
        reply.reset(serviceSocket.invokeRemoteMethod("test.multiParam", params));
#endif
        request = reply->request();
    }

    QJsonRpcMessage bufferMessage = QJsonRpcMessage::fromJson(buffer.data());
    QCOMPARE(request.id(), bufferMessage.id());
    QCOMPARE(request.type(), bufferMessage.type());
    QCOMPARE(request.method(), bufferMessage.method());
    QCOMPARE(request.params(), bufferMessage.params());
    QCOMPARE(spyMessageReceived.count(), 0);
}

void TestQJsonRpcSocket::notification()
{
    QFETCH_GLOBAL(InvokeType, invokeType);

    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QJsonRpcSocket serviceSocket(&buffer, this);
    QSignalSpy spyMessageReceived(&serviceSocket,
                                  SIGNAL(messageReceived(QJsonRpcMessage)));
    QVERIFY(serviceSocket.isValid());

    QJsonRpcMessage notification;
    QScopedPointer<QJsonRpcServiceReply> reply;
    if (invokeType == SendMessage) {
        notification = QJsonRpcMessage::createRequest("test.notify");
        reply.reset(serviceSocket.sendMessage(notification));
    } else if (invokeType == InvokeRemoteMethod) {
        reply.reset(serviceSocket.invokeRemoteMethod("test.notify"));
        notification = reply->request();
    }

    QJsonRpcMessage bufferMessage = QJsonRpcMessage::fromJson(buffer.data());
    QCOMPARE(notification.id(), bufferMessage.id());
    QCOMPARE(notification.type(), bufferMessage.type());
    QCOMPARE(notification.method(), bufferMessage.method());
    QCOMPARE(notification.params(), bufferMessage.params());
    QCOMPARE(spyMessageReceived.count(), 0);
}

void TestQJsonRpcSocket::response()
{
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QJsonRpcSocket serviceSocket(&buffer, this);
    QSignalSpy spyMessageReceived(&serviceSocket,
                                  SIGNAL(messageReceived(QJsonRpcMessage)));
    QVERIFY(serviceSocket.isValid());

    QJsonRpcMessage response = QJsonRpcMessage::createRequest("test.response");
    response = response.createResponse(QJsonValue());

    QScopedPointer<QJsonRpcServiceReply> reply;
    reply.reset(serviceSocket.sendMessage(response));

    QJsonRpcMessage bufferMessage = QJsonRpcMessage::fromJson(buffer.data());
    QCOMPARE(response.id(), bufferMessage.id());
    QCOMPARE(response.type(), bufferMessage.type());
    QCOMPARE(response.method(), bufferMessage.method());
    QCOMPARE(response.params(), bufferMessage.params());
    QCOMPARE(spyMessageReceived.count(), 0);
}

void TestQJsonRpcSocket::jsonParsingBenchmark()
{
    QFile testData(":/testwire.json");
    QVERIFY(testData.open(QIODevice::ReadOnly));
    QByteArray jsonData = testData.readAll();
    QJsonRpcSocketPrivate socketPrivate(0);

    int messageCount = 0;
    while (!jsonData.isEmpty()) {
        int pos = 0;
        QBENCHMARK {
            pos = socketPrivate.findJsonDocumentEnd(jsonData);
        }

        if (pos > -1) {
            messageCount++;
            jsonData = jsonData.mid(pos + 1);
        } else {
            break;
        }
    }

    QCOMPARE(messageCount, 8);
}

void TestQJsonRpcSocket::delayedMessageReceive()
{
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QBufferBackedQJsonRpcSocket serviceSocket(&buffer, this);
    QSignalSpy spyMessageReceived(&serviceSocket,
                                  SIGNAL(messageReceived(QJsonRpcMessage)));
    QVERIFY(serviceSocket.isValid());

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("test.multiParam");

    QJsonRpcMessage response = serviceSocket.sendMessageBlocking(request, 1);
    QVERIFY(response.type() == QJsonRpcMessage::Error);
    spyMessageReceived.removeLast();

    // this should cause a crash, before the fix
    const char *fakeDelayedResult =
        "{" \
            "\"id\": %1," \
            "\"jsonrpc\": \"2.0\"," \
            "\"result\": true" \
        "}";

    buffer.write(QString(fakeDelayedResult).arg(request.id()).toLatin1());
    while (!spyMessageReceived.size())
        qApp->processEvents();
}

QTEST_MAIN(TestQJsonRpcSocket)
#include "tst_qjsonrpcsocket.moc"
