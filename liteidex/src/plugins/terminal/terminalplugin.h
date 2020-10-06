/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2020 LiteIDE. All rights reserved.
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
// Module: terminalplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TERMINALPLUGIN_H
#define TERMINALPLUGIN_H

#include "terminal_global.h"
#include "liteapi/liteapi.h"

class TerminalPlugin : public LiteApi::IPlugin
{
public:
    TerminalPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<TerminalPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.TerminalPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/Terminal");
        m_info->setVer("X37.3");
        m_info->setName("Terminal");
        m_info->setAuthor("visualfc");
        m_info->setInfo("Terminal");
        m_info->appendDepend("plugin/liteenv");
    }
};


#endif // TERMINALPLUGIN_H
