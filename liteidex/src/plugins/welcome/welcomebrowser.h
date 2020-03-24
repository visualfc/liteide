/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: welcomebrowser.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef WELCOMEBROWSER_H
#define WELCOMEBROWSER_H

#include "liteapi/liteapi.h"
#include "extension/extension.h"
#include "documentbrowser/documentbrowser.h"
#include <QUrl>

namespace Ui {
    class WelcomeWidget;
}

class WelcomeBrowser : public LiteApi::IBrowserEditor
{
    Q_OBJECT
public:
    WelcomeBrowser(LiteApi::IApplication *app, QObject *parent);
    virtual ~WelcomeBrowser();
    virtual LiteApi::IExtension *extension();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
public slots:
    void openUrl(const QUrl &url);
    void highlightedUrl(const QUrl &url);
    void loadData();
    void openFolder();
protected:
    LiteApi::IApplication *m_liteApp;
    Extension   *m_extension;
    QWidget *m_widget;
    DocumentBrowser *m_browser;
    QString m_templateData;
};

#endif // WELCOMEBROWSER_H
