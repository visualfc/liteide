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
// Module: filebrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filebrowser.h"
#include "golangdocapi/golangdocapi.h"
#include "liteenvapi/liteenvapi.h"
#include "litebuildapi/litebuildapi.h"
#include "litedebugapi/litedebugapi.h"
#include "litefindapi/litefindapi.h"
#include "fileutil/fileutil.h"
#include "filebrowser_global.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTreeView>
#include <QHeaderView>
#include <QToolBar>
#include <QAction>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#ifdef Q_OS_WIN
#include <windows.h>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end
#endif

FileBrowser::FileBrowser(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_widget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QDir::Filters filters = QDir::AllDirs | QDir::Files | QDir::Drives
                            | QDir::Readable| QDir::Writable
                            | QDir::Executable /*| QDir::Hidden*/
                            | QDir::NoDotAndDotDot;

    bool bShowHiddenFiles = m_liteApp->settings()->value(FILEBROWSER_SHOWHIDDENFILES,false).toBool();
    if (bShowHiddenFiles) {
        filters |= QDir::Hidden;
    }
    bool bShowDetails = m_liteApp->settings()->value(FILEBROWSER_SHOWDETAILS,false).toBool();

#ifdef Q_OS_WIN // Symlinked directories can cause file watcher warnings on Win32.
    filters |= QDir::NoSymLinks;
#endif
    //create filter toolbar
    //m_filterToolBar = new QToolBar(m_widget);
    //m_filterToolBar->setIconSize(QSize(16,16));

#ifdef Q_OS_MAC
    m_folderView = new FolderView(true,m_liteApp);
#else
    m_folderView = new FolderView(false,m_liteApp);
#endif
    m_folderView->setRootIsDecorated(true);
    m_folderView->setFilter(filters);

    m_syncAct = new QAction(QIcon("icon:images/sync.png"),tr("Synchronize with editor"),this);
    m_syncAct->setCheckable(true);

    m_reloadAct = new QAction(QIcon("icon:filebrowser/images/reload.png"),tr("Reload Folder"),this);

    m_showHideFilesAct = new QAction(tr("Show Hidden Files"),this);
    m_showHideFilesAct->setCheckable(true);
    if (bShowHiddenFiles) {
        m_showHideFilesAct->setChecked(true);
    }
    connect(m_showHideFilesAct,SIGNAL(triggered(bool)),this,SLOT(showHideFiles(bool)));

    m_showDetailsAct = new QAction(tr("Show Details"),this);
    m_showDetailsAct->setCheckable(true);
    if (bShowDetails) {
        m_showDetailsAct->setChecked(true);
    }
    connect(m_showDetailsAct,SIGNAL(triggered(bool)),m_folderView,SLOT(setShowDetails(bool)));

    m_folderView->setShowDetails(bShowDetails);

    m_executeFileAct = new QAction(tr("Execute File"),this);
    connect(m_executeFileAct,SIGNAL(triggered()),this,SLOT(executeFile()));

    m_debugFileAct = new QAction(tr("Debug File"),this);
    connect(m_debugFileAct,SIGNAL(triggered()),this,SLOT(debugFile()));

    m_fmctxFileSearchAction = new QAction(tr("File Search"),this);
    connect(m_fmctxFileSearchAction,SIGNAL(triggered()),this,SLOT(fmctxFileSearchAction()));

//    m_filterCombo = new QComboBox;
//    m_filterCombo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
//    m_filterCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
//    m_filterCombo->setEditable(true);
//    m_filterCombo->addItem("*");
//    m_filterCombo->addItem("Makefile;*.go;*.cgo;*.s;*.goc;*.y;*.e64;*.pro");
//    m_filterCombo->addItem("*.sh;Makefile;*.go;*.cgo;*.s;*.goc;*.y;*.*.c;*.cpp;*.h;*.hpp;*.e64;*.pro");

    //m_filterToolBar->addAction(m_syncAct);
    //m_filterToolBar->addSeparator();
    //m_filterToolBar->addWidget(m_filterCombo);

    //create root toolbar
    m_rootToolBar = new QToolBar(m_widget);
    m_rootToolBar->setIconSize(QSize(16,16));

    m_cdupAct = new QAction(QIcon("icon:filebrowser/images/cdup.png"),tr("Open Parent"),this);

    m_rootCombo = new QComboBox;
    m_rootCombo->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_rootCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    m_rootCombo->setEditable(false);

    m_rootToolBar->addAction(m_cdupAct);
    m_rootToolBar->addAction(m_reloadAct);
    m_rootToolBar->addSeparator();
    m_rootToolBar->addWidget(m_rootCombo);

    //mainLayout->addWidget(m_filterToolBar);
    mainLayout->addWidget(m_rootToolBar);
    mainLayout->addWidget(m_folderView);

    m_widget->setLayout(mainLayout);

    m_setRootAct = new QAction(tr("Set As Root Folder"),this);
    m_openFolderInNewWindowAct = new QAction(tr("Open Folder in New Window"),this);
    m_addToFoldersAct = new QAction(tr("Add to Folders"),this);

    connect(m_setRootAct,SIGNAL(triggered()),this,SLOT(setFolderToRoot()));
    connect(m_cdupAct,SIGNAL(triggered()),this,SLOT(cdUp()));
    connect(m_openFolderInNewWindowAct,SIGNAL(triggered()),this,SLOT(openFolderInNewWindow()));
    connect(m_addToFoldersAct,SIGNAL(triggered()),this,SLOT(addToFolders()));

    QList<QAction*> actions;
    m_filterMenu = new QMenu(tr("Filter"));
    m_filterMenu->setIcon(QIcon("icon:images/filter.png"));
    m_filterMenu->addAction(m_showHideFilesAct);
    m_filterMenu->addAction(m_showDetailsAct);
    actions << m_filterMenu->menuAction() << m_syncAct;

    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_widget,"FileSystem",tr("File System"),true,actions);
    connect(m_toolWindowAct,SIGNAL(toggled(bool)),this,SLOT(visibilityChanged(bool)));
    //connect(m_filterCombo,SIGNAL(activated(QString)),this,SLOT(activatedFilter(QString)));
    connect(m_rootCombo,SIGNAL(activated(QString)),this,SLOT(activatedRoot(QString)));
    connect(m_syncAct,SIGNAL(triggered(bool)),this,SLOT(syncFileModel(bool)));
    connect(m_reloadAct,SIGNAL(triggered()),this,SLOT(reloadFileModel()));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_folderView,SIGNAL(aboutToShowContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)),this,SLOT(aboutToShowContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)));
    //connect(m_folderView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openEditor(QModelIndex)));
    connect(m_folderView,SIGNAL(enterKeyPressed(QModelIndex)),this,SLOT(enterKeyPressedFolderView(QModelIndex)));
    connect(m_folderView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedFolderView(QModelIndex)));

    QString root = m_liteApp->settings()->value("FileBrowser/root","").toString();
    if (!root.isEmpty()) {
        addFolderToRoot(root);
    }
    bool b = m_liteApp->settings()->value("FileBrowser/synceditor",true).toBool();
    if (b) {
        m_syncAct->setChecked(true);
    }
}

