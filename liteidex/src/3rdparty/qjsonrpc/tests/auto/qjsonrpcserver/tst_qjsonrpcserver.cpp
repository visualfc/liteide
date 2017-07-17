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
#include "qjsonrpclocalserver.h"
#include "qjsonrpctcpserver.h"
#include "qjsonrpchttpserver.h"
#include "qjsonrpchttpclient.h"
#include "qjsonrpcsocket.h"
#include "qjsonrpcmessage.h"
#include "qjsonrpcservicereply.h"
#include "testservices.h"

class TestQJsonRpcServer: public QObject
{
    Q_OBJECT
public:
    TestQJsonRpcServer();

    enum ServerType {
        TcpServer,
        LocalServer,
        HttpServer
    };

private Q_SLOTS:
    void initTestCase_data();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void noParameter();
    void singleParameter();
    void multiParameter();
    void variantParameter();
    void variantListParameter();
    void variantResult();
    void invalidArgs();
    void methodNotFound();
    void invalidRequest();
    void notifyConnectedClients_data();
    void notifyConnectedClients();
    void numberParameters();
    void hugeResponse();
    void complexMethod();
    void defaultParameters();
    void overloadedMethod();
    void qVariantMapInvalidParam();
    void stringListParameter();
    void outputParameter();
#if QT_VERSION >= 0x050200
    void jsonReturnTypes();
#endif
    void notifyServiceSocket();
    void userDeletesReplyOnDelayedResponse();
    void delayedResponseBasic();
    void delayedResponseSocketClosed();

    void addRemoveService();
    void serviceWithNoGivenName();
    void cantRemoveInvalidService();
    void cantAddServiceTwice();

private:
    QJsonRpcAbstractSocket *createClient();

    // client related
    QScopedPointer<QJsonRpcAbstractSocket> clientSocket;
    QList<QTcpSocket*> tcpSockets;
    QList<QLocalSocket*> localSockets;

    // server related
    QJsonRpcAbstractServer *server;
    QThread serverThread;
    QScopedPointer<QJsonRpcTcpServer> tcpServer;
    QScopedPointer<QJsonRpcLocalServer> localServer;
    QScopedPointer<QJsonRpcHttpServer> httpServer;

    quint16 tcpServerPort;
    quint16 httpServerPort;

private:
    // temporarily disabled
    // void testListOfInts();
};
Q_DECLARE_METATYPE(TestQJsonRpcServer::ServerType)
Q_DECLARE_METATYPE(QJsonRpcMessage::Type)

TestQJsonRpcServer::TestQJsonRpcServer()
    : server(0)
{
}

void TestQJsonRpcServer::initTestCase_data()
{
    QTest::addColumn<ServerType>("serverType");
    QTest::newRow("tcp") << TcpServer;
    QTest::newRow("local") << LocalServer;
    QTest::newRow("http") << HttpServer;
}

void TestQJsonRpcServer::initTestCase()
{
    serverThread.start();
}

void TestQJsonRpcServer::cleanupTestCase()
{
    serverThread.quit();
    QVERIFY(serverThread.wait());
}

QJsonRpcAbstractSocket *TestQJsonRpcServer::createClient()
{
    QFETCH_GLOBAL(ServerType, serverType);

    QJsonRpcAbstractSocket *socket = 0;
    if (serverType == LocalServer) {
        QLocalSocket *localSocket = new QLocalSocket;
        connect(localServer.data(), SIGNAL(clientConnected()),
                &QTestEventLoop::instance(), SLOT(exitLoop()));
        localSocket->connectToServer("qjsonrpc-test-local-server");
        QTestEventLoop::instance().enterLoop(5);
        if (QTestEventLoop::instance().timeout() || !localSocket->waitForConnected()) {
            delete localSocket;
            return 0;
        }

        socket = new QJsonRpcSocket(localSocket);
        localSockets.append(localSocket);
    } else if (serverType == TcpServer) {
        QTcpSocket *tcpSocket = new QTcpSocket;
        connect(tcpServer.data(), SIGNAL(clientConnected()),
                &QTestEventLoop::instance(), SLOT(exitLoop()));
        tcpSocket->connectToHost(QHostAddress::LocalHost, tcpServerPort);
        QTestEventLoop::instance().enterLoop(5);
        if (QTestEventLoop::instance().timeout() || !tcpSocket->waitForConnected()) {
            delete tcpSocket;
            return 0;
        }

        socket = new QJsonRpcSocket(tcpSocket);
        tcpSockets.append(tcpSocket);
    } else if (serverType == HttpServer) {
        QJsonRpcHttpClient *client = new QJsonRpcHttpClient;
        client->setEndPoint("http://127.0.0.1:" + QString::number(httpServerPort));
        socket = client;
    }

    return socket;
}

