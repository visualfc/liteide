/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: folderproject.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "folderproject.h"
#include <QFileInfo>
#include "memory.h"

FolderProject::FolderProject(IApplication *app) :
    m_liteApp(app), m_folderView(0)
{
#ifdef Q_OS_MAC
    m_folderView = new FolderListView(true,m_liteApp);
#else
    m_folderView = new FolderListView(false,m_liteApp);
#endif
}

FolderProject::~FolderProject()
{
    if (m_folderView) {
        delete m_folderView;
    }
}

QWidget *FolderProject::widget()
{
    return m_folderView;
}

QString FolderProject::name() const
{
    return "FolderProject";
}

QString FolderProject::filePath() const
{
    QStringList paths = m_folderView->rootPathList();
    if (!paths.isEmpty()) {
        return paths[0];
    }
    return QString();
}

QString FolderProject::mimeType() const
{
    return "folder/x-path";
}

void FolderProject::openFolder(const QString &folder)
{
    m_folderView->setRootPathList(QStringList() << folder);
}

void FolderProject::appendFolder(const QString &folder)
{
    m_folderView->addRootPath(folder);
}

void FolderProject::clear()
{
    m_folderView->clear();
}

QStringList FolderProject::folderList() const
{
    return m_folderView->rootPathList();
}

QStringList FolderProject::fileNameList() const
{
    return QStringList();
}

QStringList FolderProject::filePathList() const
{
    return QStringList();
}

QString FolderProject::fileNameToFullPath(const QString &filePath)
{
    return filePath;
}

QMap<QString,QString> FolderProject::targetInfo() const
{
    return QMap<QString,QString>();
}

void FolderProject::load()
{

}
