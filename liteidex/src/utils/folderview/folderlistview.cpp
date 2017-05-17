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
// Module: folderlistview.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "folderlistview.h"

#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProcess>
#include <QInputDialog>
#include <QFileDialog>
#include <QHeaderView>
#include <QSortFilterProxyModel>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

class FolderListProxyModel : public QSortFilterProxyModel
{
public:
    FolderListProxyModel(QObject *parent) :
        QSortFilterProxyModel(parent)
    {
    }
    virtual bool lessThan( const QModelIndex & left, const QModelIndex & right ) const
    {
        FolderListModel *model = static_cast<FolderListModel*>(this->sourceModel());
        if (model->isRootIndex(left) && model->isRootIndex(right)) {
            return false;
        }
        QFileInfo l = model->fileInfo(left);
        QFileInfo r = model->fileInfo(right);
        if (l.isDir() && r.isFile()) {
            return true;
        } else if (l.isFile() && r.isDir()) {
            return false;
        }
#ifdef Q_OS_WIN
        if (l.filePath().length() <= 3 || r.filePath().length() <= 3) {
            return l.filePath().at(0) < r.filePath().at(0);
        }
#endif
        return (l.fileName().compare(r.fileName(),Qt::CaseInsensitive) < 0);
    }
};


FolderListView::FolderListView(bool proxyMode, LiteApi::IApplication *app, QWidget *parent) :
    BaseFolderView(app,parent)
{   
    m_model = new FolderListModel(this);
    if (proxyMode) {
        m_proxy = new FolderListProxyModel(this);
        m_proxy->setSourceModel(m_model);
        m_proxy->sort(0);
        this->setModel(m_proxy);
    } else {
        m_proxy = 0;
        this->setModel(m_model);
    }

    this->setHeaderHidden(true);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
}

void FolderListView::setFilter(QDir::Filters filters)
{
    m_model->setFilter(filters);
}

QDir::Filters FolderListView::filter() const
{
    return m_model->filter();
}

QFileInfo FolderListView::fileInfo(const QModelIndex &index) const
{
    if (m_proxy) {
        return m_model->fileInfo(m_proxy->mapToSource(index));
    } else {
        return m_model->fileInfo(index);
    }
}

bool FolderListView::addRootPath(const QString &path)
{
    if (m_model->isRootPath(path)) {
        return true;
    }
    if (!m_model->addRootPath(path).isValid()) {
        m_liteApp->appendLog("Add path false",path,true);
        return false;
    }
    if (m_proxy) {
        m_proxy->invalidate();
    }
    return true;
}

void FolderListView::setRootPathList(const QStringList &pathList)
{
    m_model->clear();
    foreach(QString path, pathList) {
        m_model->addRootPath(path);
    }
    if (m_proxy) {
        m_proxy->invalidate();
    }
}

QStringList FolderListView::rootPathList() const
{
    return m_model->rootPathList();
}

void FolderListView::clear()
{
    m_model->clear();
}

void FolderListView::expandFolder(const QString &path, bool expand)
{
    QList<QModelIndex> indexList = m_model->indexForPath(path);
    foreach (QModelIndex sourceIndex, indexList) {
        QModelIndex index = sourceIndex;
        if (m_proxy) {
            index = m_proxy->mapFromSource(sourceIndex);
        }
        if (expand) {
            this->expand(index);
        } else {
            this->collapse(index);
        }
    }
}

QList<QModelIndex> FolderListView::indexForPath(const QString &path) const
{
    QList<QModelIndex> indexList = m_model->indexForPath(path);
    if (!m_proxy) {
        return indexList;
    }
    QList<QModelIndex> indexs;
    foreach (QModelIndex sourceIndex, indexList) {
        indexs.push_back(m_proxy->mapFromSource(sourceIndex));
    }
    return indexs;
}

