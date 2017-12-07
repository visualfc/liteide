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
// Module: quickopenplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef QUICKOPENPLUGIN_H
#define QUICKOPENPLUGIN_H

#include "quickopen_global.h"
#include "liteapi/liteapi.h"

class QuickOpenPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    QuickOpenPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<QuickOpenPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.QuickOpenPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/QuickOpen");
        m_info->setVer("X33.1");
        m_info->setName("QuickOpen");
        m_info->setAuthor("visualfc");
        m_info->setInfo("QuickOpen");
        m_info->setMustLoad(true);
        //m_info->appendDepend("plugin/liteenv");
    }
};


#endif // QUICKOPENPLUGIN_H
