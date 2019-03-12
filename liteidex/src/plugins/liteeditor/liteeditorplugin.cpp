/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: liteeditorplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditorplugin.h"
#include "liteeditorfilefactory.h"
#include "liteeditoroptionfactory.h"
#include "liteeditor_global.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteEditorPlugin::LiteEditorPlugin()
{
}

bool LiteEditorPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;
    QString style = app->settings()->value(EDITOR_STYLE,"default.xml").toString();
    if (!style.isEmpty()) {
        QString styleFileName = app->resourcePath()+"/liteeditor/color/"+style;
        if (!QFileInfo(styleFileName).exists()) {
             styleFileName = app->resourcePath()+"/liteeditor/color/default.xml";
        }
        app->editorManager()->loadColorStyleScheme(styleFileName);
    }
    LiteEditorFileFactory *factory = new LiteEditorFileFactory(app,this);
    app->editorManager()->addFactory(factory);

    foreach(QString mime, app->editorManager()->mimeTypeList()) {
        if (mime.startsWith("text/") || mime.startsWith("application/")) {
            LiteApi::IMimeType *imt = app->mimeTypeManager()->findMimeType(mime);
            if (imt) {
                QString custom = app->settings()->value(EDITOR_CUSTOMEXTENSION+mime,"").toString();
                if (!custom.isEmpty()) {
                    imt->setCustomPatterns(custom.split(";"));
                }
            }
        }
    }

    app->optionManager()->addFactory(new LiteEditorOptionFactory(app,this));

    m_toolBarAct = new QAction(tr("Edit ToolBar"));
    m_toolBarAct->setCheckable(true);
    m_toolBarAct->setChecked(m_liteApp->settings()->value(EDITOR_TOOLBAR_VISIBLE,true).toBool());
    app->actionManager()->insertViewMenu(LiteApi::ViewMenuToolBarPos,m_toolBarAct);
    connect(m_toolBarAct,SIGNAL(triggered(bool)),this,SLOT(editorToolBarVisibleChanged(bool)));

    m_navBarAct = new QAction(tr("Edit Navigation Bar"));
    m_navBarAct->setCheckable(true);
    m_navBarAct->setChecked(m_liteApp->settings()->value(EDITOR_NAVBAR_VISIBLE,true).toBool());
    app->actionManager()->insertViewMenu(LiteApi::ViewMenuToolBarPos,m_navBarAct);
    connect(m_navBarAct,SIGNAL(triggered(bool)),this,SLOT(editorNavigateVisibleChanged(bool)));

    return true;
}

void LiteEditorPlugin::editorToolBarVisibleChanged(bool b)
{
    m_liteApp->settings()->setValue(EDITOR_TOOLBAR_VISIBLE,b);
    m_liteApp->sendBroadcast("liteeditor",EDITOR_TOOLBAR_VISIBLE,b);
}

void LiteEditorPlugin::editorNavigateVisibleChanged(bool b)
{
    m_liteApp->settings()->setValue(EDITOR_NAVBAR_VISIBLE,b);
    m_liteApp->sendBroadcast("liteeditor",EDITOR_NAVBAR_VISIBLE,b);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
