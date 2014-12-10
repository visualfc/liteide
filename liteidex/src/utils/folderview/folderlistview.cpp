/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


FolderListView::FolderListView(LiteApi::IApplication *app, QWidget *parent) :
    BaseFolderView(app,parent)
{   
    m_model = new FolderListModel(this);
    this->setModel(m_model);

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
    return m_model->fileInfo(index);
}

void FolderListView::addRootPath(const QString &path)
{
    if (m_model->isRootPath(path)) {
        return;
    }
    m_model->addRootPath(path);
}

void FolderListView::setRootPathList(const QStringList &pathList)
{
    m_model->clear();
    foreach(QString path, pathList) {
        m_model->addRootPath(path);
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

void FolderListView::customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOT;
    QModelIndex index = this->indexAt(pos);
    if (index.isValid()) {
        m_contextIndex = index;
        m_contextInfo = m_model->fileInfo(index);
        if (m_model->isRootIndex(index)) {
            flag = LiteApi::FILESYSTEM_ROOTFOLDER;
        } else if (m_model->isDir(index)) {
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
        menu.addAction(m_addFolderAct);
    } else if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addAction(m_closeFolerAct);
        menu.addSeparator();
        if (hasGo) {
            menu.addAction(m_viewGodocAct);
            menu.addSeparator();
        }
        menu.addAction(m_openShellAct);
        menu.addAction(m_openExplorerAct);
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
        menu.addAction(m_openShellAct);
        menu.addAction(m_openExplorerAct);
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
        menu.addAction(m_openShellAct);
        menu.addAction(m_openExplorerAct);

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
                          tr("Are you sure that you want to permanently delete this file?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!m_model->remove(this->currentIndex())) {
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
                          tr("Are you sure that you want to permanently delete this folder and all of its contents?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!m_model->remove(this->currentIndex())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete Folder"),
                                     tr("Failed to delete the folder!"));
        }
    }
}

void FolderListView::addFolder()
{
#if QT_VERSION >= 0x050000
        static QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
        static QString home = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
    QString folder = QFileDialog::getExistingDirectory(m_liteApp->mainWindow(),tr("Add Folder"),home);
    if (folder.isEmpty()) {
        return;
    }
    m_model->addRootPath(folder);
    QDir dir(folder);
    if (dir.cdUp()) {
        home = dir.path();
    }
}

void FolderListView::closeFolder()
{
    m_model->removeRoot(this->currentIndex());
}

void FolderListView::closeAllFolders()
{
    m_model->clear();
}
