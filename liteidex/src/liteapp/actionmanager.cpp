/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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

#include "actionmanager.h"
#include "liteapp_global.h"
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

ActionManager::~ActionManager()
{
    QMapIterator<QObject*,IActionContext*> it(m_objContextMap);
    while(it.hasNext()) {
        it.next();
        delete it.value();
    }
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
    m_viewMenu->addSeparator();
    m_baseToolBarAct = m_viewMenu->addSeparator();
    m_baseBrowserAct = m_viewMenu->addSeparator();
    m_viewMenu->addSeparator();
    insertMenu("menu/help",tr("&Help"));

    QToolBar *stdToolBar = insertToolBar("toolbar/std",tr("Standard Toolbar"));

    insertViewMenu(LiteApi::ViewMenuToolBarPos,stdToolBar->toggleViewAction());

    return true;
}

QMenu *ActionManager::insertMenu(const QString &id, const QString &title, const QString &idBefore)
{    
    QMenu *menu = m_idMenuMap.value(id);
    if (menu) {
        return menu;
    }
    menu = new QMenu(title,m_liteApp->mainWindow());
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
    toolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));

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

void ActionManager::insertToolBar(QToolBar *toolBar, const QString &idBefore)
{
    QString id = toolBar->objectName();

    toolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));

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
    } else if(pos == ViewMenuBrowserPos){
        m_viewMenu->insertAction(m_baseBrowserAct,act);
    } else {
        m_viewMenu->addAction(act);
    }
}

void ActionManager::setViewMenuSeparator(const QString &sepid, bool group)
{
    if (sepid.isEmpty()) {
        return;
    }
    if (m_idSeperatorMap.contains(sepid)) {
        return;
    }
    if (group) {
        m_viewMenu->addSeparator();
    }
    QAction *sep = m_viewMenu->addSeparator();
    m_idSeperatorMap.insert(sepid,sep);
}

void ActionManager::insertViewMenuAction(QAction *act, const QString &sepid)
{
    QAction *sep = m_idSeperatorMap[sepid];
    m_viewMenu->insertAction(sep,act);
}

IActionContext *ActionManager::getActionContext(QObject *obj, const QString &name)
{
    IActionContext *context = m_objContextMap.value(obj);
    if (!context) {
        context = new ActionContext(m_liteApp,name);
        connect(obj,SIGNAL(destroyed(QObject*)),this,SLOT(removeActionContext(QObject*)));
        m_objContextMap.insert(obj,context);
    }
    return context;
}

QStringList ActionManager::actionKeys() const
{
    QStringList keys;
    QMapIterator<QObject*,IActionContext*> it(m_objContextMap);
    while(it.hasNext()) {
        it.next();
        keys.append(it.value()->actionKeys());
    }
    keys.removeDuplicates();
    return keys;
}

ActionInfo *ActionManager::actionInfo(const QString &id) const
{
    QMapIterator<QObject*,IActionContext*> it(m_objContextMap);
    while (it.hasNext()) {
        it.next();
        ActionInfo *info = it.value()->actionInfo(id);
        if (info) {
            return info;
        }
    }
    return 0;
}

QList<QKeySequence> ActionManager::toShortcuts(const QString &ks)
{
    QString keyseq = ks;
    QString part;
    int p = 0, diff = 0;

    QList<QKeySequence> keys;
    while (keyseq.length()) {
        // We MUST use something to separate each sequence, and space
        // does not cut it, since some of the key names have space
        // in them.. (Let's hope no one translate with a comma in it:)
        p = keyseq.indexOf(QLatin1Char(';'));
        if (-1 != p) {
            if (p == keyseq.count() - 1) { // Last comma 'Ctrl+;'
                p = -1;
            } else {
                if (QLatin1Char(';') == keyseq.at(p+1)) // e.g. 'Ctrl+;; Shift+;;'
                    p++;
                if (QLatin1Char(' ') == keyseq.at(p+1)) { // Space after comma
                    diff = 1;
                    p++;
                } else {
                    diff = 0;
                }
            }
        }
        part = keyseq.left(-1 == p ? keyseq.length() : p - diff);
        keyseq = keyseq.right(-1 == p ? 0 : keyseq.length() - (p + 1));
        QKeySequence key(part);
        if (!key.isEmpty()) {
            keys.append(key);
        }
    }
    return keys;
}

