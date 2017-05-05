/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
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
// Module: recentmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "recentmanager.h"
#include "liteapp_global.h"
#include <QMenu>
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

RecentManager::RecentManager(QObject *parent)
    : IRecentManager(parent)
{
    m_maxRecentFiles = 32;
}

bool RecentManager::initWithApp(IApplication *app)
{
    if (!IRecentManager::initWithApp(app)) {
        return false;
    }

    m_maxRecentFiles = m_liteApp->settings()->value(LITEAPP_MAXRECENTFILES,32).toInt();

    m_recentMenu = m_liteApp->actionManager()->loadMenu("menu/recent");
    QAction *clearAllRecent = new QAction(tr("Clear All History"),this);
    m_recentSeparator = m_recentMenu->addSeparator();
    m_recentMenu->addAction(clearAllRecent);
    connect(clearAllRecent,SIGNAL(triggered(bool)),this,SLOT(clearAllRecentMenu()));

    registerRecent(new FileRecent(app,this));
    registerRecent(new FolderRecent(app,this));
    registerRecent(new SessionRecent(app,this));

    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));

    return true;
}

void RecentManager::registerRecent(IRecent *recent)
{
    if (m_recentTypeList.contains(recent->type())) {
        return;
    }
    m_recentTypeList.append(recent->type());
    m_recentList.append(recent);

    updateRecentMenu(recent->type());
}

QList<IRecent *> RecentManager::recentList() const
{
    return m_recentList;
}

IRecent *RecentManager::findRecent(const QString &type) const
{
    foreach (IRecent *recent, m_recentList) {
        if (recent->type() == type) {
            return recent;
        }
    }
    return 0;
}

QStringList RecentManager::recentTypeList() const
{
    return m_recentTypeList;
}

void RecentManager::addRecent(const QString &name, const QString &type)
{
    IRecent *recent = findRecent(type);
    if (!recent) {
        return;
    }
    recent->addRecent(name,m_maxRecentFiles);

    updateAppListRecentMenu(type);
}

void RecentManager::removeRecent(const QString &name, const QString &type)
{
    IRecent *recent = findRecent(type);
    if (!recent) {
        return;
    }
    recent->removeRecent(name);

    updateAppListRecentMenu(type);
}

QStringList RecentManager::recentNameList(const QString &type)
{
    IRecent *recent = findRecent(type);
    if (!recent) {
        return QStringList();
    }
    return recent->recentNameList();
}

void RecentManager::clearRecentNameList(const QString &type)
{
    IRecent *recent = findRecent(type);
    if (!recent) {
        return;
    }
    recent->clearRecentNameList();

    updateAppListRecentMenu(type);
}

void RecentManager::openRecent(const QString &name, const QString &type)
{
    IRecent *recent = findRecent(type);
    if (recent) {
        recent->openRecent(name);
    }
}

void RecentManager::updateRecentMenu(const QString &type)
{
    IRecent *recent = findRecent(type);
    if (!recent) {
        return;
    }
    QMenu *menu = m_mapRecentTypeMenu.value(type,0);
    if (!menu) {
        QString name = recent->displyType();
        QAction *act = new QAction(name,this);
        m_recentMenu->insertAction(m_recentSeparator,act);
        menu = new QMenu(type,m_recentMenu);
        act->setMenu(menu);
        m_mapRecentTypeMenu.insert(type,menu);
    }
    if (!menu) {
        return;
    }
    menu->clear();
    QAction *sep = menu->addSeparator();
    QAction *clear = menu->addAction(tr("Clear Menu"));
    clear->setData(type);
    connect(clear,SIGNAL(triggered(bool)),this,SLOT(clearRecentMenu()));
    int count = 0;
    foreach (QString name, this->recentNameList(type)) {
        if (count++ > m_maxRecentFiles) {
            return;
        }
        QAction *act = new QAction(name,menu);
        menu->insertAction(sep,act);
        act->setData(type);
        connect(act,SIGNAL(triggered()),this,SLOT(openRecentAction()));
    }
}

void RecentManager::updateAppListRecentMenu(const QString &type)
{
    foreach (IApplication *app, m_liteApp->instanceList()) {
        app->recentManager()->updateRecentMenu(type);
        ((RecentManager*)app->recentManager())->emitRecentNameListChanged(type);
    }
}

void RecentManager::emitRecentNameListChanged(const QString &type)
{
    emit recentNameListChanged(type);
}

void RecentManager::applyOption(const QString &opt)
{
    if (opt != OPTION_LITEAPP) {
        return;
    }
    m_maxRecentFiles = m_liteApp->settings()->value(LITEAPP_MAXRECENTFILES,32).toInt();
}

void RecentManager::openRecentAction()
{
    QAction *act = (QAction*)sender();
    if (!act) {
        return;
    }
    QString name = act->text();
    QString type = act->data().toString();
    IRecent *recent = findRecent(type);
    if (recent) {
        recent->openRecent(name);
    }
}

void RecentManager::clearRecentMenu()
{
    QAction *act = (QAction*)sender();
    if (!act) {
        return;
    }
    QString type = act->data().toString();

    clearRecentNameList(type);
}

void RecentManager::clearAllRecentMenu()
{
    foreach (IRecent *recent, m_recentList) {
        recent->clearRecentNameList();
        updateAppListRecentMenu(recent->type());
    }
}