void TestQJsonRpcServer::init()
{
    QFETCH_GLOBAL(ServerType, serverType);
    if (serverType == LocalServer) {
        localServer.reset(new QJsonRpcLocalServer);
        QVERIFY(localServer->listen("qjsonrpc-test-local-server"));
        localServer->moveToThread(&serverThread);
        server = localServer.data();
    } else if (serverType == TcpServer) {
        tcpServer.reset(new QJsonRpcTcpServer);
        tcpServerPort = quint16(91919 + qrand() % 1000);
        QVERIFY(tcpServer->listen(QHostAddress::LocalHost, tcpServerPort));
        tcpServer->moveToThread(&serverThread);
        server = tcpServer.data();
    } else if (serverType == HttpServer) {
        httpServer.reset(new QJsonRpcHttpServer);
        httpServerPort = quint16(8118 + qrand() % 1000);
        QVERIFY(httpServer->listen(QHostAddress::LocalHost, httpServerPort));
        httpServer->moveToThread(&serverThread);
        server = httpServer.data();
    }

    clientSocket.reset(createClient());
    QVERIFY(!clientSocket.isNull());

    if (serverType == LocalServer || serverType == TcpServer)
        QCOMPARE(server->connectedClientCount(), 1);
}

void TestQJsonRpcServer::cleanup()
{
    QFETCH_GLOBAL(ServerType, serverType);
    if (serverType == TcpServer) {
        // disconnect clients
        while (!tcpSockets.isEmpty()) {
            QTcpSocket *tcpSocket = tcpSockets.takeFirst();
            connect(tcpServer.data(), SIGNAL(clientDisconnected()), &QTestEventLoop::instance(), SLOT(exitLoop()));
            if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
                tcpSocket->disconnectFromHost();
                QTestEventLoop::instance().enterLoop(5);
                QVERIFY(!QTestEventLoop::instance().timeout());
            }

            delete tcpSocket;
        }

        // close server
        tcpServer->close();
    } else if (serverType == LocalServer) {
        // disconnect clients
        while (!localSockets.isEmpty()) {
            QLocalSocket *localSocket = localSockets.takeFirst();
            connect(localServer.data(), SIGNAL(clientDisconnected()), &QTestEventLoop::instance(), SLOT(exitLoop()));
            if (localSocket->state() == QLocalSocket::ConnectedState) {
                localSocket->disconnectFromServer();
                QTestEventLoop::instance().enterLoop(5);
                QVERIFY(!QTestEventLoop::instance().timeout());
            }

            delete localSocket;
        }

        // close server
        localServer->close();
    } else if (serverType == HttpServer) {
        httpServer->close();
    }

    QCOMPARE(server->connectedClientCount(), 0);
}

void TestQJsonRpcServer::noParameter()
{
    QVERIFY(server->addService(new TestService));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.noParam");
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QCOMPARE(request.id(), response.id());
    QCOMPARE(spyMessageReceived.count(), 1);
}

void TestQJsonRpcServer::singleParameter()
{
    QVERIFY(server->addService(new TestService));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.singleParam", QString("single"));
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QCOMPARE(request.id(), response.id());
    QCOMPARE(response.result().toString(), QLatin1String("single"));
}

