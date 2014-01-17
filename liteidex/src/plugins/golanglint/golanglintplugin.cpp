/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: golanglint.cpp
// Creator: Hai Thanh Nguyen <phaikawl@gmail.com>

#include "golanglintplugin.h"
#include "golanglint.h"
#include "fileutil/fileutil.h"
#include "golanglintoptionfactory.h"

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

GolangLintPlugin::GolangLintPlugin()
    : m_lint(0)
{
}

bool GolangLintPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;

    app->optionManager()->addFactory(new GolangLintOptionFactory(app,this));

    m_lint = new GolangLint(app,this);
    m_golintAct = new QAction(QIcon("icon:golanglint/images/golint.png"),tr("Lint Code"),this);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GoLint");

    actionContext->regAction(m_golintAct,"GoLint","Ctrl+Shift+F7");

    m_goplayAct = new QAction(QIcon("icon:golanglint/images/golint.png"),tr("Lint Code"),this);
    actionContext->regAction(m_goplayAct,"GoplayLint","Ctrl+Shift+F7");

    connect(m_golintAct,SIGNAL(triggered()),m_lint,SLOT(golint()));
    connect(m_goplayAct,SIGNAL(triggered()),this,SLOT(goplayLint()));

    connect(app->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));

    return true;
}

QStringList GolangLintPlugin::dependPluginList() const{
    return QStringList() << "plugin/liteenv";
}

void GolangLintPlugin::goplayLint()
{
    m_lint->syncLintEditor(m_playEditor,false,false,2000);
    /*
    QPlainTextEdit* ed=LiteApi::getPlainTextEdit(m_playEditor);
    ed->document()->setModified(false);
    m_lint->lintEditor(m_playEditor,false);
    */
}

void GolangLintPlugin::appLoaded()
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

void GolangLintPlugin::editorCreated(LiteApi::IEditor *editor)
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
        menu->addAction(m_golintAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_golintAct);
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
