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
#include <QtTest/QtTest>

#include "testhttpserver.h"
#include "qjsonrpcmessage.h"
#include "qjsonrpchttpclient.h"

#if QT_VERSION < 0x050000
template <typename T>
struct QScopedPointerObjectDeleteLater
{
    static inline void cleanup(T *pointer) { if (pointer) pointer->deleteLater(); }
};

class QObject;
typedef QScopedPointerObjectDeleteLater<QObject> QScopedPointerDeleteLater;
#endif

class TestQJsonRpcHttpClient : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init();

    void properties();
    void basicRequest();
    void invalidResponse_data();
    void invalidResponse();
    void connectionRefused();
    void requestTimedOut();
    void issue23_doubleFinishedEmitted();
};

void TestQJsonRpcHttpClient::init()
{
}

class JsonRpcRequestHandler : public TestHttpServerRequestHandler
{
public:
    virtual QByteArray handleRequest(QNetworkAccessManager::Operation operation,
                                     const QNetworkRequest &request, const QByteArray &body)
    {
        Q_UNUSED(operation)
        Q_UNUSED(request)
        QJsonRpcMessage requestMessage = QJsonRpcMessage::fromJson(body);
        QJsonRpcMessage responseMessage =
            requestMessage.createResponse(QLatin1String("some response data"));
        QByteArray responseData = responseMessage.toJson();

        QByteArray reply;
        reply += "HTTP/1.0 200\r\n";
        reply += "Content-Type: application/json\r\n";
        reply += "Content-length: " + QByteArray::number(responseData.size()) + "\r\n";
        reply += "\r\n";
        reply += responseData;
        return reply;
    }
};

void TestQJsonRpcHttpClient::properties()
{
    QJsonRpcHttpClient client;
    client.setEndPoint("testing");
    QCOMPARE(client.endPoint(), QUrl("http://testing"));
    client.setEndPoint(QUrl("http://www.google.com"));
    QCOMPARE(client.endPoint(), QUrl("http://www.google.com"));

    QNetworkAccessManager manager;
    QJsonRpcHttpClient withManager(&manager);
    QCOMPARE(withManager.networkAccessManager(), &manager);
}

void TestQJsonRpcHttpClient::basicRequest()
{
    TestHttpServer server;
    server.setRequestHandler(new JsonRpcRequestHandler);
    QVERIFY(server.listen());

    QString url =
        QString("%1://localhost:%2").arg("http").arg(server.serverPort());
    QJsonRpcHttpClient client(url);
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("testMethod");
    QJsonRpcMessage response = client.sendMessageBlocking(message);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE(response.result().toString(), QLatin1String("some response data"));
}

void TestQJsonRpcHttpClient::invalidResponse_data()
{
    QTest::addColumn<QByteArray>("responseData");
    QTest::addColumn<QJsonRpc::ErrorCode>("expectedError");

    QTest::newRow("empty-data") <<
        QByteArray("HTTP/1.0 200\r\nContent-Type: application/json\r\nContent-length: 0\r\n\r\n") << QJsonRpc::ParseError;
    QTest::newRow("invalid-json") <<
        QByteArray("HTTP/1.0 200\r\nContent-Type: application/json\r\nContent-length: 2\r\n\r\n{}") << QJsonRpc::InternalError;
}

void TestQJsonRpcHttpClient::invalidResponse()
{
    QFETCH(QByteArray, responseData);
    QFETCH(QJsonRpc::ErrorCode, expectedError);

    TestHttpServer server;
    server.setResponseData(responseData);
    QVERIFY(server.listen());

    QString url =
        QString("%1://localhost:%2").arg("http").arg(server.serverPort());
    QJsonRpcHttpClient client(url);
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("someMethod");
    QJsonRpcMessage response = client.sendMessageBlocking(message);
    QCOMPARE(response.type(), QJsonRpcMessage::Error);
    QCOMPARE(response.errorCode(), int(expectedError));
}

void TestQJsonRpcHttpClient::connectionRefused()
{
    QString url = QString("%1://localhost:%2").arg("http").arg(9191);
    QJsonRpcHttpClient client(url);
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("someMethod");
    QJsonRpcMessage response = client.sendMessageBlocking(message);
    QCOMPARE(response.type(), QJsonRpcMessage::Error);
    QCOMPARE(response.errorCode(), int(QJsonRpc::InternalError));
}

void TestQJsonRpcHttpClient::requestTimedOut()
{
    TestHttpServer server;
    server.setResponseData("HTTP/1.0 200\r\n\r\n");
    QVERIFY(server.listen());

    QString url =
        QString("%1://localhost:%2").arg("http").arg(server.serverPort());
    QJsonRpcHttpClient client(url);
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("someMethod");
    QJsonRpcMessage response = client.sendMessageBlocking(message, 1);
    QCOMPARE(response.type(), QJsonRpcMessage::Error);
    QCOMPARE(response.errorCode(), int(QJsonRpc::TimeoutError));
}

void TestQJsonRpcHttpClient::issue23_doubleFinishedEmitted()
{
    QString url = QString("%1://localhost:%2").arg("http").arg(9191);
    QJsonRpcHttpClient client(url);
    QJsonRpcMessage message = QJsonRpcMessage::createRequest("someMethod");
    QJsonRpcServiceReply *reply = client.sendMessage(message);
    QScopedPointer<QJsonRpcServiceReply> replyPtr(reply);
    QSignalSpy spy(reply, SIGNAL(finished()));

    QEventLoop responseLoop;
    connect(reply, SIGNAL(finished()), &responseLoop, SLOT(quit()));
    QTimer::singleShot(10000, &responseLoop, SLOT(quit()));
    responseLoop.exec();

    QJsonRpcMessage response = reply->response();
    QCOMPARE(response.type(), QJsonRpcMessage::Error);
    QCOMPARE(response.errorCode(), int(QJsonRpc::InternalError));
    QCOMPARE(spy.size(), 1);
}

QTEST_MAIN(TestQJsonRpcHttpClient)
#include "tst_qjsonrpchttpclient.moc"
