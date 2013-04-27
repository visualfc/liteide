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
// Module: golangfmtplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangfmtplugin.h"
#include "golangfmt.h"
#include "golangfmtoptionfactory.h"
#include "fileutil/fileutil.h"

#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangFmtPlugin::GolangFmtPlugin()
    : m_fmt(0)
{
}

bool GolangFmtPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;

    app->optionManager()->addFactory(new GolangFmtOptionFactory(app,this));

    m_fmt = new GolangFmt(app,this);
    m_gofmtAct = new QAction(QIcon("icon:golangfmt/images/gofmt.png"),tr("Format Code"),this);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GoFmt");

    actionContext->regAction(m_gofmtAct,"Gofmt","Shift+F7");

    m_goplayAct = new QAction(QIcon("icon:golangfmt/images/gofmt.png"),tr("Format Code"),this);
    actionContext->regAction(m_goplayAct,"Goplayfmt","Shift+F7");

    connect(m_gofmtAct,SIGNAL(triggered()),m_fmt,SLOT(gofmt()));
    connect(m_goplayAct,SIGNAL(triggered()),this,SLOT(goplayFmt()));

    connect(app->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));

    return true;
}

QStringList GolangFmtPlugin::dependPluginList() const{
    return QStringList() << "plugin/liteenv";
}

void GolangFmtPlugin::goplayFmt()
{
    m_fmt->syncfmtEditor(m_playEditor,false,false);
}

void GolangFmtPlugin::appLoaded()
{
    m_playEditor = LiteApi::findExtensionObject<LiteApi::IEditor*>(m_liteApp->extension(),"LiteApi.Goplay.IEditor");
    if (m_playEditor) {
        if (m_playEditor->mimeType() != "text/x-gosrc") {
            return;
        }
        m_playEditor->widget()->addAction(m_goplayAct);
        QMenu *menu = LiteApi::getContextMenu(m_playEditor);
        if (menu) {
            menu->addSeparator();
            menu->addAction(m_goplayAct);            
        }
    }
}

void GolangFmtPlugin::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    if (editor->mimeType() != "text/x-gosrc") {
        return;
    }
    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_gofmtAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_gofmtAct);
    }
}

Q_EXPORT_PLUGIN(PluginFactory)
