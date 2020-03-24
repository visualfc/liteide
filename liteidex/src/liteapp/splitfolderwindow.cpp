/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2018 LiteIDE. All rights reserved.
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
// Module: splitfolderwindow.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "splitfolderwindow.h"
#include "symboltreeview/symboltreeview.h"
#include "liteapp_global.h"
#include <QTreeView>
#include <QStackedWidget>
#include <QSplitter>
#include <QDir>
#include <QAction>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMenu>
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

SplitFolderWindow::SplitFolderWindow(IApplication *app, QObject *parent)
    : IFolderWindow(parent), m_liteApp(app)
{
    m_spliter = new QSplitter(Qt::Vertical);

    m_tree = new SplitFolderView(app);
    m_tree->setHeaderHidden(true);
    m_tree->setRootIsDecorated(false);

    m_stacked = new QStackedWidget();
    m_spliter->addWidget(m_tree);
    m_spliter->addWidget(m_stacked);
    m_spliter->setStretchFactor(0,0);
    m_spliter->setStretchFactor(1,1);

    m_filters = QDir::AllDirs | QDir::Files | QDir::Drives
                            | QDir::Readable| QDir::Writable
                            | QDir::Executable /*| QDir::Hidden*/
                            | QDir::NoDotAndDotDot;
    m_bShowDetails = false;
    m_bSyncEditor = false;

    connect(m_tree,SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)),this,SLOT(currentIndexChanged(QModelIndex,QModelIndex)));
    connect(m_tree,SIGNAL(aboutToShowContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)),m_liteApp->fileManager(),SIGNAL(aboutToShowFolderContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)));
    connect(m_tree,SIGNAL(closeFolderIndex(QModelIndex)),this,SLOT(closeFolderIndex(QModelIndex)));
    connect(m_tree,SIGNAL(reloadFolderIndex(QModelIndex)),this,SLOT(reloadFolderIndex(QModelIndex)));

    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    QByteArray state = m_liteApp->settings()->value("LiteApp/BoxFolderSplitter").toByteArray();
    m_spliter->restoreState(state);
}

SplitFolderWindow::~SplitFolderWindow()
{
    m_liteApp->settings()->setValue("LiteApp/BoxFolderSplitter",m_spliter->saveState());

    delete m_spliter;
}

QString SplitFolderWindow::id() const
{
    return "folderwindow/splitfolder";
}

QWidget *SplitFolderWindow::widget() const
{
    return m_spliter;
}

QStringList SplitFolderWindow::folderList() const
{
    return m_folderList;
}

void SplitFolderWindow::setFolderList(const QStringList &folders)
{
    foreach (QString folder, folders) {
       addFolderImpl(folder);
    }
}

void SplitFolderWindow::addFolderList(const QString &folder)
{
    addFolderImpl(folder);
}

void SplitFolderWindow::closeAllFolders()
{
    m_folderList.clear();
    m_tree->clear();
    int count = m_stacked->count();
    while (count) {
        count--;
        QWidget *widget = m_stacked->widget(count);
        m_stacked->removeWidget(widget);
        delete widget;
    }
}

void SplitFolderWindow::currentIndexChanged(const QModelIndex &index, const QModelIndex &/*prev*/)
{
    int row = findInStacked(index);
    if (row == -1) {
        return;
    }
    QString folder = index.data(Qt::UserRole+1).toString();
    if (!QFileInfo(folder).exists()) {
        m_tree->closeFolder();
        return;
    }
    FolderView *widget = (FolderView*)m_stacked->widget(row);
    if (widget->filter() != m_filters) {
        widget->setFilter(m_filters);
    }

    if (widget->isShowDetails() != m_bShowDetails) {
        widget->setShowDetails(m_bShowDetails);
    }
    m_stacked->setCurrentIndex(row);
}

void SplitFolderWindow::closeFolderIndex(const QModelIndex &index)
{
    int row = findInStacked(index);
    if (row == -1) {
        return;
    }
    QWidget *widget = m_stacked->widget(row);
    m_stacked->removeWidget(widget);
    delete widget;
    m_folderList.removeAt(row);
}

void SplitFolderWindow::reloadFolderIndex(const QModelIndex &index)
{
    int row = findInStacked(index);
    if (row == -1) {
        return;
    }
    FolderView *widget = (FolderView*)m_stacked->widget(row);
    widget->reload();
}

void SplitFolderWindow::setShowHideFiles(bool b)
{
    if (b) {
        m_filters |= QDir::Hidden;
    } else if (m_filters.testFlag(QDir::Hidden)) {
        m_filters ^= QDir::Hidden;
    }
    FolderView *widget = (FolderView*)m_stacked->currentWidget();
    if (!widget) {
        return;
    }
    if (widget->filter() != m_filters) {
        widget->setFilter(m_filters);
    }
}

void SplitFolderWindow::setShowDetails(bool b)
{
    m_bShowDetails = b;
    FolderView *widget = (FolderView*)m_stacked->currentWidget();
    if (!widget) {
        return;
    }
    if (widget->isShowDetails() != b) {
        widget->setShowDetails(b);
    }
}

