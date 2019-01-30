/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: webkithtmlwidgetplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef WEBKITHTMLWIDGETPLUGIN_H
#define WEBKITHTMLWIDGETPLUGIN_H

#include "webkithtmlwidget_global.h"
#include "liteapi/liteapi.h"

class WebKitBrowser;
class WebKitHtmlWidgetPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    WebKitHtmlWidgetPlugin();
    virtual bool load(LiteApi::IApplication *app);
public slots:
    void openHtmlWithWebkit();
protected:
    LiteApi::IApplication *m_liteApp;
    WebKitBrowser *m_browser;
    QAction *m_browserAct;
};

class PluginFactory : public LiteApi::PluginFactoryT<WebKitHtmlWidgetPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.WebKitHtmlWidgetPlugin")
#endif
public:
    PluginFactory() {
        m_info->setVer("X23");
        m_info->setId("plugin/WebKitHtmlWidget");
        m_info->setName("WebKitHtmlWidget");
        m_info->setAuthor("visualfc");
        m_info->setInfo("QtWebKit Support");
    }
};


#endif // WEBKITHTMLWIDGETPLUGIN_H
