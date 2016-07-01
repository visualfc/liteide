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
// Module: fakevimeditplugin.h
// Creator: jsuppe <jon.suppe@gmail.com>

#ifndef FAKEVIMEDITPLUGIN_H
#define FAKEVIMEDITPLUGIN_H

#include "liteapi/liteapi.h"

class FakeVimEditPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    FakeVimEditPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<FakeVimEditPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.FakeVimEditPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/FakeVimEdit");
        m_info->setVer("X30");
        m_info->setName("FakeVimEdit");
        m_info->setAuthor("jsuppe");
        m_info->setInfo("Fake Vim Edit Support");
        m_info->appendDepend("plugin/liteeditor");
    }
};


#endif // FAKEVIMEDITPLUGIN_H
