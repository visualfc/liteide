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
// Module: golangplayplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-3-8
// $Id: golangplayplugin.cpp,v 1.0 2012-3-8 visualfc Exp $

#include "golangplayplugin.h"
#include "goplaybrowser.h"
#include "liteapi/liteobj.h"
#include <QAction>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangPlayPlugin::GolangPlayPlugin()
{
    m_info->setId("plugin/golangplay");
    m_info->setName("GolangPlay");
    m_info->setAnchor("visualfc");
    m_info->setVer("x13.1");
    m_info->setInfo("GolangPlay Plugin");
}

bool GolangPlayPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    GoplayBrowser *gopaly = new GoplayBrowser(app,this);
    QAction *act = m_liteApp->editorManager()->registerBrowser(gopaly);
    act->setIcon(QIcon("icon:images/gopher.png"));
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,act);
    return true;
}

QStringList GolangPlayPlugin::dependPluginList() const
{
    return QStringList() << "plugin/liteeditor";
}

Q_EXPORT_PLUGIN(PluginFactory)
