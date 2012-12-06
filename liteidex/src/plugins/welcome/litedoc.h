/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: litedoc.h
// Creator: visualfc <visualfc@gmail.com>



#ifndef LITEDOC_H
#define LITEDOC_H

#include "liteapi/liteapi.h"
#include "extension/extension.h"
#include <QUrl>

class DocumentBrowser;

namespace LiteApi {

class ILiteDoc : public IObject
{
    Q_OBJECT
public:
    ILiteDoc(QObject *parent) : IObject(parent) {}
public slots:
    virtual void openUrl(const QUrl &url) = 0;
    virtual void activeBrowser() = 0;
};

}

class LiteDoc : public LiteApi::ILiteDoc
{
    Q_OBJECT
public:
    explicit LiteDoc(LiteApi::IApplication *app, QObject *parent = 0);
    virtual void activeBrowser();
public slots:
    virtual void openUrl(const QUrl &url);
    void highlighted(const QUrl &url);
protected:
    void openUrlFile(const QUrl &url);
    void updateTextDoc(const QUrl &url, const QByteArray &ba, const QString &header);
    void updateHtmlDoc(const QUrl &url, const QByteArray &ba, const QString &header = QString(), bool toNav = true);
    QUrl parserUrl(const QUrl &url);
    LiteApi::IApplication *m_liteApp;
    DocumentBrowser *m_docBrowser;
    QAction *m_browserAct;
    QString m_templateData;
    QUrl m_lastUrl;
};

#endif // LITEDOC_H
