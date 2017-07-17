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
#ifndef TESTSERVICES_H
#define TESTSERVICES_H

#include "qjsonrpcservice.h"

class TestService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", "service")
public:
    explicit TestService(QObject *parent = 0);

    void resetCount();
    int callCount() const;

Q_SIGNALS:
    void numberParametersCalled();

public Q_SLOTS:
    void noParam() const;
    QString singleParam(const QString &string) const;
    QString multipleParam(const QString &first, const QString &second, const QString &third) const;
    void numberParameters(int intParam, double doubleParam);
    bool variantParameter(const QVariant &variantParam) const;
    QVariantList variantListParameter(const QVariantList &data);
    QVariant variantStringResult();
    QVariantList variantListResult();
    QVariantMap variantMapResult();
    QVariantMap hugeResponse();
    QString defaultParametersMethod(const QString &name = QString());
    QString defaultParametersMethod2(const QString &name = QString(), int year = 2012);

    // NOTE: suppress binding warnings
    // bool methodWithListOfInts(const QList<int> &list);

    QString variantMapInvalidParam(const QVariantMap &map);
    void outputParameter(int in1, int &out, int in2);
    void outputParameterWithStrings(const QString &first, QString &output, const QString &last);
    bool overloadedMethod(int input);
    bool overloadedMethod(const QString &input);
    bool stringListParameter(int one, const QString &two, const QString &three, const QStringList &list);

    // return values
    QJsonArray returnQJsonArray();
    QJsonObject returnQJsonObject();

private:
    int m_called;

};

class TestServiceWithoutServiceName : public QJsonRpcService
{
    Q_OBJECT
public:
    TestServiceWithoutServiceName(QObject *parent = 0);

public Q_SLOTS:
    QString testMethod(const QString &string) const;

};

class TestComplexMethodService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", "service.complex.prefix.for")
public:
    TestComplexMethodService(QObject *parent = 0);

public Q_SLOTS:
    void testMethod();
};

class TestDelayedResponseService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", "service")
public:
    TestDelayedResponseService(QObject *parent = 0);

Q_SIGNALS:
    void responseResult(bool result);

public Q_SLOTS:
    void delayedResponse();
    void delayedResponseWithClosedSocket();
    QString immediateResponse();

private Q_SLOTS:
    void delayedResponseComplete();
    void delayedResponseWithClosedSocketComplete();

private:
    QJsonRpcServiceRequest m_request;

};

#endif  // TESTSERVICES_H
