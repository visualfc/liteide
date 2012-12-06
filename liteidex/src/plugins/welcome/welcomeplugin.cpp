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
// Module: welcomeplugin.cpp
// Creator: visualfc <visualfc@gmail.com>



#include "welcomeplugin.h"
#include "../../liteapp/liteapp_global.h"
#include <QAction>
#include <QMenu>
#include <QToolBar>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


WelcomePlugin::WelcomePlugin() : m_welcome(0)
{
    m_info->setId("plugin/welcome");
    m_info->setName("Welcome");
    m_info->setAnchor("visualfc");
    m_info->setVer("x13.2");
    m_info->setInfo("LiteIDE Welcome Plugin");
}

void WelcomePlugin::home()
{
    m_liteApp->editorManager()->activeBrowser(m_welcome);
}

bool WelcomePlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    m_welcome = new WelcomeBrowser(app,this);
    m_welcomeAct = m_liteApp->editorManager()->registerBrowser(m_welcome);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_welcomeAct);

    if (m_liteApp->settings()->value(LITEAPP_WELCOMEPAGEVISIBLE,true).toBool()) {
        m_welcomeAct->toggle();
    }

    QToolBar *toolBar = m_liteApp->actionManager()->loadToolBar("toolbar/std");
    if (toolBar) {
        toolBar->addSeparator();
        m_homeAct = new QAction(QIcon("icon:welcome/images/home.png"),tr("Home"),this);
        m_homeAct->setShortcut(QKeySequence("Ctrl+Alt+H"));
        connect(m_homeAct,SIGNAL(triggered()),this,SLOT(home()));
        toolBar->addAction(m_homeAct);
    }

    m_liteDoc = new LiteDoc(m_liteApp,this);

    return true;
}

Q_EXPORT_PLUGIN(PluginFactory)
