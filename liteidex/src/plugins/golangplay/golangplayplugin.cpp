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
// Module: golangplayplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangplayplugin.h"
#include "goplaybrowser.h"
#include "liteapi/liteobj.h"
#include <QAction>
#include "memory.h"

GolangPlayPlugin::GolangPlayPlugin()
{
}

bool GolangPlayPlugin::load(LiteApi::IApplication *app)
{
    GoplayBrowser *gopaly = new GoplayBrowser(app,this);
    QAction *act = app->editorManager()->registerBrowser(gopaly);
    act->setIcon(QIcon("icon:images/gopher.png"));
    app->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,act);
    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
