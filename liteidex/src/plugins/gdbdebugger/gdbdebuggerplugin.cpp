/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
#include "gdbdebuggeroptionfactory.h"
#include "litedebugapi/litedebugapi.h"
#include "memory.h"

GdbDebuggerPlugin::GdbDebuggerPlugin()
{
}

bool GdbDebuggerPlugin::load(LiteApi::IApplication *app)
{
    LiteApi::IDebuggerManager *manager = LiteApi::getDebugManager(app);
    if (!manager) {
        return false;
    }
    GdbDebugger *debug = new GdbDebugger(app);
    manager->addDebugger(debug);
    manager->setCurrentDebugger(debug);
    app->optionManager()->addFactory(new GdbDebuggerOptionFactory(app,this));
    return true;
}

QStringList GdbDebuggerPlugin::dependPluginList() const
{
    return QStringList() << "plugin/litedebug";
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
