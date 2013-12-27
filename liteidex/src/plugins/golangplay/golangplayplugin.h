/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: golangplayplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGPLAYPLUGIN_H
#define GOLANGPLAYPLUGIN_H

#include "golangplay_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class GolangPlayPlugin : public LiteApi::IPlugin
{
public:
    GolangPlayPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<GolangPlayPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.GolangPlayPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/golangplay");
        m_info->setName("GolangPlay");
        m_info->setAnchor("visualfc");
        m_info->setVer("x18");
        m_info->setInfo("Golang Playground");
        m_info->appendDepend("plugin/liteeditor");
    }
};

#endif // GOLANGPLAYPLUGIN_H
