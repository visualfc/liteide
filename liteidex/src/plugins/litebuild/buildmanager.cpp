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
// Module: buildmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "buildmanager.h"
#include "build.h"
#include <QDir>
#include <QFileInfo>
#include <QFile>
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


BuildManager::BuildManager(QObject *parent)
    : LiteApi::IBuildManager(parent)
{
}

BuildManager::~BuildManager()
{
    qDeleteAll(m_buildList);
}

void BuildManager::addBuild(IBuild *build)
{
    m_buildList.append(build);
}

void BuildManager::removeBuild(IBuild *build)
{
    m_buildList.removeAll(build);
}

IBuild *BuildManager::findBuild(const QString &mimeType)
{
    foreach(IBuild *build, m_buildList) {
        if (build->mimeType() == mimeType) {
            return build;
        }
    }
    return 0;
}

QList<IBuild*> BuildManager::buildList() const
{
    return m_buildList;
}

void BuildManager::setCurrentBuild(LiteApi::IBuild *build)
{
    if (m_build == build) {
        return;
    }
    m_build = build;
    emit buildChanged(m_build);
}

IBuild *BuildManager::currentBuild() const
{
    return m_build;
}

void BuildManager::load(const QString &path)
{
    QDir dir = path;
    m_liteApp->appendLog("BuildManager","load "+path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.xml"));
    foreach (QString fileName, dir.entryList()) {
        Build::loadBuild(this,QFileInfo(dir,fileName).absoluteFilePath());
    }
}
