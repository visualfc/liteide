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
// Module: quickopenfilesystem.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenfilesystem.h"
#include "folderview/filesystemmodelex.h"
#include "folderview/dirsortfilterproxymodel.h"
#include <QFileSystemModel>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QuickOpenFileSystem::QuickOpenFileSystem(LiteApi::IApplication *app, QObject *parent) : LiteApi::IQuickOpenFileSystem(parent)
{
    m_liteApp = app;
    m_model = new FileSystemModelEx(this);
    bool proxyMode = false;
#ifdef Q_OS_MAC
    proxyMode = true;
#endif
    if (proxyMode) {
        m_proxy = new FileSystemProxyModel(this);
        m_proxy->setSourceModel(m_model);
        m_proxy->sort(0);
    } else {
        m_proxy = 0;
    }
}

QString QuickOpenFileSystem::id() const
{
    return "quickopen/filesystem";
}

QString QuickOpenFileSystem::placeholderText() const
{
    return m_placehoderText;
}

void QuickOpenFileSystem::activate()
{

}

QAbstractItemModel *QuickOpenFileSystem::model() const
{
    if (m_proxy) {
        return m_proxy;
    }
    return m_model;
}

QModelIndex QuickOpenFileSystem::rootIndex() const
{
    if (m_proxy) {
        return m_proxy->mapFromSource(m_rootIndex);
    }
    return m_rootIndex;
}

void QuickOpenFileSystem::updateModel()
{

}

QModelIndex QuickOpenFileSystem::filterChanged(const QString &text)
{
    int count = m_model->rowCount(m_rootIndex);
    for (int i = 0; i < count; i++) {
        QModelIndex index = m_model->index(i,0,m_rootIndex);
        if (m_model->fileName(index).startsWith(text,Qt::CaseInsensitive)) {
            if (m_proxy) {
                return m_proxy->mapFromSource(index);
            }
            return index;
        }
    }
    return QModelIndex();
}

void QuickOpenFileSystem::cancel()
{

}

void QuickOpenFileSystem::setRootPath(const QString &path)
{
    m_rootIndex = m_model->setRootPath(path);
}

void QuickOpenFileSystem::setPlaceholderText(const QString &text)
{
    m_placehoderText = text;
}

QModelIndex QuickOpenFileSystem::indexForPath(const QString &filePath) const
{
    QModelIndex index = m_model->index(filePath,0);
    if (m_proxy) {
        index = m_proxy->mapFromSource(index);
    }
    return index;
}

QString QuickOpenFileSystem::pathForIndex(const QModelIndex &index) const
{
    QModelIndex i = index;
    if (m_proxy) {
        i = m_proxy->mapToSource(index);
    }
    return m_model->filePath(index);
}

bool QuickOpenFileSystem::selected(const QString &/*text*/, const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QModelIndex fileIndex = index;
    if (m_proxy) {
        fileIndex = m_proxy->mapToSource(index);
    }
    if (m_model->isDir(fileIndex)) {
        return false;
    }
    QString filePath = m_model->filePath(fileIndex);
    if (!m_liteApp->fileManager()->openFile(filePath)) {
        return false;
    }
    return true;
}

void QuickOpenFileSystem::indexChanged(const QModelIndex &index)
{

}

QString QuickOpenFileSystem::info() const
{
    return tr("File System");
}