void TestQJsonRpcServer::overloadedMethod()
{
    QVERIFY(server->addService(new TestService));
    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));

    QJsonRpcMessage stringRequest = QJsonRpcMessage::createRequest("service.overloadedMethod", QString("single"));
    QJsonRpcMessage stringResponse = clientSocket->sendMessageBlocking(stringRequest);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVERIFY(stringResponse.errorCode() == QJsonRpc::NoError);
    QCOMPARE(stringRequest.id(), stringResponse.id());
    QCOMPARE(stringResponse.result().toBool(), false);

    QJsonRpcMessage intRequest = QJsonRpcMessage::createRequest("service.overloadedMethod", 10);
    QJsonRpcMessage intResponse = clientSocket->sendMessageBlocking(intRequest);
    QCOMPARE(spyMessageReceived.count(), 2);
    QVERIFY(intResponse.errorCode() == QJsonRpc::NoError);
    QCOMPARE(intRequest.id(), intResponse.id());
    QCOMPARE(intResponse.result().toBool(), true);

    QVariantMap testMap;
    testMap["one"] = 1;
    testMap["two"] = 2;
    testMap["three"] = 3;
    QJsonRpcMessage mapRequest =
        QJsonRpcMessage::createRequest("service.overloadedMethod", QJsonValue::fromVariant(testMap));
    QJsonRpcMessage mapResponse = clientSocket->sendMessageBlocking(mapRequest);
    QCOMPARE(spyMessageReceived.count(), 3);
    QVERIFY(mapResponse.errorCode() == QJsonRpc::InvalidParams);
    QCOMPARE(mapRequest.id(), mapResponse.id());
}

void TestQJsonRpcServer::multiParameter()
{
    QVERIFY(server->addService(new TestService));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));

    QJsonArray params;
    params.append(QLatin1String("a"));
    params.append(QLatin1String("b"));
    params.append(QLatin1String("c"));
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.multipleParam", params);
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QCOMPARE(request.id(), response.id());
    QCOMPARE(response.result().toString(), QLatin1String("abc"));
}

void TestQJsonRpcServer::variantParameter()
{
    QVERIFY(server->addService(new TestService));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));

    QJsonArray params;
    params.append(QJsonValue::fromVariant(QVariant(true)));
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.variantParameter", params);
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QCOMPARE(request.id(), response.id());
    QVERIFY(response.result() == true);
}

void TestQJsonRpcServer::variantListParameter()
{
    QVERIFY(server->addService(new TestService));

    QJsonArray data;
    data.append(1);
    data.append(20);
    data.append(QLatin1String("hello"));
    data.append(false);

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));

    QJsonArray params;
    params.append(data);
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.variantListParameter", params);
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QCOMPARE(request.id(), response.id());
    QCOMPARE(response.result().toArray(), data);
}

void TestQJsonRpcServer::variantResult()
{
    QVERIFY(server->addService(new TestService));

    QJsonRpcMessage response =
        clientSocket->invokeRemoteMethodBlocking("service.variantStringResult");
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QString stringResult = response.result().toString();
    QCOMPARE(stringResult, QLatin1String("hello"));
}

void TestQJsonRpcServer::invalidArgs()
{
    QVERIFY(server->addService(new TestService));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.noParam", false);
    clientSocket->sendMessageBlocking(request);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVariant message = spyMessageReceived.takeFirst().at(0);
    QJsonRpcMessage error = message.value<QJsonRpcMessage>();
    QCOMPARE(request.id(), error.id());
    QVERIFY(error.errorCode() == QJsonRpc::InvalidParams);
}

void TestQJsonRpcServer::methodNotFound()
{
    QVERIFY(server->addService(new TestService));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.doesNotExist");
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVERIFY(response.isValid());
    QVariant message = spyMessageReceived.takeFirst().at(0);
    QJsonRpcMessage error = message.value<QJsonRpcMessage>();
    QCOMPARE(request.id(), error.id());
    QVERIFY(error.errorCode() == QJsonRpc::MethodNotFound);
}

void TestQJsonRpcServer::invalidRequest()
{
    QVERIFY(server->addService(new TestService));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request = QJsonRpcMessage::fromJson("{\"jsonrpc\": \"2.0\", \"id\": 666}");
    clientSocket->sendMessageBlocking(request);

    QCOMPARE(spyMessageReceived.count(), 1);
    QVariant message = spyMessageReceived.takeFirst().at(0);
    QJsonRpcMessage error = message.value<QJsonRpcMessage>();
    QCOMPARE(request.id(), error.id());
    QVERIFY(error.errorCode() == QJsonRpc::InvalidRequest);
}

