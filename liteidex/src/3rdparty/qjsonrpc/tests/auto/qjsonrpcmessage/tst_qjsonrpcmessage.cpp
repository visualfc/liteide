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
#include <QtCore/QVariant>
#include <QtTest/QtTest>

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#else
#include "json/qjsondocument.h"
#endif

#include "qjsonrpcmessage.h"

class TestQJsonRpcMessage: public QObject
{
    Q_OBJECT
private slots:
    void debugStreams_data();
    void debugStreams();
    void invalidData();
    void invalidStringData_data();
    void invalidStringData();
    void invalidDataResponseWithId();
    void invalidDataResponseWithoutId();
    void responseSameId();
    void notificationNoId();
    void messageTypes();
    void positionalParameters();
    void equivalence_data();
    void equivalence();
    void withVariantListArgs();
    void idSentAsString();
};

void TestQJsonRpcMessage::debugStreams_data()
{
    QTest::addColumn<QJsonRpcMessage>("message");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("invalid-message") << QJsonRpcMessage() <<
        "QJsonRpcMessage(type=QJsonRpcMessage::Invalid, id=0)";
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.method");
    QTest::newRow("request-message") << request <<
        "QJsonRpcMessage(type=QJsonRpcMessage::Request, id=1, method=\"service.method\", params=QJsonValue(undefined) )";

    QJsonRpcMessage response = request.createResponse(QLatin1String("response"));
    QTest::newRow("response-message") << response <<
        "QJsonRpcMessage(type=QJsonRpcMessage::Response, id=1, result=QJsonValue(string, \"response\") )";

    QJsonRpcMessage error = request.createErrorResponse(QJsonRpc::MethodNotFound);
    QTest::newRow("error-message") << error <<
        "QJsonRpcMessage(type=QJsonRpcMessage::Error, id=1, code=-32601, message=\"\", data=QJsonValue(null) )";

    QJsonRpcMessage notification = QJsonRpcMessage::createNotification("service.notification");
    QTest::newRow("notification-message") << notification <<
        "QJsonRpcMessage(type=QJsonRpcMessage::Notification, method=\"service.notification\", params=QJsonValue(undefined) )";
}

void TestQJsonRpcMessage::debugStreams()
{
    QFETCH(QJsonRpcMessage, message);
    QFETCH(QString, expectedOutput);

    QString actualOutput;
    QDebug debug(&actualOutput);
    debug << message;
    QCOMPARE(actualOutput.trimmed(), expectedOutput.trimmed());
}

void TestQJsonRpcMessage::invalidData()
{
    QJsonObject invalidData;
    QJsonRpcMessage message = QJsonRpcMessage::fromObject(invalidData);
    QCOMPARE(message.isValid(), false);
    QCOMPARE(message.type(), QJsonRpcMessage::Invalid);
    QVERIFY(message.toObject().isEmpty());
}

void TestQJsonRpcMessage::invalidStringData_data()
{
    QTest::addColumn<QByteArray>("stringData");
    QTest::newRow("not-json") << QByteArray("invalid json string");
    QTest::newRow("not-an-object") << QByteArray("[\"string\"]");
}

void TestQJsonRpcMessage::invalidStringData()
{
    QFETCH(QByteArray, stringData);
    QJsonRpcMessage message = QJsonRpcMessage::fromJson(stringData);
    QCOMPARE(message.isValid(), false);
    QCOMPARE(message.type(), QJsonRpcMessage::Invalid);
    QVERIFY(message.toObject().isEmpty());
    QVERIFY(message.params().isUndefined() || message.params().isNull());
    QVERIFY(message.result().isUndefined() || message.params().isNull());
    QVERIFY(message.errorData().isUndefined() || message.params().isNull());
}

void TestQJsonRpcMessage::invalidDataResponseWithId()
{
    // invalid with id
    const char *invalid = "{\"jsonrpc\": \"2.0\", \"params\": [], \"id\": 666}";
    QJsonRpcMessage request = QJsonRpcMessage::fromJson(invalid);
    QJsonRpcMessage error =
        request.createErrorResponse(QJsonRpc::NoError, QString());
    QJsonRpcMessage response = request.createResponse(QString());
    QCOMPARE(request.type(), QJsonRpcMessage::Invalid);
    QCOMPARE(response.id(), request.id());
    QCOMPARE(error.type(), QJsonRpcMessage::Error);
}

void TestQJsonRpcMessage::invalidDataResponseWithoutId()
{
    // invalid without id
    const char *invalid = "{\"jsonrpc\": \"2.0\", \"params\": []}";
    QJsonRpcMessage request = QJsonRpcMessage::fromJson(invalid);
    QJsonRpcMessage error =
        request.createErrorResponse(QJsonRpc::NoError, QString());
    QJsonRpcMessage response = request.createResponse(QString());
    QCOMPARE(request.type(), QJsonRpcMessage::Invalid);
    QCOMPARE(response.type(), QJsonRpcMessage::Invalid);
    QCOMPARE(error.id(), 0);
}