FileBrowser::~FileBrowser()
{
    QString root = m_rootCombo->currentText();
    m_liteApp->settings()->setValue("FileBrowser/root",root);
    m_liteApp->settings()->setValue("FileBrowser/synceditor",m_syncAct->isChecked());
    m_liteApp->settings()->setValue(FILEBROWSER_SHOWHIDDENFILES,m_showHideFilesAct->isChecked());
    m_liteApp->settings()->setValue(FILEBROWSER_SHOWDETAILS,m_showDetailsAct->isChecked());

    delete m_filterMenu;
    delete m_widget;
}

void FileBrowser::visibilityChanged(bool)
{
}

void FileBrowser::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!m_syncAct->isChecked()) {
        return;
    }
    if (!editor) {
        return;
    }
    QString fileName = editor->filePath();
    if (fileName.isEmpty()) {
        return;
    }
    QFileInfo info(fileName);

    addFolderToRoot(info.path());

    QModelIndex index = m_folderView->indexForPath(fileName);
    if (!index.isValid()) {
        return;
    }
    m_folderView->scrollTo(index,QAbstractItemView::EnsureVisible);
    m_folderView->setCurrentIndex(index);
}

void FileBrowser::syncFileModel(bool b)
{
    if (b == false) {
        return;
    } else {
        currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    }
}

void FileBrowser::reloadFileModel()
{
    m_folderView->reload();
}

