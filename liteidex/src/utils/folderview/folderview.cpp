/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: folderview.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "folderview.h"
#include "filesystemmodelex.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QDesktopServices>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

FolderView::FolderView(bool proxyMode, LiteApi::IApplication *app, QWidget *parent) :
    BaseFolderView(app,parent)
{
    m_model = new FileSystemModelEx(this);
    if (proxyMode) {
        m_proxy = new FileSystemProxyModel(this);
        m_proxy->setSourceModel(m_model);
        this->setModel(m_proxy);
        m_proxy->sort(0);
    } else {
        m_proxy = 0;
        this->setModel(m_model);
    }
    this->setHeaderHidden(true);

    m_contextMenu = new QMenu(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
}

FolderView::~FolderView()
{
    delete m_contextMenu;
}

void FolderView::setRootPath(const QString &path)
{
    QModelIndex index = m_model->setRootPath(path);
    if (!path.isEmpty() && !index.isValid()) {
#if QT_VERSION >= 0x050000
        QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
        QString home = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
        index = m_model->setRootPath(home);
    }
    if (m_proxy)
        this->setRootIndex(m_proxy->mapFromSource(index));
    else
        this->setRootIndex(index);
}

QString FolderView::rootPath() const
{
    return m_model->rootPath();
}

void FolderView::setFilter(QDir::Filters filters)
{
    m_model->setFilter(filters);
}

QDir::Filters FolderView::filter() const
{
    return m_model->filter();
}

void FolderView::setNameFilters(const QStringList &filters)
{
    m_model->setNameFilters(filters);
}

QStringList FolderView::nameFilters() const
{
    return m_model->nameFilters();
}

QFileInfo FolderView::fileInfo(const QModelIndex &index)
{
    if (m_proxy)
        return m_model->fileInfo(m_proxy->mapToSource(index));
    else
        return m_model->fileInfo(index);
}

QModelIndex FolderView::indexForPath(const QString &fileName)
{
    QString filePath = QDir::fromNativeSeparators(fileName);
    if (!filePath.startsWith(this->rootPath()+"/")) {
        return QModelIndex();
    }
    QModelIndex index = m_model->index(filePath);
    if (!index.isValid()) {
        return QModelIndex();
    }
    if (m_proxy) {
        return m_proxy->mapFromSource(index);
    }
    return index;
}

void FolderView::reload()
{
    this->collapseAll();
    this->setRootPath(this->rootPath());
}

bool FolderView::isShowDetails() const
{
    return m_model->isShowDetails();
}

void FolderView::setShowDetails(bool b)
{
    m_model->setShowDetails(b);
}

void FolderView::removeFile()
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

void FolderView::removeFolder()
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

void FolderView::customContextMenuRequested(const QPoint &pos)
{
    m_contextMenu->clear();

    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOT;
    QModelIndex index = this->indexAt(pos);
    if (index.isValid()) {
        bool isDir = false;
        if (m_proxy) {
            m_contextInfo = m_model->fileInfo(m_proxy->mapToSource(index));
            isDir = m_model->isDir(m_proxy->mapToSource(index));
        } else {
            m_contextInfo = m_model->fileInfo(index);
            isDir = m_model->isDir(index);
        }
        m_contextIndex = index;
        if (isDir) {
            flag = LiteApi::FILESYSTEM_FOLDER;
        } else {
            flag = LiteApi::FILESYSTEM_FILES;
        }
    } else {
        m_contextIndex = this->rootIndex();
        if (m_proxy) {
            m_contextInfo = m_model->fileInfo(m_proxy->mapToSource(m_contextIndex));
        } else {
            m_contextInfo = m_model->fileInfo(m_contextIndex);
        }
        flag = LiteApi::FILESYSTEM_ROOTFOLDER;
    }
    //root folder
    if (flag == LiteApi::FILESYSTEM_ROOT) {
        m_contextMenu->addAction(m_newFileAct);
        m_contextMenu->addAction(m_newFileWizardAct);
        m_contextMenu->addAction(m_newFolderAct);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_openExplorerAct);
        m_contextMenu->addAction(m_openShellAct);
    } else if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
#ifdef Q_OS_MAC
        if (m_contextInfo.isBundle()) {
            m_contextMenu->addAction(m_openBundleAct);
            m_contextMenu->addSeparator();
        }
#endif
        m_contextMenu->addAction(m_newFileAct);
        m_contextMenu->addAction(m_newFileWizardAct);
        m_contextMenu->addAction(m_newFolderAct);
        m_contextMenu->addAction(m_renameFolderAct);
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
    emit aboutToShowContextMenu(m_contextMenu,flag,m_contextInfo);
    m_contextMenu->exec(this->mapToGlobal(pos));
}
