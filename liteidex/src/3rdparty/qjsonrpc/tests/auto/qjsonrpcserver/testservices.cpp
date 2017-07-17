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
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QtTest>

#include "qjsonrpcsocket.h"
#include "testservices.h"

TestService::TestService(QObject *parent)
    : QJsonRpcService(parent),
      m_called(0)
{
}

void TestService::resetCount()
{
    m_called = 0;
}

int TestService::callCount() const
{
    return m_called;
}

void TestService::noParam() const
{
}

QString TestService::singleParam(const QString &string) const
{
    return string;
}

QString TestService::multipleParam(const QString &first, const QString &second, const QString &third) const
{
    return first + second + third;
}

void TestService::numberParameters(int intParam, double doubleParam)
{
    if (intParam == 10 && doubleParam == 3.14159) {
        m_called++;
        Q_EMIT numberParametersCalled();
    }
}

bool TestService::variantParameter(const QVariant &variantParam) const
{
    return variantParam.toBool();
}

QVariantList TestService::variantListParameter(const QVariantList &data)
{
    return data;
}

QVariant TestService::variantStringResult()
{
    return QLatin1String("hello");
}

QVariantList TestService::variantListResult()
{
    return QVariantList() << "one" << 2 << 3.0;
}

QVariantMap TestService::variantMapResult()
{
    QVariantMap result;
    result["one"] = 1;
    result["two"] = 2.0;
    return result;
}

/* NOTE: suppress binding warnings
bool TestService::methodWithListOfInts(const QList<int> &list)
{
    if (list.size() < 3)
        return false;
    if (list.at(0) != 300)
        return false;
    if (list.at(1) != 30)
        return false;
    if (list.at(2) != 3)
        return false;
    return true;
}
*/

QString TestService::variantMapInvalidParam(const QVariantMap &map)
{
    return map["foo"].toString();
}

void TestService::outputParameter(int in1, int &out, int in2)
{
    out = in1 + out + in2;
}

void TestService::outputParameterWithStrings(const QString &first, QString &output, const QString &last)
{
    if (output.isEmpty())
        output = QString("%1 %2").arg(first).arg(last);
    else
        output.append(QString(" %1 %2").arg(first).arg(last));
}

bool TestService::overloadedMethod(int input)
{
    Q_UNUSED(input)
    return true;
}

bool TestService::overloadedMethod(const QString &input)
{
    Q_UNUSED(input)
    return false;
}

bool TestService::stringListParameter(int one, const QString &two, const QString &three, const QStringList &list)
{
    Q_UNUSED(one);
    Q_UNUSED(two);
    Q_UNUSED(three);
    Q_UNUSED(list);
    return true;
}

QJsonArray TestService::returnQJsonArray()
{
    QJsonArray array;
    array.append(1);
    array.append(QLatin1String("two"));
    array.append(true);
    return array;
}

QJsonObject TestService::returnQJsonObject()
{
    QJsonObject object;
    object.insert("one", QLatin1String("one"));
    object.insert("two", 2);
    object.insert("three", true);
    return object;
}

QVariantMap TestService::hugeResponse()
{
    QVariantMap result;
    for (int i = 0; i < 1000; i++) {
        QString key = QString("testKeyForHugeResponse%1").arg(i);
        result[key] = "some sample data to make the response larger";
    }

    return result;
}

QString TestService::defaultParametersMethod(const QString &name)
{
    if (name.isEmpty())
        return "empty string";
    return QString("hello %1").arg(name);
}

QString TestService::defaultParametersMethod2(const QString &name, int year)
{
    return QString("%1%2").arg(name).arg(year);
}

TestServiceWithoutServiceName::TestServiceWithoutServiceName(QObject *parent)
    : QJsonRpcService(parent)
{
}

QString TestServiceWithoutServiceName::testMethod(const QString &string) const
{
    return string;
}

TestComplexMethodService::TestComplexMethodService(QObject *parent)
    : QJsonRpcService(parent)
{
}

void TestComplexMethodService::testMethod()
{
}

TestDelayedResponseService::TestDelayedResponseService(QObject *parent)
    : QJsonRpcService(parent)
{
}

void TestDelayedResponseService::delayedResponse()
{
    QVERIFY(currentRequest().isValid());
    beginDelayedResponse();
    m_request = currentRequest();
    QTimer::singleShot(250, this, SLOT(delayedResponseComplete()));
}

void TestDelayedResponseService::delayedResponseWithClosedSocket()
{
    QVERIFY(currentRequest().isValid());
    beginDelayedResponse();
    m_request = currentRequest();
    QTimer::singleShot(250, this, SLOT(delayedResponseWithClosedSocketComplete()));
}

QString TestDelayedResponseService::immediateResponse()
{
    return QLatin1String("immediate");
}

void TestDelayedResponseService::delayedResponseComplete()
{
    m_request.respond(QLatin1String("delayed"));
}

void TestDelayedResponseService::delayedResponseWithClosedSocketComplete()
{
    QJsonRpcMessage requestMessage = m_request.request();
    QJsonRpcMessage responseMessage = requestMessage.createResponse(QLatin1String("delayed"));
    bool result = m_request.respond(responseMessage);
    Q_EMIT responseResult(result);
}