void TestQJsonRpcServer::qVariantMapInvalidParam()
{
    QVERIFY(server->addService(new TestService));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    const char *invalid = "{\"jsonrpc\": \"2.0\", \"id\": 0, \"method\": \"service.variantMapInvalidParam\",\"params\": [[{\"foo\":\"bar\",\"baz\":\"quux\"}, {\"foo\":\"bar\"}]]}";
    QJsonRpcMessage request = QJsonRpcMessage::fromJson(invalid);
    clientSocket->sendMessageBlocking(request);

    QCOMPARE(spyMessageReceived.count(), 1);
    QVariant message = spyMessageReceived.takeFirst().at(0);
    QJsonRpcMessage error = message.value<QJsonRpcMessage>();
    QCOMPARE(request.id(), error.id());
    QVERIFY(error.errorCode() == QJsonRpc::InvalidParams);
}

class ServerNotificationHelper : public QObject
{
    Q_OBJECT
public:
    ServerNotificationHelper(const QJsonRpcMessage &message, QJsonRpcAbstractServer *provider)
        : m_provider(provider),
          m_notification(message) {}

public Q_SLOTS:
    void activate() {
        m_provider->notifyConnectedClients(m_notification);
    }

private:
    QJsonRpcAbstractServer *m_provider;
    QJsonRpcMessage m_notification;

};

void TestQJsonRpcServer::notifyConnectedClients_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<QJsonRpcMessage::Type>("type");
    QTest::addColumn<QJsonArray>("parameters");
    QTest::addColumn<bool>("sendAsMessage");

    QTest::newRow("notification-message") << "testNotification" << QJsonRpcMessage::Notification << QJsonArray() << true;
    QTest::newRow("notification-direct") << "testNotification" << QJsonRpcMessage::Notification << QJsonArray() << false;

    QJsonArray parameters;
    parameters.append(QLatin1String("test"));
    QTest::newRow("request-message") << "testRequest" << QJsonRpcMessage::Request << parameters << true;
    QTest::newRow("request-direct") << "testRequest" << QJsonRpcMessage::Request << parameters << false;
}

void TestQJsonRpcServer::notifyConnectedClients()
{
    QFETCH(QString, method);
    QFETCH(QJsonRpcMessage::Type, type);
    QFETCH(QJsonArray, parameters);
    QFETCH(bool, sendAsMessage);
    QFETCH_GLOBAL(ServerType, serverType);

    if (serverType == HttpServer) {
#if QT_VERSION >= 0x050000
        QSKIP("Not supported for HTTP connections");
#else
        QSKIP("Not supported for HTTP connections", SkipAll);
#endif
    }

    QVERIFY(server->addService(new TestService));
    QEventLoop loop;
    connect(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)), &loop, SLOT(quit()));
    QSignalSpy spy(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage message;
    if (sendAsMessage) {
        switch (type) {
        case QJsonRpcMessage::Request:
            message = QJsonRpcMessage::createRequest(method, parameters);
            break;
        case QJsonRpcMessage::Notification:
            message = QJsonRpcMessage::createNotification(method, parameters);
            break;
        default:
            break;
        }

        if (serverType == TcpServer)
            QMetaObject::invokeMethod(tcpServer.data(), "notifyConnectedClients", Q_ARG(QJsonRpcMessage, message));
        else if (serverType == LocalServer)
            QMetaObject::invokeMethod(localServer.data(), "notifyConnectedClients", Q_ARG(QJsonRpcMessage, message));

        // server->notifyConnectedClients(message);
    } else {
        if (serverType == TcpServer)
            QMetaObject::invokeMethod(tcpServer.data(), "notifyConnectedClients", Q_ARG(QString, method), Q_ARG(QJsonArray, parameters));
        else if (serverType == LocalServer)
            QMetaObject::invokeMethod(localServer.data(), "notifyConnectedClients", Q_ARG(QString, method), Q_ARG(QJsonArray, parameters));
        // server->notifyConnectedClients(method, parameters);
    }

    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();

    QCOMPARE(spy.count(), 1);
    QJsonRpcMessage receivedMessage = spy.takeFirst().first().value<QJsonRpcMessage>();
    if (sendAsMessage) {
        QCOMPARE(receivedMessage, message);
    } else {
        QCOMPARE(receivedMessage.method(), method);
        QCOMPARE(receivedMessage.params().toArray(), parameters);
    }
}