void FileBrowser::aboutToShowContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info)
{
    if (flag == LiteApi::FILESYSTEM_FILES) {
        QString cmd = FileUtil::lookPathInDir(info.fileName(),info.path());
        if (!cmd.isEmpty()) {
            QAction *act = 0;
            if (!menu->actions().isEmpty()) {
                act = menu->actions().at(0);
            }
            menu->insertAction(act,m_executeFileAct);
            bool hasGo = false;
            foreach(QFileInfo info, QDir(info.path()).entryInfoList(QDir::Files)) {
                if (info.suffix() == "go") {
                    hasGo = true;
                    break;
                }
            }
            if (hasGo) {
                menu->insertAction(act,m_debugFileAct);
            }
            menu->insertSeparator(act);
        }
    } else if (flag == LiteApi::FILESYSTEM_FOLDER || flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
        menu->addSeparator();
        menu->addAction(m_fmctxFileSearchAction);
        menu->addSeparator();
        if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
            menu->addAction(m_cdupAct);
        } else {
            menu->addAction(m_setRootAct);
        }
        menu->addAction(m_addToFoldersAct);
        menu->addAction(m_openFolderInNewWindowAct);
    }
}

void FileBrowser::fmctxFileSearchAction()
{
    LiteApi::IFileSearchManager *mgr = LiteApi::getFileSearchManager(m_liteApp);
    if (!mgr) {
        return;
    }
    bool hasGo = false;
    foreach(QFileInfo info, m_folderView->contextDir().entryInfoList(QDir::Files)) {
        if (info.suffix() == "go") {
            hasGo = true;
            break;
        }
    }
    mgr->showFileSearch("",hasGo ? "*.go": "*",m_folderView->contextDir().path());
}

void FileBrowser::showHideFiles(bool b)
{
    if (isShowHideFiles() == b) {
        return;
    }
    QDir::Filters filters = m_folderView->filter();
    if (b) {
        filters |= QDir::Hidden;
    } else {
        filters ^= QDir::Hidden;
    }
    m_folderView->setFilter(filters);
}

bool FileBrowser::isShowHideFiles() const
{
    return m_folderView->filter() & QDir::Hidden;
}

void FileBrowser::openFolderInNewWindow()
{
    QDir dir = m_folderView->contextDir();
    m_liteApp->fileManager()->openFolderInNewWindow(dir.path());
}

void FileBrowser::addToFolders()
{
    QDir dir = m_folderView->contextDir();
    m_liteApp->fileManager()->addFolderList(dir.path());
}

void FileBrowser::executeFile()
{
    LiteApi::ILiteBuild *build = LiteApi::getLiteBuild(m_liteApp);
    if (build) {
        QFileInfo info = m_folderView->contextFileInfo();
        QString cmd = FileUtil::lookPathInDir(info.fileName(),info.path());
        if (!cmd.isEmpty()) {
            build->execCommand(cmd,QString(),info.path(),true,true,false);
        }
    }
}

void FileBrowser::debugFile()
{
    LiteApi::ILiteDebug *debug = LiteApi::getLiteDebug(m_liteApp);
    if (debug) {
        QFileInfo info = m_folderView->contextFileInfo();
        debug->startDebug(info.fileName(),"",info.path());
    }
}

void FileBrowser::enterKeyPressedFolderView(const QModelIndex &index)
{
    QFileInfo info = m_folderView->fileInfo(index);
    if (info.isFile()) {
        m_liteApp->fileManager()->openEditor(info.filePath());
    } else if (info.isDir()) {
        m_folderView->setExpanded(index,!m_folderView->isExpanded(index));
    }
}

void FileBrowser::doubleClickedFolderView(const QModelIndex &index)
{
    QFileInfo info = m_folderView->fileInfo(index);
    if (info.isFile()) {
        m_liteApp->fileManager()->openEditor(info.filePath());
    }
}

void FileBrowser::addFolderToRoot(const QString &path)
{
    int index = -1;
    for (int i = 0; i < m_rootCombo->count(); i++) {
        QString text = m_rootCombo->itemText(i);
        if (text == path) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        m_rootCombo->addItem(path);
        index = m_rootCombo->count()-1;
    }
    m_rootCombo->setCurrentIndex(index);
    activatedRoot(path);
}

void FileBrowser::setFolderToRoot()
{
    QDir dir = m_folderView->contextDir();
    addFolderToRoot(dir.path());
}

void FileBrowser::activatedRoot(QString path)
{
    m_folderView->setRootPath(path);
}

void FileBrowser::cdUp()
{
    QString path = m_folderView->rootPath();
    if (path.isEmpty()) {
        return;
    }
    QDir dir(path);
    if (!dir.path().isEmpty() && dir.cdUp()) {
        addFolderToRoot(dir.path());
    }
}