void TestQJsonRpcMessage::responseSameId()
{
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("testMethod");
    QJsonRpcMessage response =
        request.createResponse(QLatin1String("testResponse"));
    QCOMPARE(response.id(), request.id());
}

void TestQJsonRpcMessage::notificationNoId()
{
    QJsonRpcMessage notification =
        QJsonRpcMessage::createNotification("testNotification");
    QCOMPARE(notification.id(), -1);
}

void TestQJsonRpcMessage::messageTypes()
{
    QJsonRpcMessage invalid;
    QCOMPARE(invalid.type(), QJsonRpcMessage::Invalid);

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("testMethod");
    QCOMPARE(request.type(), QJsonRpcMessage::Request);

    QJsonRpcMessage response =
        request.createResponse(QLatin1String("testResponse"));
    QCOMPARE(response.type(), QJsonRpcMessage::Response);

    QJsonRpcMessage error = request.createErrorResponse(QJsonRpc::NoError);
    QCOMPARE(error.type(), QJsonRpcMessage::Error);

    QJsonRpcMessage notification =
        QJsonRpcMessage::createNotification("testNotification");
    QCOMPARE(notification.type(), QJsonRpcMessage::Notification);
}

// this is from the spec, I don't think it proves much..
void TestQJsonRpcMessage::positionalParameters()
{
    const char *first = "{\"jsonrpc\": \"2.0\", \"method\": \"subtract\", \"params\": [42, 23], \"id\": 1}";
    QJsonObject firstObject = QJsonDocument::fromJson(first).object();
    const char *second = "{\"jsonrpc\": \"2.0\", \"method\": \"subtract\", \"params\": [23, 42], \"id\": 2}";
    QJsonObject secondObject = QJsonDocument::fromJson(second).object();
    QVERIFY2(firstObject.value("params").toArray() != secondObject.value("params").toArray(), "params should maintain order");
}

void TestQJsonRpcMessage::equivalence_data()
{
    QTest::addColumn<QJsonRpcMessage>("lhs");
    QTest::addColumn<QJsonRpcMessage>("rhs");
    QTest::addColumn<bool>("equal");

    QJsonRpcMessage invalid;
    {
        // REQUESTS
        const char *simpleData =
            "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"request\" }";
        QJsonRpcMessage simpleRequest = QJsonRpcMessage::fromJson(simpleData);
        QJsonRpcMessage simpleRequestCopyCtor(simpleRequest);
        QJsonRpcMessage simpleRequestEqualsOp = simpleRequest;

        const char *withParametersData =
            "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"request\", \"params\": [\"with\", \"parameters\"]}";
        QJsonRpcMessage simpleRequestWithParameters =
            QJsonRpcMessage::fromJson(withParametersData);

        const char *withNamedParametersData =
            "{\"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"request\", \"params\": {\"firstName\": \"yogi\", \"lastName\": \"thebear\"}}";
        QJsonRpcMessage simpleRequestWithNamedParameters =
            QJsonRpcMessage::fromJson(withNamedParametersData);

        QTest::newRow("simpleRequestCopiesEqual_1") << simpleRequest << simpleRequestCopyCtor << true;
        QTest::newRow("simpleRequestCopiesEqual_2") << simpleRequest << simpleRequestEqualsOp << true;
        QTest::newRow("simpleRequestAndSimpleRequestWithParamsNotEqual") << simpleRequest
            << simpleRequestWithParameters << false;
        QTest::newRow("simpleRequestAndSimpleRequestWithNamedParamsNotEqual") << simpleRequest
            << simpleRequestWithNamedParameters << false;
        QTest::newRow("requestWithParamsNotEqualWithNamedParameters")
            << simpleRequestWithParameters << simpleRequestWithNamedParameters << false;
        QTest::newRow("simpleRequestNotEqualInvalid") << simpleRequest << invalid << false;
    }

    {
        // NOTIFICATIONS
        QJsonRpcMessage simpleNotification = QJsonRpcMessage::createNotification("notification");
        QJsonRpcMessage simpleNotificationCopyCtor(simpleNotification);
        QJsonRpcMessage simpleNotificationEqualsOp = simpleNotification;

        QJsonArray params;
        params.append(QLatin1String("yogi"));
        params.append(QLatin1String("thebear"));
        QJsonRpcMessage simpleNotificationWithParams =
            QJsonRpcMessage::createNotification("notification", params);

        QJsonObject namedParameters;
        namedParameters.insert("firstName", QLatin1String("yogi"));
        namedParameters.insert("lastName", QLatin1String("thebear"));
        QJsonRpcMessage simpleNotificationWithNamedParameters =
            QJsonRpcMessage::createNotification("notification", namedParameters);

        QTest::newRow("simpleNotificationCopiesEqual_1")
            << simpleNotification << simpleNotificationCopyCtor << true;
        QTest::newRow("simpleNotificationCopiesEqual_2")
            << simpleNotification << simpleNotificationEqualsOp << true;
        QTest::newRow("simpleNotificationNotEqualNotificationWithParams")
            << simpleNotification << simpleNotificationWithParams << false;
        QTest::newRow("simpleNotificationNotEqualNotificationWithNamedParameters")
            << simpleNotification << simpleNotificationWithNamedParameters << false;
        QTest::newRow("notificationWithParamsNotEqualWithNamedParameters")
            << simpleNotificationWithParams << simpleNotificationWithNamedParameters << false;
        QTest::newRow("simpleNotificationNotEqualInvalid") << simpleNotification << invalid << false;
    }

    {
        // ERRORS
        QJsonRpcMessage basicRequest = QJsonRpcMessage::createRequest("blah", QLatin1String("first"));
        QJsonRpcMessage lhs = basicRequest.createErrorResponse(QJsonRpc::InternalError, "some error");
        QJsonRpcMessage rhs = basicRequest.createErrorResponse(QJsonRpc::InternalError, "some error");
        QJsonRpcMessage invalidRhs = basicRequest.createErrorResponse(QJsonRpc::InvalidRequest, "some error");

        QTest::newRow("errorResponseEqual") << lhs << rhs << true;
        QTest::newRow("errorResponseNotEqual") << lhs << invalidRhs << false;
    }
}

