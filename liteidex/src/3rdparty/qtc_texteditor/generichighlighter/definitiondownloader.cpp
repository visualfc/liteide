/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "definitiondownloader.h"

#include <QtCore/QLatin1Char>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QScopedPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

using namespace TextEditor;
using namespace Internal;

DefinitionDownloader::DefinitionDownloader(const QUrl &url, const QString &localPath) :
    m_url(url), m_localPath(localPath), m_status(Unknown)
{}

void DefinitionDownloader::run()
{
    QNetworkAccessManager manager;

    int currentAttempt = 0;
    const int maxAttempts = 5;
    while (currentAttempt < maxAttempts) {
        QScopedPointer<QNetworkReply> reply(getData(&manager));
        if (reply->error() != QNetworkReply::NoError) {
            m_status = NetworkError;
            return;
        }

        ++currentAttempt;
        QVariant variant = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (variant.isValid() && currentAttempt < maxAttempts) {
            m_url = variant.toUrl();
        } else if (!variant.isValid()) {
            saveData(reply.data());
            return;
        }
    }
}

QNetworkReply *DefinitionDownloader::getData(QNetworkAccessManager *manager) const
{
    QNetworkRequest request(m_url);
    QNetworkReply *reply = manager->get(request);

    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    return reply;
}

void DefinitionDownloader::saveData(QNetworkReply *reply)
{
    const QString &urlPath = m_url.path();
    const QString &fileName =
        urlPath.right(urlPath.length() - urlPath.lastIndexOf(QLatin1Char('/')) - 1);
    QFile file(m_localPath + fileName);
    if (file.open(QIODevice::Text | QIODevice::WriteOnly)) {
        file.write(reply->readAll());
        file.close();
        m_status = Ok;
    } else {
        m_status = WriteError;
    }
}

DefinitionDownloader::Status DefinitionDownloader::status() const
{ return m_status; }
