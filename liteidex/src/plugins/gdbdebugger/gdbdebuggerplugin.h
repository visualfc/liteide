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
// Module: gdbdebuggerplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GDBDEBUGGERPLUGIN_H
#define GDBDEBUGGERPLUGIN_H

#include "gdbdebugger_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class GdbDebuggerPlugin : public LiteApi::IPlugin
{
public:
    GdbDebuggerPlugin();
    virtual bool load(LiteApi::IApplication *app);
    virtual QStringList dependPluginList() const;
};

class PluginFactory : public LiteApi::PluginFactoryT<GdbDebuggerPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
public:
    PluginFactory()
    {
        m_info->setId("plugin/GdbDebugger");
        m_info->setName("GdbDebugger");
        m_info->setAnchor("visualfc");
        m_info->setVer("x13.1");
        m_info->setInfo("Gdb Debugger");
        m_info->setMustLoad(true);
    }
};

#endif // GDBDEBUGGERPLUGIN_H
