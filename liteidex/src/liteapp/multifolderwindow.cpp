/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2018 LiteIDE Team. All rights reserved.
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
// Module: multifolderwindow.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "multifolderwindow.h"
#include "liteapp_global.h"
#include <QMenu>
#include <QAction>

MultiFolderWindow::MultiFolderWindow(LiteApi::IApplication *app, QObject *parent) : IFolderWindow(parent)
{
    m_liteApp = app;
    m_folderListView = new MultiFolderView(app);

    QDir::Filters filters = QDir::AllDirs | QDir::Files | QDir::Drives
                            | QDir::Readable| QDir::Writable
                            | QDir::Executable /*| QDir::Hidden*/
                            | QDir::NoDotAndDotDot;

    bool bShowHiddenFiles = m_liteApp->settings()->value(LITEAPP_FOLDERSHOWHIDENFILES,false).toBool();
    if (bShowHiddenFiles) {
        filters |= QDir::Hidden;
    }
    this->showHideFiles(bShowHiddenFiles);
    m_showHideFilesAct = new QAction(tr("Show Hidden Files"),this);
    m_showHideFilesAct->setCheckable(true);
    if (bShowHiddenFiles) {
        m_showHideFilesAct->setChecked(true);
    }
    connect(m_showHideFilesAct,SIGNAL(triggered(bool)),this,SLOT(showHideFiles(bool)));

    m_folderListView->setFilter(filters);

    bool bShowDetails = m_liteApp->settings()->value(LITEAPP_FOLDERSHOWDETAILS,false).toBool();
    m_folderListView->setShowDetails(bShowDetails);
    m_showDetailsAct = new QAction(tr("Show Details"),this);
    m_showDetailsAct->setCheckable(true);
    if (bShowDetails) {
        m_showDetailsAct->setChecked(true);
    }
    connect(m_showDetailsAct,SIGNAL(triggered(bool)),m_folderListView,SLOT(setShowDetails(bool)));

    m_syncEditorAct = new QAction(QIcon("icon:images/sync.png"),tr("Synchronize with editor"),this);
    m_syncEditorAct->setCheckable(true);

    QList<QAction*> actions;
    m_filterMenu = new QMenu(tr("Filter"));
    m_filterMenu->setIcon(QIcon("icon:images/filter.png"));
    m_filterMenu->addAction(m_showHideFilesAct);
    m_filterMenu->addAction(m_showDetailsAct);
    actions << m_filterMenu->menuAction() << m_syncEditorAct;

    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_folderListView,"Folders",tr("Folders"),false,actions);

    connect(m_folderListView,SIGNAL(aboutToShowContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)),m_liteApp->fileManager(),SIGNAL(aboutToShowFolderContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)));
    connect(m_folderListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedFolderView(QModelIndex)));
    connect(m_folderListView,SIGNAL(enterKeyPressed(QModelIndex)),this,SLOT(enterKeyPressedFolderView(QModelIndex)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    connect(m_syncEditorAct,SIGNAL(triggered(bool)),this,SLOT(triggeredSyncEditor(bool)));
    bool b = m_liteApp->settings()->value(LITEAPP_FOLDERSSYNCEDITOR,false).toBool();
    if (b) {
        m_syncEditorAct->setChecked(true);
    }
}

MultiFolderWindow::~MultiFolderWindow()
{
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSSYNCEDITOR,m_syncEditorAct->isChecked());
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSHOWHIDENFILES,m_showHideFilesAct->isChecked());
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSHOWDETAILS,m_showDetailsAct->isChecked());
    delete m_filterMenu;
    delete m_folderListView;
}

QWidget *MultiFolderWindow::widget()
{
     return m_folderListView;
}

bool MultiFolderWindow::isShowHideFiles() const
{
    return m_folderListView->filter() & QDir::Hidden;
}

void MultiFolderWindow::showHideFiles(bool b)
{
    QDir::Filters filters = m_folderListView->filter();
    if (b) {
        filters |= QDir::Hidden;
    } else {
        filters ^= QDir::Hidden;
    }
    m_folderListView->setFilter(filters);
}

void MultiFolderWindow::doubleClickedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QFileInfo info = m_folderListView->fileInfo(index);
    if (info.isFile()) {
       m_liteApp->fileManager()->openEditor(info.filePath());
    }
}

void MultiFolderWindow::enterKeyPressedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QFileInfo info = m_folderListView->fileInfo(index);
    if (info.isFile()) {
        m_liteApp->fileManager()->openEditor(info.filePath());
    } else {
        m_folderListView->setExpanded(index,!m_folderListView->isExpanded(index));
    }
}

QStringList MultiFolderWindow::folderList() const
{
    return m_folderListView->rootPathList();
}

void MultiFolderWindow::setFolderList(const QStringList &folders)
{
    QStringList all = folders;
    all.removeDuplicates();
    m_folderListView->setRootPathList(all);
    foreach (QString folder, all) {
        m_liteApp->recentManager()->addRecent(folder,"folder");
    }
    if (m_folderListView->rootPathList().size() == 1) {
        m_folderListView->expandFolder(m_folderListView->rootPathList().first(),true);
    }
}

void MultiFolderWindow::addFolderList(const QString &folder)
{
    if (!m_folderListView->addRootPath(folder)) {
        return;
    }
    m_toolWindowAct->setChecked(true);
    m_liteApp->recentManager()->addRecent(folder,"folder");
    m_folderListView->expandFolder(folder,true);
}

void MultiFolderWindow::closeAllFolders()
{
    m_folderListView->closeAllFolders();
}

void MultiFolderWindow::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!m_syncEditorAct->isChecked()) {
        return;
    }
    if (!editor) {
        return;
    }
    QString fileName = editor->filePath();
    if (fileName.isEmpty()) {
        return;
    }
    QList<QModelIndex> indexList = m_folderListView->indexForPath(fileName);
    if (indexList.isEmpty()) {
        m_folderListView->setCurrentIndex(QModelIndex());
        return;
    }
    QModelIndex index = indexList.first();
//    QModelIndex parent = index.parent();
//    if (parent.isValid()) {
//        m_folderListView->setExpanded(parent,true);
//    }
    m_folderListView->scrollTo(index,QAbstractItemView::EnsureVisible);
    m_folderListView->setCurrentIndex(index);
}

void MultiFolderWindow::triggeredSyncEditor(bool b)
{
    if (b) {
        this->currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    }
}
