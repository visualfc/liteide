/*
 * Copyright (C) 2013 Fargier Sylvain
 * Copyright (C) 2014 Matt Broadstone
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

#include "qjsonrpcmetatype.h"
#include "qjsonrpcabstractserver.h"
#include "qjsonrpcmessage.h"
#include "qjsonrpcservice.h"

class TestQJsonRpcMetaType: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void customParameterTypes();
    void customReturnTypes();
    void invalidParameterTypes();
    void enums();

private:
    /*
     * There's no way to guess which method is the best to dispatch a custom type
     * for the moment, something like a signature, or a typetesting functor might
     * be good.
     *
     * This test will fail until then
     */
    void commonMethodName();

};

class CustomClass : public QObject
{
public:
    CustomClass(int data = 0, QObject *parent = 0)
        : QObject(parent),
          data(data)
    {}

    CustomClass(const CustomClass &other)
        : QObject(),
          data(other.data)
    {}

    CustomClass &operator=(const CustomClass &other) {
        if (&other != this)
            data = other.data;
        return *this;
    }

    QJsonValue toJson() const {
        return QJsonValue(data);
    }

    static CustomClass fromJson(const QJsonValue &value) {
        return CustomClass(value.toInt());
    }

    int data;
};
Q_DECLARE_METATYPE(CustomClass)

class AnotherCustomClass
{
public:
    explicit AnotherCustomClass(const QString &str = QString())
        : data(str)
    {}

    QJsonValue toJson() const {
        return QJsonValue(data);
    }

    static AnotherCustomClass fromJson(const QJsonValue &value) {
        return AnotherCustomClass(value.toString());
    }

    QString data;
};
Q_DECLARE_METATYPE(AnotherCustomClass)

class UnboundClass : public QObject
{
public:
    UnboundClass(QObject *parent = 0)
        : QObject(parent)
    {}
};

class TestService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", "service")
    Q_ENUMS(TestEnum)

public:
    TestService(QObject *parent = 0)
        : QJsonRpcService(parent)
    {}

    QJsonRpcMessage testDispatch(const QJsonRpcMessage &message) {
        return QJsonRpcService::dispatch(message);
    }

    enum TestEnum {
        ZERO = 0, ONE = 1, TWO = 2, THREE = 3
    };

    static QMetaEnum TestMetaEnum() {
        static int i = TestService::staticMetaObject.indexOfEnumerator("TestEnum");
        return TestService::staticMetaObject.enumerator(i);
    }

public Q_SLOTS:
    void customParameterType(const CustomClass &param) const {
        QCOMPARE(param.data, 42);
    }

    CustomClass customReturnType(const CustomClass &param) const {
        CustomClass ret(param);
        ++ret.data;
        return ret;
    }

    void invalidParameterType(const UnboundClass &) const {
        QVERIFY(false);
    }

    void enums(TestService::TestEnum) {
        QVERIFY(true);
    }

    int commonMethodName(const CustomClass &c) {
        return c.data;
    }

    QString commonMethodName(const AnotherCustomClass &c) {
        return c.data;
    }
};
Q_DECLARE_METATYPE(TestService::TestEnum)

QJsonValue toJson(TestService::TestEnum e)
{
    return QString(TestService::TestMetaEnum().valueToKey(e));
}

TestService::TestEnum fromJson(const QJsonValue &val)
{
    if (val.isString()) {
        QString str(val.toString());
        if (str.isEmpty())
            return TestService::ZERO;
        return (TestService::TestEnum) TestService::TestMetaEnum().keysToValue(str.toLatin1().constData());
    } else if (val.isDouble()) {
        int idx = (int) val.toDouble();
        if (!TestService::TestMetaEnum().valueToKey(idx))
            return TestService::ZERO;
        return (TestService::TestEnum) idx;
    }

    return TestService::ZERO;
}

class TestServiceProvider : public QObject, public QJsonRpcServiceProvider
{
public:
    TestServiceProvider() {}
};

void TestQJsonRpcMetaType::initTestCase()
{
    qRegisterJsonRpcMetaType<CustomClass>("CustomClass");
    qRegisterJsonRpcMetaType<AnotherCustomClass>("CustomClass");

    // TODO: incorporate enums into qRegisterJsonRpcMetaType
    QMetaType::registerConverter<TestService::TestEnum, QJsonValue>(&toJson);
    QMetaType::registerConverter<QJsonValue, TestService::TestEnum>(&fromJson);
}

/*
 * Custom class parameter
 */
void TestQJsonRpcMetaType::customParameterTypes()
{
    TestServiceProvider provider;
    TestService service;
    provider.addService(&service);

    CustomClass custom(42);
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.customParameterType", custom.toJson());
    QJsonRpcMessage response = service.testDispatch(request);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
}

void TestQJsonRpcMetaType::customReturnTypes()
{
    TestServiceProvider provider;
    TestService service;
    provider.addService(&service);

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.customReturnType", CustomClass().toJson());
    QJsonRpcMessage response = service.testDispatch(request);

    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QVariant result = response.result();
    QVERIFY(result.canConvert<CustomClass>());
    QCOMPARE(result.value<CustomClass>().data, 1);
}

void TestQJsonRpcMetaType::invalidParameterTypes()
{
    TestServiceProvider provider;
    TestService service;
    provider.addService(&service);

    UnboundClass custom;
    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.invalidParameterType",
                QJsonValue::fromVariant(QVariant::fromValue(CustomClass())));
    QJsonRpcMessage response = service.testDispatch(request);
    QVERIFY(response.type() == QJsonRpcMessage::Error);
    QCOMPARE(response.errorCode(), (int) QJsonRpc::MethodNotFound);
}

void TestQJsonRpcMetaType::enums()
{
    TestServiceProvider provider;
    TestService service;
    provider.addService(&service);

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.enums", TestService::ONE);
    QJsonRpcMessage response = service.testDispatch(request);
    QVERIFY(response.type() != QJsonRpcMessage::Error);

    request = QJsonRpcMessage::createRequest("service.enums", QLatin1String("ONE"));
    response = service.testDispatch(request);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
}

void TestQJsonRpcMetaType::commonMethodName()
{
    TestServiceProvider provider;
    TestService service;
    provider.addService(&service);

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest("service.commonMethodName", CustomClass(42).toJson());
    QJsonRpcMessage response = service.testDispatch(request);
    QVERIFY(response.type() != QJsonRpcMessage::Error);

    CustomClass c(CustomClass::fromJson(response.result()));
    QCOMPARE(c.data, 42);

    request = QJsonRpcMessage::createRequest("service.commonMethodName",
                AnotherCustomClass("test string").toJson());
    response = service.testDispatch(request);
    QVERIFY(response.type() != QJsonRpcMessage::Error);

    AnotherCustomClass ac(AnotherCustomClass::fromJson(response.result()));
    QCOMPARE(ac.data, QLatin1String("test string"));
}

QTEST_MAIN(TestQJsonRpcMetaType)
#include "tst_qjsonrpcmetatype.moc"
