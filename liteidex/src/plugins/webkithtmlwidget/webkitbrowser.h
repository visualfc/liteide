/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: webkitbrowser.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef WEBKITBROWSER_H
#define WEBKITBROWSER_H

#include "liteapi/liteapi.h"
#include <QWidget>
#include <QUrl>

class QWebView;
class QToolBar;
class QLineEdit;
class QProgressBar;
class WebKitBrowser : public LiteApi::IWebKitBrowser
{
    Q_OBJECT
public:
    explicit WebKitBrowser(LiteApi::IApplication *app, QObject *parent = 0);
    ~WebKitBrowser();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    virtual void openUrl(const QUrl &url);
public slots:
    void changeLocation();
    void adjustLocation();
    void loadUrlFinished(bool);
    void linkClicked(QUrl);
    void loadUrl(const QUrl &url);
    void linkHovered(const QString & link, const QString & title, const QString & textContent);
    void statusBarMessage(const QString &msg);
    void loadStarted();
    void loadProgress(int);
    void openHtmlFile();
    void reloadUrl();
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget  *m_widget;
    QToolBar *m_toolBar;
    QLineEdit *m_locationEdit;
    QWebView *m_view;
    QProgressBar *m_progressBar;
};

#endif // WEBKITBROWSER_H
