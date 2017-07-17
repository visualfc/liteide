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

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#else
#include "json/qjsondocument.h"
#endif

#include "qjsonrpcsocket_p.h"
#include "qjsonrpcservicereply_p.h"
#include "qjsonrpchttpclient.h"

class QJsonRpcHttpReplyPrivate : public QJsonRpcServiceReplyPrivate
{
public:
    QNetworkReply *reply;
};

class QJsonRpcHttpReply : public QJsonRpcServiceReply
{
    Q_OBJECT
public:
    QJsonRpcHttpReply(const QJsonRpcMessage &request,
                      QNetworkReply *reply, QObject *parent = 0)
        : QJsonRpcServiceReply(*new QJsonRpcHttpReplyPrivate, parent)
    {
        Q_D(QJsonRpcHttpReply);
        d->request = request;
        d->reply = reply;
        connect(d->reply, SIGNAL(finished()), this, SLOT(networkReplyFinished()));
        connect(d->reply, SIGNAL(error(QNetworkReply::NetworkError)),
                    this, SLOT(networkReplyError(QNetworkReply::NetworkError)));
    }

    virtual ~QJsonRpcHttpReply() {}

Q_SIGNALS:
    void messageReceived(const QJsonRpcMessage &message);

private Q_SLOTS:
    void networkReplyFinished()
    {
        Q_D(QJsonRpcHttpReply);
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        if (!reply) {
            qJsonRpcDebug() << Q_FUNC_INFO << "invalid reply";
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            // this should be handled by the networkReplyError slot
        } else {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isEmpty() || doc.isNull() || !doc.isObject()) {
                d->response =
                    d->request.createErrorResponse(QJsonRpc::ParseError,
                                                   "unable to process incoming JSON data",
                                                   QString::fromUtf8(data));
            } else {
                qJsonRpcDebug() << "received: " << doc.toJson();
                QJsonRpcMessage response = QJsonRpcMessage::fromObject(doc.object());
                Q_EMIT messageReceived(response);

                if (d->request.type() == QJsonRpcMessage::Request &&
                    d->request.id() != response.id()) {
                    d->response =
                        d->request.createErrorResponse(QJsonRpc::InternalError,
                                                       "invalid response id",
                                                       QString::fromUtf8(data));
                } else {
                    d->response = response;
                }
            }
        }

        reply->deleteLater();
        Q_EMIT finished();
    }

    void networkReplyError(QNetworkReply::NetworkError code)
    {
        Q_D(QJsonRpcHttpReply);
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        if (!reply) {
            qJsonRpcDebug() << Q_FUNC_INFO << "invalid reply";
            return;
        }

        if (code == QNetworkReply::NoError)
            return;

        QJsonRpcMessage response = QJsonRpcMessage::fromJson(reply->readAll());
        if (response.isValid()) {
            d->response = response;
            Q_EMIT messageReceived(response);
        } else {
            d->response = d->request.createErrorResponse(QJsonRpc::InternalError,
                                           QString("error with http request: %1").arg(reply->error()),
                                           reply->errorString());
        }
    }

private:
    Q_DISABLE_COPY(QJsonRpcHttpReply)
    Q_DECLARE_PRIVATE(QJsonRpcHttpReply)

};

class QJsonRpcHttpClientPrivate : public QJsonRpcAbstractSocketPrivate
{
public:
    void initializeNetworkAccessManager(QJsonRpcHttpClient *client) {
        QObject::connect(networkAccessManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                client, SLOT(handleAuthenticationRequired(QNetworkReply*,QAuthenticator*)));
        QObject::connect(networkAccessManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
                client, SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));
    }

    QNetworkReply *writeMessage(const QJsonRpcMessage &message) {
        QNetworkRequest request(endPoint);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Accept", "application/json-rpc");
        if (!sslConfiguration.isNull())
            request.setSslConfiguration(sslConfiguration);

        QByteArray data = message.toJson();
        qJsonRpcDebug() << "sending: " << data;
        return networkAccessManager->post(request, data);
    }

    QUrl endPoint;
    QNetworkAccessManager *networkAccessManager;
    QSslConfiguration sslConfiguration;
};

QJsonRpcHttpClient::QJsonRpcHttpClient(QObject *parent)
    : QJsonRpcAbstractSocket(*new QJsonRpcHttpClientPrivate, parent)
{
    Q_D(QJsonRpcHttpClient);
    d->networkAccessManager = new QNetworkAccessManager(this);
    d->initializeNetworkAccessManager(this);
}

QJsonRpcHttpClient::QJsonRpcHttpClient(QNetworkAccessManager *manager, QObject *parent)
    : QJsonRpcAbstractSocket(*new QJsonRpcHttpClientPrivate, parent)
{
    Q_D(QJsonRpcHttpClient);
    d->networkAccessManager = manager;
    d->initializeNetworkAccessManager(this);
}

QJsonRpcHttpClient::QJsonRpcHttpClient(const QString &endPoint, QObject *parent)
    : QJsonRpcAbstractSocket(*new QJsonRpcHttpClientPrivate, parent)
{
    Q_D(QJsonRpcHttpClient);
    d->endPoint = QUrl::fromUserInput(endPoint);
    d->networkAccessManager = new QNetworkAccessManager(this);
    d->initializeNetworkAccessManager(this);
}

QJsonRpcHttpClient::~QJsonRpcHttpClient()
{
}

bool QJsonRpcHttpClient::isValid() const
{
    Q_D(const QJsonRpcHttpClient);
    return d->networkAccessManager && !d->endPoint.isEmpty() && d->endPoint.isValid();
}

QUrl QJsonRpcHttpClient::endPoint() const
{
    Q_D(const QJsonRpcHttpClient);
    return d->endPoint;
}