QString ActionManager::formatShortcutsString(const QString &ks)
{
    QStringList ksList;
    foreach(QKeySequence k, toShortcuts(ks)) {
        ksList.append(k.toString());
    }
    return ksList.join("; ");
}

QString ActionManager::formatShortcutsNativeString(const QString &ks)
{
    QStringList ksList;
    foreach(QKeySequence k, toShortcuts(ks)) {
        ksList.append(k.toString(QKeySequence::NativeText));
    }
    return ksList.join("; ");

}

void ActionManager::setActionShourtcuts(const QString &id, const QString &shortcuts)
{
    QMapIterator<QObject*,IActionContext*> it(m_objContextMap);
    while(it.hasNext()) {
        it.next();
        it.value()->setActionShortcuts(id,shortcuts);
    }
}

QStringList ActionManager::actionContextNameList() const
{
    QStringList nameList;
    QMapIterator<QObject*,IActionContext*> it(m_objContextMap);
    while(it.hasNext()) {
        it.next();
        nameList.append(it.value()->contextName());
    }
    nameList.removeDuplicates();
    return nameList;
}

IActionContext *ActionManager::actionContextForName(const QString &name)
{
    QMapIterator<QObject*,IActionContext*> it(m_objContextMap);
    while(it.hasNext()) {
        it.next();
        if (it.value()->contextName().compare(name,Qt::CaseInsensitive) == 0) {
            return it.value();
        }
    }
    return 0;
}

void ActionManager::removeActionContext(QObject *obj)
{
    QMutableMapIterator<QObject*,IActionContext*> it(m_objContextMap);
    while (it.hasNext()) {
        it.next();
        if (it.key() == obj) {
            delete it.value();
            it.remove();
            break;
        }
    }
}

ActionContext::ActionContext(IApplication *app, const QString &name)
    : m_liteApp(app), m_name(name)
{
}

QString ActionContext::contextName() const
{
    return m_name;
}

ActionContext::~ActionContext()
{
    QMapIterator<QString,ActionInfo*> it(m_actionInfoMap);
    while(it.hasNext()) {
        it.next();
        ActionInfo* info = it.value();
        delete info;
    }
    m_actionInfoMap.clear();
}

void ActionContext::regAction(QAction *act, const QString &id, const QString &defks, bool standard)
{
    ActionInfo *info = m_actionInfoMap.value(id);
    if (info == 0) {
        info = new ActionInfo;
        m_actionInfoMap.insert(id,info);
    }
    info->standard = standard;
    info->defks = ActionManager::formatShortcutsString(defks);
    info->ks = m_liteApp->settings()->value(LITEAPP_SHORTCUTS+id,info->defks).toString();
    info->ks = ActionManager::formatShortcutsString(info->ks);
    info->keys = ActionManager::toShortcuts(info->ks);
    if (act) {
        info->label = act->text();
        act->setShortcuts(info->keys);
        if (!info->ks.isEmpty()) {
            act->setToolTip(QString("%1 (%2)").arg(act->text()).arg(ActionManager::formatShortcutsNativeString(info->ks)));
        }
        info->action = act;
    } else {
        info->action = 0;
    }
}

void ActionContext::regAction(QAction *act, const QString &id, const QKeySequence::StandardKey &def)
{
    regAction(act,id,QKeySequence(def).toString(),true);
}

QStringList ActionContext::actionKeys() const
{
    return m_actionInfoMap.keys();
}

ActionInfo *ActionContext::actionInfo(const QString &key) const
{
    return m_actionInfoMap.value(key);
}

void ActionContext::setActionShortcuts(const QString &id, const QString &shortcuts)
{
    ActionInfo *info = m_actionInfoMap.value(id);
    if (!info) {
        return;
    }
    info->ks = ActionManager::formatShortcutsString(shortcuts);
    info->keys = ActionManager::toShortcuts(info->ks);
    if (info->action) {
        info->action->setShortcuts(info->keys);
        if (!info->ks.isEmpty()) {
            info->action->setToolTip(QString("%1 (%2)").arg(info->action->text()).arg(ActionManager::formatShortcutsNativeString(info->ks)));
        }
    }
    if (info->ks != info->defks) {
        m_liteApp->settings()->setValue(LITEAPP_SHORTCUTS+id,info->ks);
    } else {
        m_liteApp->settings()->remove(LITEAPP_SHORTCUTS+id);
    }
}