void TestQJsonRpcMessage::equivalence()
{
    QFETCH(QJsonRpcMessage, lhs);
    QFETCH(QJsonRpcMessage, rhs);
    QFETCH(bool, equal);

    if (equal)
        QCOMPARE(lhs, rhs);
    else
        QVERIFY(lhs != rhs);
}

void TestQJsonRpcMessage::withVariantListArgs()
{
    const char *varListArgsFormat = "{ " \
            "\"id\": %1, " \
            "\"jsonrpc\": \"2.0\", " \
            "\"method\": \"service.variantListParameter\", " \
            "\"params\": [[ 1, 20, \"hello\", false ]] " \
            "}";

    QVariantList firstParameter;
    firstParameter << 1 << 20 << "hello" << false;

    QJsonArray params;
    params.append(QJsonArray::fromVariantList(firstParameter));
    QJsonRpcMessage requestFromQJsonRpc =
        QJsonRpcMessage::createRequest("service.variantListParameter", params);

    // QJsonRpcMessage::createRequest is creating objects with an unique id,
    // and to allow a random test execution order - json data must have the same id
    int id = requestFromQJsonRpc.id();
    QByteArray varListArgs = QString(varListArgsFormat).arg(id).toLatin1();

    QJsonRpcMessage requestFromData = QJsonRpcMessage::fromJson(varListArgs);
    QCOMPARE(requestFromQJsonRpc, requestFromData);
}

void TestQJsonRpcMessage::idSentAsString()
{
    const char *messageWithStringId = "{ " \
            "\"id\": \"%1\", " \
            "\"jsonrpc\": \"2.0\", " \
            "\"method\": \"service.someMethod\", " \
            "\"params\": [[ 1, 20, \"hello\", false ]] " \
            "}";

    const char *errorMessageWithStringId = "{ " \
            "\"id\": \"%1\", " \
            "\"jsonrpc\": \"2.0\", " \
            "\"error\": { \"code\": \"-32601\", \"data\": null } " \
            "}";

    QVariantList firstParameter;
    firstParameter << 1 << 20 << "hello" << false;

    QJsonArray params;
    params.append(QJsonArray::fromVariantList(firstParameter));
    QJsonRpcMessage requestFromQJsonRpc =
        QJsonRpcMessage::createRequest("service.someMethod", params);
    int id = requestFromQJsonRpc.id();
    QByteArray messageData = QString(messageWithStringId).arg(id).toLatin1();
    QJsonRpcMessage requestFromData = QJsonRpcMessage::fromJson(messageData);

    QJsonRpcMessage errorFromQJsonRpc =
        requestFromQJsonRpc.createErrorResponse(QJsonRpc::MethodNotFound);
    QByteArray errorData = QString(errorMessageWithStringId).arg(id).toLatin1();
    QJsonRpcMessage errorFromData = QJsonRpcMessage::fromJson(errorData);

    QCOMPARE(requestFromQJsonRpc, requestFromData);
    QCOMPARE(errorFromQJsonRpc, errorFromData);
}

QTEST_MAIN(TestQJsonRpcMessage)
#include "tst_qjsonrpcmessage.moc"
