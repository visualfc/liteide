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
#include <QCoreApplication>
#include <QAuthenticator>
#include <QStringList>
#include <QDebug>

#include "qjsonrpchttpclient.h"

class HttpClient : public QJsonRpcHttpClient
{
    Q_OBJECT
public:
    HttpClient(const QString &endpoint, QObject *parent = 0)
        : QJsonRpcHttpClient(endpoint, parent)
    {
        // defaults added for my local test server
        m_username = "bitcoinrpc";
        m_password = "232fb3276bbb7437d265298ea48bdc46";
    }

    void setUsername(const QString &username) {
        m_username = username;
    }

    void setPassword(const QString &password) {
        m_password = password;
    }

private Q_SLOTS:
    virtual void handleAuthenticationRequired(QNetworkReply *reply, QAuthenticator * authenticator)
    {
        Q_UNUSED(reply)
        authenticator->setUser(m_username);
        authenticator->setPassword(m_password);
    }

private:
    QString m_username;
    QString m_password;

};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    if (app.arguments().size() < 2) {
        qDebug() << "usage: " << argv[0] << "[-u username] [-p password] <command> <arguments>";
        return -1;
    }

    HttpClient client("http://127.0.0.1:8332");
    if (app.arguments().contains("-u")) {
        int idx = app.arguments().indexOf("-u");
        app.arguments().removeAt(idx);
        client.setUsername(app.arguments().takeAt(idx));
    }

    if (app.arguments().contains("-p")) {
        int idx = app.arguments().indexOf("-p");
        app.arguments().removeAt(idx);
        client.setPassword(app.arguments().takeAt(idx));
    }

    QJsonRpcMessage message = QJsonRpcMessage::createRequest(app.arguments().at(1));
    QJsonRpcMessage response = client.sendMessageBlocking(message);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug() << response.errorData();
        return -1;
    }

    qDebug() << response.toJson();
}

#include "main.moc"
