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
// Module: pluginmanager.cpp
// Creator: visualfc <visualfc@gmail.com>



#include "pluginmanager.h"
#include "pluginsdialog.h"

#include <QDir>
#include <QPluginLoader>
#include <QMenu>
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


PluginManager::~PluginManager()
{
    m_filePluginMap.clear();
    qDeleteAll(m_pluginList);
}

void PluginManager::addPlugin(IPlugin *plugin)
{
    m_pluginList.append(plugin);
}

void PluginManager::removePlugin(IPlugin *plugin)
{
    m_pluginList.removeOne(plugin);
}

QList<IPlugin*> PluginManager::pluginList()
{
    return m_pluginList;
}

bool PluginManager::loadPlugin(const QString &fileName)
{
    QPluginLoader loader(fileName);
    if (IPluginFactory *factory = qobject_cast<IPluginFactory*>(loader.instance())) {
        IPlugin *plugin = factory->createPlugin();
        if (plugin) {
            addPlugin(plugin);
            m_filePluginMap.insert(fileName,plugin);
            return true;
        }
    }
    return false;
}

void PluginManager::loadPlugins(const QString &dir)
{
    QDir pluginsDir = dir;
    pluginsDir.setFilter(QDir::Files | QDir::NoSymLinks);
    foreach (QString fileName, pluginsDir.entryList()) {
        loadPlugin(pluginsDir.absoluteFilePath(fileName));
    }
}

void PluginManager::aboutPlugins()
{
    PluginsDialog *dlg = new PluginsDialog(m_liteApp->mainWindow());
    QMapIterator<QString,IPlugin*> i = m_filePluginMap;
    while (i.hasNext()) {
        i.next();
        dlg->addPluginInfo(i.value()->info(),i.key());
    }
    dlg->exec();
}
