/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: actionmanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: actionmanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "actionmanager.h"

#include <QMenuBar>
#include <QToolBar>
#include <QAction>
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


ActionManager::ActionManager(QObject *parent) :
    IActionManager(parent),
    m_viewMenu(0),
    m_baseToolBarAct(0),
    m_baseBrowserAct(0)
{
}

bool ActionManager::initWithApp(IApplication *app)
{
    if (!IActionManager::initWithApp(app)) {
        return false;
    }

    insertMenu("menu/file",tr("&File"));
    insertMenu("menu/recent",tr("&Recent"));
    insertMenu("menu/edit",tr("&Edit"));
    insertMenu("menu/find",tr("F&ind"));
    m_viewMenu = insertMenu("menu/view",tr("&View"));
    m_viewToolMenu = m_viewMenu->addMenu(tr("Tool Windows"));
    m_viewMenu->addSeparator();
    m_baseToolBarAct = m_viewMenu->addSeparator();
    m_baseBrowserAct = m_viewMenu->addSeparator();
    m_viewMenu->addSeparator();
    insertMenu("menu/help",tr("&Help"));

    QToolBar *stdToolBar = insertToolBar("toolbar/std",tr("Standard ToolBar"));

    insertViewMenu(LiteApi::ViewMenuToolBarPos,stdToolBar->toggleViewAction());

    return true;
}

QMenu *ActionManager::insertMenu(const QString &id, const QString &title, const QString &idBefore)
{    
    QMenu *menu = m_idMenuMap.value(id);
    if (menu) {
        return menu;
    }
    menu = new QMenu(title,m_liteApp->mainWindow()->menuBar());
    menu->setObjectName(id);
    QMenu *m = 0;
    if (!idBefore.isEmpty()) {
        m = m_idMenuMap.value(idBefore);
    }
    if (m) {
        m_liteApp->mainWindow()->menuBar()->insertMenu(m->menuAction(),menu);
    } else {
        m_liteApp->mainWindow()->menuBar()->addAction(menu->menuAction());
    }
    m_idMenuMap.insert(id,menu);
    return menu;
}

void ActionManager::removeMenu(QMenu *menu)
{
    if (!menu) {
        return;
    }
    QString id = m_idMenuMap.key(menu);
    if (!id.isEmpty()) {
        m_idMenuMap.remove(id);
    }
    m_liteApp->mainWindow()->menuBar()->removeAction(menu->menuAction());
}

QList<QString> ActionManager::menuList() const
{
    return m_idMenuMap.keys();
}

QMenu *ActionManager::loadMenu(const QString &id)
{
    return m_idMenuMap.value(id);
}

QToolBar *ActionManager::insertToolBar(const QString &id, const QString &title, const QString &idBefore)
{
    QToolBar *toolBar = m_idToolBarMap.value(id);
    if (toolBar) {
        return toolBar;
    }
    toolBar = new QToolBar(title, m_liteApp->mainWindow());
    toolBar->setObjectName(id);
    toolBar->setIconSize(QSize(24,24));
    QToolBar *m = 0;
    if (!idBefore.isEmpty()) {
        m = m_idToolBarMap.value(idBefore);
    }
    if (m) {
        m_liteApp->mainWindow()->insertToolBar(m,toolBar);
    } else {
        m_liteApp->mainWindow()->addToolBar(toolBar);
    }
    m_idToolBarMap.insert(id,toolBar);

    return toolBar;
}

QToolBar *ActionManager::loadToolBar(const QString &id)
{
    return m_idToolBarMap.value(id);
}

QList<QString> ActionManager::toolBarList() const
{
    return m_idToolBarMap.keys();
}

void ActionManager::removeToolBar(QToolBar* toolBar)
{
    if (!toolBar) {
        return;
    }
    QString id = m_idToolBarMap.key(toolBar);
    if (!id.isEmpty()) {
        m_idToolBarMap.remove(id);
    }
    m_liteApp->mainWindow()->removeToolBar(toolBar);
}

void ActionManager::insertViewMenu(VIEWMENU_ACTION_POS pos, QAction *act)
{
    if (pos == ViewMenuToolBarPos) {
        m_viewMenu->insertAction(m_baseToolBarAct,act);
    } else if (pos == ViewMenuToolWindowPos) {
        m_viewToolMenu->addAction(act);
    } else if(pos == ViewMenuBrowserPos){
        m_viewMenu->insertAction(m_baseBrowserAct,act);
    } else {
        m_viewMenu->addAction(act);
    }
}
