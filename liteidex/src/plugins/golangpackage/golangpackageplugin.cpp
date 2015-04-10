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
// Module: golangpackageplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangpackageplugin.h"
#include "packagebrowser.h"
#include "packageproject.h"
#include "packageprojectfactory.h"
#include <QAction>
#include "memory.h"

GolangPackagePlugin::GolangPackagePlugin()
{
}

bool GolangPackagePlugin::load(LiteApi::IApplication *app)
{
    new PackageBrowser(app,this);
    //PackageProjectFactory *factory = new PackageProjectFactory(app,this);
    //m_liteApp->projectManager()->addFactory(factory);

    return true;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
