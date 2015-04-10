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
// Module: liteenvplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteenvplugin.h"
#include "envmanager.h"
#include "liteenvoptionfactory.h"
#include "memory.h"

LiteEnvPlugin::LiteEnvPlugin()
{
}

bool LiteEnvPlugin::load(LiteApi::IApplication *app)
{
    m_envManager = new EnvManager(this);
    if (!m_envManager->initWithApp(app)) {
        return false;
    }
    app->optionManager()->addFactory(new LiteEnvOptionFactory(app,this));

    return true;
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