void SplitFolderWindow::setSyncEditor(bool b)
{
    m_bSyncEditor = b;
    if (b) {
        this->currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    }
}

void SplitFolderWindow::currentEditorChanged(IEditor *editor)
{
    if (!editor || !m_bSyncEditor) {
        return;
    }
    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    filePath = QDir::toNativeSeparators(filePath);

    FolderView *widget = (FolderView*)m_stacked->currentWidget();
    if (widget) {
        QModelIndex index = widget->indexForPath(filePath);
        if (index.isValid()) {
            widget->scrollTo(index);
            widget->clearSelection();
            widget->setCurrentIndex(index);
            return;
        }
    }
    int count = m_folderList.count();
    for (int i = 0; i < count; i++) {
        QString folder = m_folderList.at(i);
        if (!QFileInfo(folder).exists()) {
            continue;
        }
        FolderView *widget = (FolderView*)m_stacked->widget(i);
        QModelIndex index = widget->indexForPath(filePath);
        if (index.isValid()) {
            widget->scrollTo(index);
            widget->clearSelection();
            widget->setCurrentIndex(index);
            m_tree->setCurrentIndex(m_tree->model()->index(i,0));
            m_stacked->setCurrentIndex(i);
            return;
        }
    }
}

void SplitFolderWindow::doubleClickedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    FolderView *view = (FolderView*)(sender());
    QFileInfo info = view->fileInfo(index);
    if (info.isFile()) {
       m_liteApp->fileManager()->openEditor(info.filePath());
    }
}

void SplitFolderWindow::enterKeyPressedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    FolderView *view = (FolderView*)(sender());
    QFileInfo info = view->fileInfo(index);
    if (info.isFile()) {
        m_liteApp->fileManager()->openEditor(info.filePath());
    } else {
        view->setExpanded(index,!view->isExpanded(index));
    }
}

void SplitFolderWindow::addFolderImpl(const QString &_folder)
{
    QString folder = QDir::toNativeSeparators(_folder);
    if (m_folderList.contains(folder)) {
        return;
    }
    if (!QDir(folder).exists()) {
        return;
    }
    FolderView *view = new FolderView(true,m_liteApp);
    view->setFilter(m_filters);
    view->setShowDetails(m_bShowDetails);
    view->setRootPath(folder);
    connect(view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedFolderView(QModelIndex)));
    connect(view,SIGNAL(enterKeyPressed(QModelIndex)),this,SLOT(enterKeyPressedFolderView(QModelIndex)));
    connect(view,SIGNAL(aboutToShowContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)),m_liteApp->fileManager(),SIGNAL(aboutToShowFolderContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)));

    m_stacked->addWidget(view);
    m_folderList.append(folder);
    m_tree->addRootPath(folder);
    m_liteApp->recentManager()->addRecent(folder,"folder");
}

int SplitFolderWindow::findInStacked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return -1;
    }
    QString folder = index.data(Qt::UserRole+1).toString();
    return m_folderList.indexOf(folder);
}


SplitFolderView::SplitFolderView(IApplication *app, QWidget *parent)
    : BaseFolderView(app,parent)
{
    m_model = new QStandardItemModel(this);
    this->setModel(m_model);
    m_contextMenu = new QMenu();

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
}

void SplitFolderView::addRootPath(const QString &folder)
{
    QStandardItem *item = new QStandardItem(folder);
    item->setData(folder,Qt::UserRole+1);
    item->setToolTip(folder);

    m_model->appendRow(item);
    this->setCurrentIndex(m_model->indexFromItem(item));
}

void SplitFolderView::clear()
{
    m_model->clear();
}

void SplitFolderView::openFolder()
{
    m_liteApp->fileManager()->openFolder();
}

void SplitFolderView::closeFolder()
{
    QModelIndex index = this->currentIndex();
    if (!index.isValid()) {
        return;
    }
    emit closeFolderIndex(index);
    m_model->removeRow(index.row());
}

void SplitFolderView::reloadFolder()
{
    QModelIndex index = this->currentIndex();
    if (!index.isValid()) {
        return;
    }
    emit reloadFolderIndex(index);
}

void SplitFolderView::customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = this->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QString dir = index.data(Qt::UserRole+1).toString();
    QFileInfo fileInfo(dir);

    m_contextMenu->clear();
    m_contextInfo = fileInfo;

    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOTFOLDER;
    m_contextMenu->addAction(m_openInNewWindowAct);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_newFileAct);
    m_contextMenu->addAction(m_newFileWizardAct);
    m_contextMenu->addAction(m_newFolderAct);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_openFolderAct);
    m_contextMenu->addAction(m_reloadFolderAct);
    m_contextMenu->addAction(m_closeFolderAct);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_openExplorerAct);
    m_contextMenu->addAction(m_openShellAct);
    emit aboutToShowContextMenu(m_contextMenu,flag,m_contextInfo);
    m_contextMenu->exec(this->mapToGlobal(pos));
}
