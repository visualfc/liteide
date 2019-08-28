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
// Module: multifolderview.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "multifolderview.h"
#include <QMessageBox>
#include <QHeaderView>
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

MultiFolderView::MultiFolderView(LiteApi::IApplication *app, QWidget *parent)
    : BaseFolderView(app,parent)
{
    m_model = new MultiFolderModel(this);

#ifdef Q_OS_MAC
    m_model->sort(0);
    m_model->setSorting(QDir::DirsFirst|QDir::Name|QDir::IgnoreCase);
#endif

    this->setModel(m_model);
    this->setHeaderHidden(true);

    m_contextMenu = new QMenu(this);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
    connect(m_model,SIGNAL(directoryLoaded(QFileSystemModel*,QString)),this,SLOT(directoryLoaded(QFileSystemModel*,QString)));
}

MultiFolderView::~MultiFolderView()
{
}

void MultiFolderView::setFilter(QDir::Filters filters)
{
    m_model->setFilter(filters);
}

QDir::Filters MultiFolderView::filter() const
{
    return m_model->filter();
}

void MultiFolderView::setNameFilters(const QStringList &filters)
{
    m_model->setNameFilters(filters);
}

QStringList MultiFolderView::nameFilters() const
{
    return m_model->nameFilters();
}

QFileInfo MultiFolderView::fileInfo(const QModelIndex &index) const
{
    return m_model->fileInfo(index);
}

QString MultiFolderView::fileRootPath(const QModelIndex &index) const
{
    return m_model->fileRootPath(index);
}

bool MultiFolderView::addRootPath(const QString &path)
{
    if (m_model->isRootPath(path)) {
        return true;
    }
    QModelIndex index = m_model->addRootPath(path);
    return index.isValid();
}

void MultiFolderView::setRootPathList(const QStringList &pathList)
{
    m_model->clearAll();
    foreach (QString path, pathList) {
        m_model->addRootPath(path);
    }
}

QStringList MultiFolderView::rootPathList() const
{
    return m_model->rootPathList();
}

void MultiFolderView::clear()
{
    m_model->clearAll();
}

void MultiFolderView::expandFolder(const QString &path, bool expand)
{
    QList<QModelIndex> indexList = m_model->indexForPath(path);
    foreach (QModelIndex sourceIndex, indexList) {
        QModelIndex index = sourceIndex;
        if (expand) {
            this->expand(index);
        } else {
            this->collapse(index);
        }
    }
}

QList<QModelIndex> MultiFolderView::indexForPath(const QString &path) const
{
    return m_model->indexForPath(path);
}

void MultiFolderView::setShowDetails(bool b)
{
    m_model->setShowDetails(b);
}

bool MultiFolderView::isShowDetails() const
{
    return m_model->isShowDetails();
}

void MultiFolderView::customContextMenuRequested(const QPoint &pos)
{
    m_contextMenu->clear();

    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOT;
    QModelIndex index = this->indexAt(pos);
    if (index.isValid()) {
        m_contextIndex = index;
        QModelIndex sourceIndex = index;
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
    //root folder
    if (flag == LiteApi::FILESYSTEM_ROOT) {
        m_contextMenu->addAction(m_openFolderAct);
    } else if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
#ifdef Q_OS_MAC
        if (m_contextInfo.isBundle()) {
            m_contextMenu->addAction(m_openBundleAct);
            m_contextMenu->addSeparator();
        }
#endif
        m_contextMenu->addAction(m_openInNewWindowAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_newFileAct);
        m_contextMenu->addAction(m_newFileWizardAct);
        m_contextMenu->addAction(m_newFolderAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_reloadFolderAct);
        m_contextMenu->addAction(m_closeFolderAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_openExplorerAct);
        m_contextMenu->addAction(m_openShellAct);
    } else if (flag == LiteApi::FILESYSTEM_FOLDER) {
#ifdef Q_OS_MAC
        if (m_contextInfo.isBundle()) {
            m_contextMenu->addAction(m_openBundleAct);
            m_contextMenu->addSeparator();
        }
#endif
        m_contextMenu->addAction(m_openInNewWindowAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_newFileAct);
        m_contextMenu->addAction(m_newFileWizardAct);
        m_contextMenu->addAction(m_newFolderAct);
        m_contextMenu->addAction(m_renameFolderAct);
        m_contextMenu->addAction(m_removeFolderAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_copyFileAct);
        m_contextMenu->addAction(m_pasteFileAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_openExplorerAct);
        m_contextMenu->addAction(m_openShellAct);
    } else if (flag == LiteApi::FILESYSTEM_FILES) {
        m_contextMenu->addAction(m_openEditorAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_newFileAct);
        m_contextMenu->addAction(m_newFileWizardAct);
        m_contextMenu->addAction(m_renameFileAct);
        m_contextMenu->addAction(m_removeFileAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_copyFileAct);
        m_contextMenu->addAction(m_pasteFileAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_openExplorerAct);
        m_contextMenu->addAction(m_openShellAct);
    }
    m_pasteFileAct->setEnabled(this->canPasteFile());

    bool check = this->selectionModel()->selectedRows(0).size() == 1;
    m_removeFileAct->setEnabled(check);
    m_removeFolderAct->setEnabled(check);
    m_renameFileAct->setEnabled(check);
    m_renameFolderAct->setEnabled(check);

    emit aboutToShowContextMenu(m_contextMenu,flag,m_contextInfo);
    m_contextMenu->exec(this->mapToGlobal(pos));
}

void MultiFolderView::removeFolder()
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
        if (!m_model->remove(index)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete Folder"),
                                     tr("Failed to delete the folder!"));
        }
    }
}


void MultiFolderView::removeFile()
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
        if (!m_model->remove(index)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete File"),
                                     tr("Failed to delete the file!"));
        }
    }
}

void MultiFolderView::openFolder()
{
    m_liteApp->fileManager()->openFolder();
}

void MultiFolderView::closeFolder()
{
    QModelIndex index = this->currentIndex();
    m_model->removeRoot(index);
}

void MultiFolderView::reloadFolder()
{
    m_model->reloadAll();
}

void MultiFolderView::closeAllFolders()
{
    m_model->clearAll();
}

void MultiFolderView::directoryLoaded(QFileSystemModel *model, const QString &path)
{
    QModelIndex index = m_model->indexForPath(model,path);
    if (!index.isValid()) {
        return;
    }
}

QModelIndexList MultiFolderView::selectionCopyOrRemoveList() const
{
    QModelIndexList selection = this->selectionModel()->selectedRows(0);
    if (selection.size() <= 1) {
        return selection;
    }
    QStringList dirList;
    foreach (QModelIndex index, selection) {
        if (m_model->isDir(index)) {
            dirList  << QDir::cleanPath(m_model->fileInfo(index).filePath());
        }
    }
    QModelIndexList itemList;
    foreach (QModelIndex index, selection) {
        QString filePath = QDir::cleanPath(m_model->fileInfo(index).filePath());
        QStringList chkList = dirList;
        chkList.removeAll(filePath);
        bool find = false;
        foreach (QString chk, chkList) {
            if (filePath.startsWith(chk+"/")) {
                find = true;
                break;
            }
        }
        if (!find) {
            itemList << index;
        }
    }
    return itemList;
}
