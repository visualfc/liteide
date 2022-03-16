/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
    qDeleteAll(m_factoryList);
}

QList<IPluginFactory*> PluginManager::factoryList()
{
    return m_factoryList;
}

void PluginManager::loadPlugins(const QString &dir)
{
    if (m_bLoaded) {
        return;
    }
    m_bLoaded = true;

    QDir pluginsDir = dir;
    pluginsDir.setFilter(QDir::Files | QDir::NoSymLinks);

    QMap<QString,int> idIndexMap;
    QMap<QString,IPluginFactory*> idPlguinMap;
    foreach (QFileInfo info, pluginsDir.entryInfoList()) {
        QPluginLoader loader(info.filePath());
        if(!loader.load()) {
            qDebug() << loader.fileName() << loader.errorString();
        }
        if (IPluginFactory *factory = qobject_cast<IPluginFactory*>(loader.instance())) {
            if (factory) {
                factory->setFilePath(info.filePath());
                idIndexMap.insert(factory->id(),0);
                idPlguinMap.insert(factory->id(),factory);
            }
        }
    }

    if (idIndexMap.isEmpty()) {
        return;
    }

    foreach(IPluginFactory *p, idPlguinMap.values()) {
        foreach(QString depId, p->dependPluginList()) {
            idIndexMap.insert(depId,idIndexMap.value(depId)-1);
        }
    }
    QMultiMap<int,IPluginFactory*> deps;
    QMapIterator<QString,int> i(idIndexMap);
    while (i.hasNext()) {
        i.next();
        deps.insertMulti(i.value(),idPlguinMap.value(i.key()));
    }
    QList<int> keys = deps.keys().toSet().toList();
    qSort(keys);
    foreach(int index, keys) {
        foreach(IPluginFactory *p, deps.values(index)) {
            m_factoryList.append(p);
        }
    }
}

bool PluginManager::isLoaded() const
{
    return m_bLoaded;
}

PluginManager::PluginManager(QObject *parent) :
    QObject(parent),
    m_bLoaded(false)
{
}
