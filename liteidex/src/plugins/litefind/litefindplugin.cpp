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
// Module: litefindplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litefindplugin.h"
#include "findeditor.h"
#include "replaceeditor.h"
#include "filesearch.h"

#include <QMenu>
#include <QAction>
#include <QLayout>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteFindPlugin::LiteFindPlugin()
    : m_findEditor(0),
      m_replaceEditor(0),
      m_fileSearch(0)
{
}

LiteFindPlugin::~LiteFindPlugin()
{
    if (m_fileSearch) {
        delete m_fileSearch;
    }
}

bool LiteFindPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;

    QLayout *layout = m_liteApp->editorManager()->widget()->layout();
    if (!layout) {
        return false;
    }

    QMenu *menu = m_liteApp->actionManager()->insertMenu("menu/find",tr("F&ind"),"menu/help");
    if (!menu) {
        return false;
    }

    m_findEditor = new FindEditor(m_liteApp,this);
    m_findEditor->widget()->hide();

    m_replaceEditor = new ReplaceEditor(m_liteApp,this);
    m_replaceEditor->widget()->hide();

    m_fileSearch = new FileSearch(m_liteApp,this);

    layout->addWidget(m_findEditor->widget());
    layout->addWidget(m_replaceEditor->widget());

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"Find");

    m_findAct = new QAction(tr("Find"),this);
    actionContext->regAction(m_findAct,"Find",QKeySequence::Find);

    m_findNextAct = new QAction(tr("Find Next"),this);
    actionContext->regAction(m_findNextAct,"FindNext",QKeySequence::FindNext);

    m_findPrevAct = new QAction(tr("Find Previous"),this);
    actionContext->regAction(m_findPrevAct,"FindPrevious",QKeySequence::FindPrevious);

    m_replaceAct = new QAction(tr("Replace"),this);
    actionContext->regAction(m_replaceAct,"Replace",QKeySequence::Replace);

    m_fileSearchAct = new QAction(tr("File Search"),this);
    actionContext->regAction(m_fileSearchAct,"FileSearch","Ctrl+Shift+F");

    menu->addAction(m_findAct);
    menu->addAction(m_findNextAct);
    menu->addAction(m_findPrevAct);
    menu->addSeparator();
    menu->addAction(m_replaceAct);
    menu->addSeparator();
    menu->addAction(m_fileSearchAct);

    connect(m_findAct,SIGNAL(triggered()),this,SLOT(find()));
    connect(m_findNextAct,SIGNAL(triggered()),m_findEditor,SLOT(findNext()));
    connect(m_findPrevAct,SIGNAL(triggered()),m_findEditor,SLOT(findPrev()));
    connect(m_replaceAct,SIGNAL(triggered()),this,SLOT(replace()));
    connect(m_findEditor,SIGNAL(hideFind()),this,SLOT(hideFind()));
    connect(m_replaceEditor,SIGNAL(hideReplace()),this,SLOT(hideReplace()));
    connect(m_findEditor,SIGNAL(swithReplace()),this,SLOT(switchReplace()));
    connect(m_fileSearchAct,SIGNAL(triggered()),this,SLOT(fileSearch()));

    return true;
}

void LiteFindPlugin::hideFind()
{
    m_findEditor->setVisible(false);
}

void LiteFindPlugin::hideReplace()
{
    m_replaceEditor->setVisible(false);
}

void LiteFindPlugin::switchReplace()
{
    m_findEditor->setVisible(false);
    m_replaceEditor->setVisible(true);
}

void LiteFindPlugin::find()
{
    m_replaceEditor->setVisible(false);
    m_findEditor->setVisible(true);
}

void LiteFindPlugin::replace()
{
    m_findEditor->setVisible(false);
    m_replaceEditor->setVisible(true);
}

void LiteFindPlugin::fileSearch()
{
    if (m_fileSearch == 0) {
        m_fileSearch = new FileSearch(m_liteApp,this);
    }
    m_fileSearch->setVisible(true);
}

Q_EXPORT_PLUGIN(PluginFactory)
