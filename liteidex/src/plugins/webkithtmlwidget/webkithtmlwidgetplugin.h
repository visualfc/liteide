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
// Module: webkithtmlwidgetplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef WEBKITHTMLWIDGETPLUGIN_H
#define WEBKITHTMLWIDGETPLUGIN_H

#include "webkithtmlwidget_global.h"
#include "liteapi/liteapi.h"

class WebKitHtmlWidgetPlugin : public LiteApi::IPlugin
{
public:
    WebKitHtmlWidgetPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<WebKitHtmlWidgetPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
public:
    PluginFactory() {
        m_info->setVer("x15");
        m_info->setId("plugin/WebKitHtmlWidget");
        m_info->setName("WebKitHtmlWidget");
        m_info->setAnchor("visualfc");
        m_info->setInfo("WebKitHtmlWidget Plugin");
    }
};


#endif // WEBKITHTMLWIDGETPLUGIN_H