void TestQJsonRpcServer::numberParameters()
{
    TestService *service = new TestService;
    QVERIFY(server->addService(service));

    QJsonArray params;
    params.append(10);
    params.append(3.14159);
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.numberParameters", params);
    clientSocket->sendMessageBlocking(request);
    QCOMPARE(service->callCount(), 1);
}

void TestQJsonRpcServer::hugeResponse()
{
    QVERIFY(server->addService(new TestService));
    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.hugeResponse");
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVERIFY(response.isValid());
}

void TestQJsonRpcServer::complexMethod()
{
    QVERIFY(server->addService(new TestComplexMethodService));
    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.complex.prefix.for.testMethod");
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QCOMPARE(spyMessageReceived.count(), 1);
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QCOMPARE(request.id(), response.id());
}

void TestQJsonRpcServer::defaultParameters()
{
    QVERIFY(server->addService(new TestService));

    // test without name
    QJsonRpcMessage noNameRequest =
        QJsonRpcMessage::createRequest("service.defaultParametersMethod");
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(noNameRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE(response.result().toString(), QLatin1String("empty string"));

    // test with name
    QJsonRpcMessage nameRequest =
        QJsonRpcMessage::createRequest("service.defaultParametersMethod", QLatin1String("matt"));
    response = clientSocket->sendMessageBlocking(nameRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE(response.result().toString(), QLatin1String("hello matt"));

    // test multiparameter
    QJsonRpcMessage konyRequest =
        QJsonRpcMessage::createRequest("service.defaultParametersMethod2", QLatin1String("KONY"));
    response = clientSocket->sendMessageBlocking(konyRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE(response.result().toString(), QLatin1String("KONY2012"));
}

void TestQJsonRpcServer::notifyServiceSocket()
{
    QFETCH_GLOBAL(ServerType, serverType);
    if (serverType == HttpServer) {
#if QT_VERSION >= 0x050000
        QSKIP("Not supported for HTTP connections");
#else
        QSKIP("Not supported for HTTP connections", SkipAll);
#endif
    }

    QScopedPointer<QJsonRpcServiceSocket> serviceSocket;
    clientSocket.reset();   // we only want a service socket, this would override that
    if (serverType == TcpServer) {
        serviceSocket.reset(new QJsonRpcServiceSocket(tcpSockets.first()));
    } else if (serverType == LocalServer) {
        serviceSocket.reset(new QJsonRpcServiceSocket(localSockets.first()));
    }

    TestService *service = new TestService;
    QVERIFY(serviceSocket->addService(service));
    QCOMPARE(service->callCount(), 0);
    connect(service, SIGNAL(numberParametersCalled()), &QTestEventLoop::instance(), SLOT(exitLoop()), Qt::QueuedConnection);

    QJsonArray params;
    params.append(10);
    params.append(3.14159);
    QJsonRpcMessage notification = QJsonRpcMessage::createNotification("service.numberParameters", params);
    if (serverType == TcpServer)
        QMetaObject::invokeMethod(tcpServer.data(), "notifyConnectedClients", Q_ARG(QJsonRpcMessage, notification));
    else if (serverType == LocalServer)
        QMetaObject::invokeMethod(localServer.data(), "notifyConnectedClients", Q_ARG(QJsonRpcMessage, notification));
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QCOMPARE(service->callCount(), 1);
}

/*
Q_DECLARE_METATYPE(QList<int>)
void TestQJsonRpcServer::testListOfInts()
{
    server->addService(new TestService);
    qRegisterMetaType<QList<int> >("QList<int>");
    QList<int> intList = QList<int>() << 300 << 30 << 3;
    QVariant variant = QVariant::fromValue(intList);
    QJsonRpcMessage intRequest =
        QJsonRpcMessage::createRequest("service.methodWithListOfInts", variant);
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(intRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QVERIFY(response.result().toBool());
}
*/

void TestQJsonRpcServer::stringListParameter()
{
    QVERIFY(server->addService(new TestService));
    QStringList strings = QStringList() << "one" << "two" << "three";

    QJsonArray params;
    params.append(1);
    params.append(QLatin1String("A"));
    params.append(QLatin1String("B"));
    params.append(QJsonValue::fromVariant(strings));
    QJsonRpcMessage strRequest =
        QJsonRpcMessage::createRequest("service.stringListParameter", params);
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(strRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QVERIFY(response.result().toBool());
}

void TestQJsonRpcServer::outputParameter()
{
    QVERIFY(server->addService(new TestService));

    // use argument 2 as in/out parameter
    QJsonArray arrParams;
    arrParams.push_back(1);
    arrParams.push_back(0);
    arrParams.push_back(2);
    QJsonRpcMessage strRequest =
        QJsonRpcMessage::createRequest("service.outputParameter", arrParams);
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(strRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE((int) response.result().toDouble(), 3);

    // only input parameters are provided
    QJsonObject objParams;
    objParams["in1"] = 1;
    objParams["in2"] = 3;
    strRequest =
        QJsonRpcMessage::createRequest("service.outputParameter", objParams);
    response = clientSocket->sendMessageBlocking(strRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE((int) response.result().toDouble(), 4);

    // also provide the in/out parameter
    objParams["out"] = 2;
    strRequest =
        QJsonRpcMessage::createRequest("service.outputParameter", objParams);
    response = clientSocket->sendMessageBlocking(strRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE((int) response.result().toDouble(), 6);

    // test strings
    QJsonArray stringParams;
    stringParams.push_back(QLatin1String("Sherlock"));
    stringParams.push_back(QLatin1String(""));
    stringParams.push_back(QLatin1String("Holmes"));
    strRequest =
        QJsonRpcMessage::createRequest("service.outputParameterWithStrings", stringParams);
    response = clientSocket->sendMessageBlocking(strRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE(response.result().toString(), QLatin1String("Sherlock Holmes"));

    // only input parameters are provided
    QJsonObject stringObjectParams;
    stringObjectParams["first"] = QLatin1String("Sherlock");
    stringObjectParams["output"] = QLatin1String("Hello");
    stringObjectParams["last"] = QLatin1String("Holmes");
    strRequest =
        QJsonRpcMessage::createRequest("service.outputParameterWithStrings", stringObjectParams);
    response = clientSocket->sendMessageBlocking(strRequest);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE(response.result().toString(), QLatin1String("Hello Sherlock Holmes"));
}

void TestQJsonRpcServer::userDeletesReplyOnDelayedResponse()
{
    QVERIFY(server->addService(new TestService));
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.delayedResponse");

    connect(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QJsonRpcServiceReply *reply = clientSocket->sendMessage(request);
    delete reply;

    // While this is not applicable for HTTP connections, I've left the code
    // up until this point to ensure that no memory is leaked

    QFETCH_GLOBAL(ServerType, serverType);
    if (serverType == HttpServer) {
#if QT_VERSION >= 0x050000
        QSKIP("Not applicable for HTTP connections");
#else
        QSKIP("Not applicable for HTTP connections", SkipAll);
#endif
    }

    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());
}

#if QT_VERSION >= 0x050200
void TestQJsonRpcServer::jsonReturnTypes()
{
    QVERIFY(server->addService(new TestService));

    {
        QJsonArray array;
        array.append(1);
        array.append(QLatin1String("two"));
        array.append(true);
        QJsonRpcMessage request =
            QJsonRpcMessage::createRequest("service.returnQJsonArray");
        QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
        QVERIFY(response.type() != QJsonRpcMessage::Error);
        QCOMPARE(response.result().toArray(), array);
    }

    {
        QJsonObject object;
        object.insert("one", QLatin1String("one"));
        object.insert("two", 2);
        object.insert("three", true);
        QJsonRpcMessage request =
            QJsonRpcMessage::createRequest("service.returnQJsonObject");
        QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
        QVERIFY(response.type() != QJsonRpcMessage::Error);
        QCOMPARE(response.result().toObject(), object);
    }
}
#endif

class QJsonRpcServiceReplySpy : public QObject
{
    Q_OBJECT
public:
    explicit QJsonRpcServiceReplySpy(int expectedReplies, QObject *parent = 0)
        : QObject(parent),
          m_expectedReplies(expectedReplies)
    {
    }

    QList<QJsonRpcMessage> responses() const { return m_responses; }

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void replyFinished() {
        QJsonRpcServiceReply *reply = qobject_cast<QJsonRpcServiceReply*>(sender());
        if (!reply)
            return;

        m_responses.append(reply->response());
        reply->deleteLater();

        if (m_responses.size() == m_expectedReplies)
            Q_EMIT finished();
    }

private:
    int m_expectedReplies;
    QList<QJsonRpcMessage> m_responses;

};

void TestQJsonRpcServer::delayedResponseBasic()
{
    QFETCH_GLOBAL(ServerType, serverType);
    if (serverType == HttpServer) {
#if QT_VERSION >= 0x050000
        QSKIP("QNAM makes deterministic order impossible here");
#else
        QSKIP("QNAM makes deterministic order impossible here", SkipAll);
#endif
    }

    QVERIFY(server->addService(new TestDelayedResponseService));
    QJsonRpcServiceReplySpy spy(6);
    connect(&spy, SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));

    int delayedMessageId = 0;
    {
        QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.delayedResponse");
        QJsonRpcServiceReply *reply = clientSocket->sendMessage(request);
        connect(reply, SIGNAL(finished()), &spy, SLOT(replyFinished()));
        delayedMessageId = request.id();
    }

    QList<int> expectedMessageOrder;
    for (int i = 0; i < 5; ++i) {
        QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.immediateResponse");
        QJsonRpcServiceReply *reply = clientSocket->sendMessage(request);
        connect(reply, SIGNAL(finished()), &spy, SLOT(replyFinished()));
        expectedMessageOrder.append(request.id());
    }
    expectedMessageOrder.append(delayedMessageId);

    QTestEventLoop::instance().enterLoop(10);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QList<int> actualMessageOrder;
    foreach (QJsonRpcMessage response, spy.responses())
        actualMessageOrder.append(response.id());
    QCOMPARE(expectedMessageOrder, actualMessageOrder);
}

void TestQJsonRpcServer::delayedResponseSocketClosed()
{
    QFETCH_GLOBAL(ServerType, serverType);
    if (serverType == HttpServer) {
#if QT_VERSION >= 0x050000
        QSKIP("Not applicable for HTTP connections");
#else
        QSKIP("Not applicable for HTTP connections", SkipAll);
#endif
    }

    TestDelayedResponseService *service = new TestDelayedResponseService;
    QVERIFY(server->addService(service));

    QSignalSpy spy(service, SIGNAL(responseResult(bool)));
    connect(service, SIGNAL(responseResult(bool)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.delayedResponseWithClosedSocket");
    QScopedPointer<QJsonRpcServiceReply> reply(clientSocket->sendMessage(request));
    if (serverType == TcpServer)
        tcpSockets.first()->disconnectFromHost();
    else if (serverType == LocalServer)
        localSockets.first()->disconnectFromServer();

    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toBool(), false);
}

void TestQJsonRpcServer::addRemoveService()
{
    TestService service;
    QVERIFY(server->addService(&service));

    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.noParam");
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QCOMPARE(request.id(), response.id());
    QCOMPARE(spyMessageReceived.count(), 1);

    QVERIFY(server->removeService(&service));
    response = clientSocket->sendMessageBlocking(request);
    QVERIFY(response.errorCode() == QJsonRpc::MethodNotFound);

    QFETCH_GLOBAL(ServerType, serverType);
    if (serverType == TcpServer)
        QVERIFY(tcpServer->errorString().isEmpty());
    else if (serverType == LocalServer)
        QVERIFY(localServer->errorString().isEmpty());
}

void TestQJsonRpcServer::serviceWithNoGivenName()
{
    QVERIFY(server->addService(new TestServiceWithoutServiceName));
    QSignalSpy spyMessageReceived(clientSocket.data(), SIGNAL(messageReceived(QJsonRpcMessage)));
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("testservicewithoutservicename.testMethod", QLatin1String("foo"));
    QJsonRpcMessage response = clientSocket->sendMessageBlocking(request);
    QVERIFY(response.errorCode() == QJsonRpc::NoError);
    QCOMPARE(request.id(), response.id());
    QCOMPARE(spyMessageReceived.count(), 1);
}

void TestQJsonRpcServer::cantRemoveInvalidService()
{
    TestService service;
    QCOMPARE(server->removeService(&service), false);
}

void TestQJsonRpcServer::cantAddServiceTwice()
{
    TestService service;
    QVERIFY(server->addService(&service));
    QCOMPARE(server->addService(&service), false);
}

QTEST_MAIN(TestQJsonRpcServer)
#include "tst_qjsonrpcserver.moc"
