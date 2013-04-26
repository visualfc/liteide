/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
#include "litedebugoptionfactory.h"
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
}

bool LiteDebugPlugin::load(LiteApi::IApplication *app)
{
    app->optionManager()->addFactory(new LiteDebugOptionFactory(app,this));

    QSplitter *splitter = LiteApi::findExtensionObject<QSplitter*>(app,"LiteApi.QMainWindow.QSplitter");
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

    app->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_viewDebug);

    return true;
}

Q_EXPORT_PLUGIN(PluginFactory)
