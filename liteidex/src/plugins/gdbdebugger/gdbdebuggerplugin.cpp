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
// Module: gdbdebuggerplugin.cpp
// Creator: visualfc <visualfc@gmail.com>



#include "gdbdebuggerplugin.h"
#include "gdbdebugger.h"
#include "litedebugapi/litedebugapi.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GdbDebuggerPlugin::GdbDebuggerPlugin()
{
    m_info->setId("plugin/GdbDebugger");
    m_info->setName("GdbDebugger");
    m_info->setAnchor("visualfc");
    m_info->setVer("x13.1");
    m_info->setInfo("Gdb Debugger Plugin");
}

bool GdbDebuggerPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    LiteApi::IDebuggerManager *manager = LiteApi::findExtensionObject<LiteApi::IDebuggerManager*>(app,"LiteApi.IDebugManager");
    if (!manager) {
        return false;
    }
    GdbDebugeer *debug = new GdbDebugeer(app);
    manager->addDebugger(debug);
    manager->setCurrentDebugger(debug);

    return true;
}

QStringList GdbDebuggerPlugin::dependPluginList() const
{
    return QStringList() << "plugin/litedebug";
}

Q_EXPORT_PLUGIN(PluginFactory)
