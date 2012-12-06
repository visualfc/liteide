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
// Module: litedebugplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litedebugplugin.h"
#include "litedebug.h"
#include <QMenu>
#include <QLayout>
#include <QAction>
#include <QSplitter>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteDebugPlugin::LiteDebugPlugin()
{
    m_info->setId("plugin/LiteDebug");
    m_info->setName("LiteDebug");
    m_info->setAnchor("visualfc");
    m_info->setVer("x13.1");
    m_info->setInfo("LiteIDE Debug Manager Plugin");
}

QStringList LiteDebugPlugin::dependPluginList() const
{
    return QStringList() << "plugin/litebuild" <<"plugin/litefind";
}

bool LiteDebugPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    QSplitter *splitter = LiteApi::findExtensionObject<QSplitter*>(m_liteApp,"LiteApi.QMainWindow.QSplitter");
    if (!splitter) {
        return false;
    }

    m_liteDebug = new LiteDebug(app,this);
    m_liteDebug->widget()->hide();
    splitter->addWidget(m_liteDebug->widget());

    m_viewDebug = new QAction(tr("Debug Window"),this);
    m_viewDebug->setCheckable(true);
    connect(m_viewDebug,SIGNAL(triggered(bool)),m_liteDebug->widget(),SLOT(setVisible(bool)));
    connect(m_liteDebug,SIGNAL(debugVisible(bool)),m_viewDebug,SLOT(setChecked(bool)));

    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuLastPos,m_viewDebug);

    return true;
}

Q_EXPORT_PLUGIN(PluginFactory)