void QJsonRpcHttpClient::setEndPoint(const QUrl &endPoint)
{
    Q_D(QJsonRpcHttpClient);
    d->endPoint = endPoint;
}

void QJsonRpcHttpClient::setEndPoint(const QString &endPoint)
{
    Q_D(QJsonRpcHttpClient);
    d->endPoint = QUrl::fromUserInput(endPoint);
}

QNetworkAccessManager *QJsonRpcHttpClient::networkAccessManager()
{
    Q_D(QJsonRpcHttpClient);
    return d->networkAccessManager;
}

QSslConfiguration QJsonRpcHttpClient::sslConfiguration() const
{
    Q_D(const QJsonRpcHttpClient);
    return d->sslConfiguration;
}

void QJsonRpcHttpClient::setSslConfiguration(const QSslConfiguration &sslConfiguration)
{
    Q_D(QJsonRpcHttpClient);
    d->sslConfiguration = sslConfiguration;
}

void QJsonRpcHttpClient::notify(const QJsonRpcMessage &message)
{
    Q_D(QJsonRpcHttpClient);
    if (d->endPoint.isEmpty()) {
        qJsonRpcDebug() << Q_FUNC_INFO << "invalid endpoint specified";
        return;
    }

    QNetworkReply *reply = d->writeMessage(message);
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));

    // NOTE: we might want to connect this to a local slot to track errors
    //       for debugging later?
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), reply, SLOT(deleteLater()));
}

QJsonRpcServiceReply *QJsonRpcHttpClient::sendMessage(const QJsonRpcMessage &message)
{
    Q_D(QJsonRpcHttpClient);
    if (d->endPoint.isEmpty()) {
        qJsonRpcDebug() << Q_FUNC_INFO << "invalid endpoint specified";
        return 0;
    }

    QNetworkReply *reply = d->writeMessage(message);
    QJsonRpcHttpReply *serviceReply = new QJsonRpcHttpReply(message, reply);
    connect(serviceReply, SIGNAL(messageReceived(QJsonRpcMessage)),
                    this, SIGNAL(messageReceived(QJsonRpcMessage)));

    return serviceReply;
}

QJsonRpcMessage QJsonRpcHttpClient::sendMessageBlocking(const QJsonRpcMessage &message, int msecs)
{
    QJsonRpcServiceReply *reply = sendMessage(message);
    QScopedPointer<QJsonRpcServiceReply> replyPtr(reply);

    QEventLoop responseLoop;
    connect(reply, SIGNAL(finished()), &responseLoop, SLOT(quit()));
    QTimer::singleShot(msecs, &responseLoop, SLOT(quit()));
    responseLoop.exec();

    if (!reply->response().isValid())
        return message.createErrorResponse(QJsonRpc::TimeoutError, "request timed out");
    return reply->response();
}

QJsonRpcMessage QJsonRpcHttpClient::invokeRemoteMethodBlocking(const QString &method, int msecs, const QVariant &param1,
                                                               const QVariant &param2, const QVariant &param3,
                                                               const QVariant &param4, const QVariant &param5,
                                                               const QVariant &param6, const QVariant &param7,
                                                               const QVariant &param8, const QVariant &param9,
                                                               const QVariant &param10)
{
    QVariantList params;
    if (param1.isValid()) params.append(param1);
    if (param2.isValid()) params.append(param2);
    if (param3.isValid()) params.append(param3);
    if (param4.isValid()) params.append(param4);
    if (param5.isValid()) params.append(param5);
    if (param6.isValid()) params.append(param6);
    if (param7.isValid()) params.append(param7);
    if (param8.isValid()) params.append(param8);
    if (param9.isValid()) params.append(param9);
    if (param10.isValid()) params.append(param10);

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest(method, QJsonArray::fromVariantList(params));
    return sendMessageBlocking(request, msecs);
}

QJsonRpcMessage QJsonRpcHttpClient::invokeRemoteMethodBlocking(const QString &method, const QVariant &param1,
                                                               const QVariant &param2, const QVariant &param3,
                                                               const QVariant &param4, const QVariant &param5,
                                                               const QVariant &param6, const QVariant &param7,
                                                               const QVariant &param8, const QVariant &param9,
                                                               const QVariant &param10)
{
    Q_D(QJsonRpcHttpClient);

    return invokeRemoteMethodBlocking(method, d->defaultRequestTimeout, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
}

QJsonRpcServiceReply *QJsonRpcHttpClient::invokeRemoteMethod(const QString &method, const QVariant &param1,
                                                             const QVariant &param2, const QVariant &param3,
                                                             const QVariant &param4, const QVariant &param5,
                                                             const QVariant &param6, const QVariant &param7,
                                                             const QVariant &param8, const QVariant &param9,
                                                             const QVariant &param10)
{
    QVariantList params;
    if (param1.isValid()) params.append(param1);
    if (param2.isValid()) params.append(param2);
    if (param3.isValid()) params.append(param3);
    if (param4.isValid()) params.append(param4);
    if (param5.isValid()) params.append(param5);
    if (param6.isValid()) params.append(param6);
    if (param7.isValid()) params.append(param7);
    if (param8.isValid()) params.append(param8);
    if (param9.isValid()) params.append(param9);
    if (param10.isValid()) params.append(param10);

    QJsonRpcMessage request =
        QJsonRpcMessage::createRequest(method, QJsonArray::fromVariantList(params));
    return sendMessage(request);
}

void QJsonRpcHttpClient::handleAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply)
    Q_UNUSED(authenticator)
}

void QJsonRpcHttpClient::handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(errors)
    reply->ignoreSslErrors();
}

#include "qjsonrpchttpclient.moc"