void FolderListView::customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOT;
    QModelIndex index = this->indexAt(pos);
    if (index.isValid()) {
        m_contextIndex = index;
        QModelIndex sourceIndex = index;
        if (m_proxy) {
            sourceIndex = m_proxy->mapToSource(index);
        }
        m_contextInfo = m_model->fileInfo(sourceIndex);
        if (m_model->isRootIndex(sourceIndex)) {
            flag = LiteApi::FILESYSTEM_ROOTFOLDER;
        } else if (m_model->isDir(sourceIndex)) {
            flag = LiteApi::FILESYSTEM_FOLDER;
        } else {
            flag = LiteApi::FILESYSTEM_FILES;
        }
    } else {
        m_contextIndex = this->rootIndex();
        m_contextInfo = QFileInfo();
    }
    bool hasGo = false;
    if (flag != LiteApi::FILESYSTEM_ROOT) {
        foreach(QFileInfo info, contextDir().entryInfoList(QDir::Files)) {
            if (info.suffix() == "go") {
                hasGo = true;
            }
        }
    }
    //root folder
    if (flag == LiteApi::FILESYSTEM_ROOT) {
        menu.addAction(m_openFolderAct);
    } else if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addSeparator();
        menu.addAction(m_reloadFolderAct);
        menu.addAction(m_closeFolderAct);
        menu.addSeparator();
        if (hasGo) {
            menu.addAction(m_viewGodocAct);
            menu.addSeparator();
        }
        menu.addAction(m_openExplorerAct);
        menu.addAction(m_openShellAct);
    } else if (flag == LiteApi::FILESYSTEM_FOLDER) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addAction(m_renameFolderAct);
        menu.addAction(m_removeFolderAct);
        menu.addSeparator();
        if (hasGo) {
            menu.addAction(m_viewGodocAct);
            menu.addSeparator();
        }
        menu.addAction(m_openExplorerAct);
        menu.addAction(m_openShellAct);
    } else if (flag == LiteApi::FILESYSTEM_FILES) {
        menu.addAction(m_openEditorAct);
        menu.addSeparator();
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_renameFileAct);
        menu.addAction(m_removeFileAct);
        menu.addSeparator();
        if (hasGo) {
            menu.addAction(m_viewGodocAct);
            menu.addSeparator();
        }
        menu.addAction(m_openExplorerAct);
        menu.addAction(m_openShellAct);
    }
    emit aboutToShowContextMenu(&menu,flag,m_contextInfo);
    menu.exec(this->mapToGlobal(pos));
}

void FolderListView::removeFile()
{
    QFileInfo info = m_contextInfo;
    if (!info.isFile()) {
        return;
    }

    int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("Delete File"),
                          tr("Are you sure that you want to permanently delete this file?")
                          +"\n"+info.filePath(),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QModelIndex index = this->currentIndex();
        if (m_proxy) {
            index = m_proxy->mapToSource(index);
        }
        if (!m_model->remove(index)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete File"),
                                     tr("Failed to delete the file!"));
        }
    }
}

void FolderListView::removeFolder()
{
    QFileInfo info = m_contextInfo;
    if (!info.isDir()) {
        return;
    }

    int ret = QMessageBox::warning(m_liteApp->mainWindow(),tr("Delete Folder"),
                          tr("Are you sure that you want to permanently delete this folder and all of its contents?")
                          +"\n"+info.filePath(),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QModelIndex index = this->currentIndex();
        if (m_proxy) {
            index = m_proxy->mapToSource(index);
        }
        if (!m_model->remove(index)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete Folder"),
                                     tr("Failed to delete the folder!"));
        }
    }
}

void FolderListView::openFolder()
{
    m_liteApp->fileManager()->openFolder();
}

void FolderListView::closeFolder()
{
    QModelIndex index = this->currentIndex();
    if (m_proxy) {
        index = m_proxy->mapToSource(index);
    }
    m_model->removeRoot(index);
}

void FolderListView::reloadFolder()
{
    QModelIndex index = this->currentIndex();
    this->collapse(index);
    if (m_proxy) {
        index = m_proxy->mapToSource(index);
    }
    m_model->reloadRoot(index);
}

void FolderListView::closeAllFolders()
{
    m_model->clear();
}
