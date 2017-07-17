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
#ifndef QJSONRPCHTTPCLIENT_H
#define QJSONRPCHTTPCLIENT_H

#include <QObject>
#include <QNetworkReply>
#include <QSslConfiguration>

#include "qjsonrpcglobal.h"
#include "qjsonrpcmessage.h"
#include "qjsonrpcsocket.h"
#include "qjsonrpcservicereply.h"

class QNetwokReply;
class QAuthenticator;
class QSslError;
class QNetworkAccessManager;
class QJsonRpcHttpClientPrivate;
class QJSONRPC_EXPORT QJsonRpcHttpClient : public QJsonRpcAbstractSocket
{
    Q_OBJECT
public:
    QJsonRpcHttpClient(QObject *parent = 0);
    QJsonRpcHttpClient(const QString &endPoint, QObject *parent = 0);
    QJsonRpcHttpClient(QNetworkAccessManager *manager, QObject *parent = 0);
    ~QJsonRpcHttpClient();

    virtual bool isValid() const;

    QUrl endPoint() const;
    void setEndPoint(const QUrl &endPoint);
    void setEndPoint(const QString &endPoint);

    QNetworkAccessManager *networkAccessManager();

    QSslConfiguration sslConfiguration() const;
    void setSslConfiguration(const QSslConfiguration &sslConfiguration);

public Q_SLOTS:
    virtual void notify(const QJsonRpcMessage &message);
    virtual QJsonRpcMessage sendMessageBlocking(const QJsonRpcMessage &message, int msecs = DEFAULT_MSECS_REQUEST_TIMEOUT);
    virtual QJsonRpcServiceReply *sendMessage(const QJsonRpcMessage &message);

    virtual QJsonRpcMessage invokeRemoteMethodBlocking(const QString &method, int msecs, const QVariant &arg1 = QVariant(),
                                               const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                               const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                               const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                               const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                               const QVariant &arg10 = QVariant());
    virtual QJsonRpcMessage invokeRemoteMethodBlocking(const QString &method, const QVariant &arg1 = QVariant(),
                                               const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                               const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                               const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                               const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                               const QVariant &arg10 = QVariant());
    virtual QJsonRpcServiceReply *invokeRemoteMethod(const QString &method, const QVariant &arg1 = QVariant(),
                                             const QVariant &arg2 = QVariant(), const QVariant &arg3 = QVariant(),
                                             const QVariant &arg4 = QVariant(), const QVariant &arg5 = QVariant(),
                                             const QVariant &arg6 = QVariant(), const QVariant &arg7 = QVariant(),
                                             const QVariant &arg8 = QVariant(), const QVariant &arg9 = QVariant(),
                                             const QVariant &arg10 = QVariant());

protected Q_SLOTS:
    virtual void handleAuthenticationRequired(QNetworkReply *reply, QAuthenticator * authenticator);
    virtual void handleSslErrors( QNetworkReply * reply, const QList<QSslError> &errors);

private:
    Q_DISABLE_COPY(QJsonRpcHttpClient)
    Q_DECLARE_PRIVATE(QJsonRpcHttpClient)

};

#endif
