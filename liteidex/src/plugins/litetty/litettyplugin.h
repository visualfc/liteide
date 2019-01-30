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
// Module: litettyplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITETTYPLUGIN_H
#define LITETTYPLUGIN_H

#include "litetty_global.h"
#include "liteapi/liteapi.h"

class LiteTtyPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    LiteTtyPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<LiteTtyPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.LiteTtyPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/LiteTty");
        m_info->setVer("X19");
        m_info->setName("LiteTty");
        m_info->setAuthor("visualfc");
        m_info->setInfo("LiteIDE tty Util");
        //m_info->appendDepend("plugin/liteenv");
    }
};


#endif // LITETTYPLUGIN_H
