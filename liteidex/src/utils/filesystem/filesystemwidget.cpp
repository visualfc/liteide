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
// Module: filesystemwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filesystemwidget.h"
#include "liteenvapi/liteenvapi.h"
#include "litebuildapi/litebuildapi.h"
#include "golangdocapi/golangdocapi.h"
#include "fileutil/fileutil.h"
#include "../folderview/folderdialog.h"

#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProcess>
#include <QInputDialog>
#include <QLineEdit>
#include <QUrl>
#include <QToolBar>
#include <QFileSystemWatcher>
#include <QFileDialog>
#include <QHeaderView>
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

FileSystemWidget::FileSystemWidget(bool bMultiDirMode, LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent),
    m_liteApp(app),
    m_bMultiDirMode(bMultiDirMode),
    m_bHideRoot(false)
{
    m_tree = new SymbolTreeView;
    m_model = new FileSystemModel(this);

    QDir::Filters filters = QDir::AllDirs | QDir::Files | QDir::Drives
                            | QDir::Readable| QDir::Writable
                            | QDir::Executable/* | QDir::Hidden*/
                            | QDir::NoDotAndDotDot;
#ifdef Q_OS_WIN // Symlinked directories can cause file watcher warnings on Win32.
    filters |= QDir::NoSymLinks;
#endif
    m_model->setFilter(filters);

    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tree->setModel(m_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_tree);
    this->setLayout(layout);

    connect(m_tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openPathIndex(QModelIndex)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

//    m_fileMenu = new QMenu(this);
//    m_folderMenu = new QMenu(this);
//    m_rootMenu = new QMenu(this);

    m_openEditorAct = new QAction(tr("Open File"),this);
    m_newFileAct = new QAction(tr("New File..."),this);
    m_newFileWizardAct = new QAction(tr("New File Wizard..."),this);
    m_renameFileAct = new QAction(tr("Rename File..."),this);
    m_removeFileAct = new QAction(tr("Delete File"),this);

    m_newFolderAct = new QAction(tr("New Folder..."),this);
    m_renameFolderAct = new QAction(tr("Rename Folder..."),this);
    m_removeFolderAct = new QAction(tr("Delete Folder"),this);

    m_openShellAct = new QAction(tr("Open Terminal Here"),this);
    m_openExplorerAct = new QAction(tr("Open Explorer Here"),this);

    m_viewGodocAct = new QAction(tr("View Godoc Here"),this);

    m_addFolderAct = new QAction(tr("Add Folder..."),this);
    m_closeFolerAct = new QAction(tr("Close Folder"),this);

    m_closeAllFoldersAct = new QAction(tr("Close All Folders"),this);

//    m_fileMenu->addAction(m_executeFileAct);
//    m_fileMenu->addAction(m_openEditorAct);
//    m_fileMenu->addSeparator();
//    m_fileMenu->addAction(m_newFileAct);
//    m_fileMenu->addAction(m_newFileWizardAct);
//    m_fileMenu->addAction(m_renameFileAct);
//    m_fileMenu->addAction(m_removeFileAct);
//    m_fileMenu->addSeparator();
//    m_fileMenu->addAction(m_viewGodocAct);
//    m_fileMenu->addSeparator();
//    m_fileMenu->addAction(m_openShellAct);
//    m_fileMenu->addAction(m_openExplorerAct);

//    m_folderMenu->addAction(m_newFileAct);
//    m_folderMenu->addAction(m_newFileWizardAct);
//    m_folderMenu->addAction(m_newFolderAct);
//    m_folderMenu->addAction(m_renameFolderAct);
//    m_folderMenu->addAction(m_removeFolderAct);
//    m_folderMenu->addSeparator();
//    m_folderMenu->addAction(m_closeFolerAct);
//    m_folderMenu->addSeparator();
//    m_folderMenu->addAction(m_viewGodocAct);
//    m_folderMenu->addSeparator();
//    m_folderMenu->addAction(m_openShellAct);
//    m_folderMenu->addAction(m_openExplorerAct);

//    m_rootMenu->addAction(m_addFolderAct);
//    m_rootMenu->addSeparator();
    //m_rootMenu->addAction(m_closeAllFoldersAct);

    connect(m_model,SIGNAL(direcotryChanged(QString)),this,SLOT(reloadDirectory(QString)));
    connect(m_openEditorAct,SIGNAL(triggered()),this,SLOT(openEditor()));
    connect(m_newFileAct,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(m_newFileWizardAct,SIGNAL(triggered()),this,SLOT(newFileWizard()));
    connect(m_renameFileAct,SIGNAL(triggered()),this,SLOT(renameFile()));
    connect(m_removeFileAct,SIGNAL(triggered()),this,SLOT(removeFile()));
    connect(m_newFolderAct,SIGNAL(triggered()),this,SLOT(newFolder()));
    connect(m_renameFolderAct,SIGNAL(triggered()),this,SLOT(renameFolder()));
    connect(m_removeFolderAct,SIGNAL(triggered()),this,SLOT(removeFolder()));
    connect(m_openShellAct,SIGNAL(triggered()),this,SLOT(openShell()));
    connect(m_openExplorerAct,SIGNAL(triggered()),this,SLOT(openExplorer()));
    connect(m_viewGodocAct,SIGNAL(triggered()),this,SLOT(viewGodoc()));
    connect(m_addFolderAct,SIGNAL(triggered()),this,SLOT(addFolder()));
    connect(m_closeFolerAct,SIGNAL(triggered()),this,SLOT(closeFolder()));
    connect(m_closeAllFoldersAct,SIGNAL(triggered()),this,SLOT(closeAllFolders()));

    connect(m_tree,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(treeViewContextMenuRequested(QPoint)));
    connect(m_model,SIGNAL(modelReset()),this,SLOT(modelReset()));
}

FileSystemWidget::~FileSystemWidget()
{
//    m_liteApp->settings()->setValue("GolangTool/synceditor",m_syncEditor->isChecked());
//    m_liteApp->settings()->setValue("GolangTool/syncproject",m_syncProject->isChecked());
    //    m_liteApp->settings()->setValue("golangtool/gopath",m_pathList);
}

void FileSystemWidget::setHideRoot(bool b)
{
    if (m_bMultiDirMode) {
        return;
    }
    m_bHideRoot = b;
}

bool FileSystemWidget::isHideRoot() const
{
    return m_bHideRoot;
}

void FileSystemWidget::clear()
{
    m_model->clear();
}

SymbolTreeView *FileSystemWidget::treeView() const
{
    return m_tree;
}

FileSystemModel *FileSystemWidget::model() const
{
    return m_model;
}

QModelIndex FileSystemWidget::rootIndex() const
{
    if (m_model->rowCount() == 0) {
        return QModelIndex();
    }
    return m_model->index(0,0);
}

void FileSystemWidget::modelReset()
{
    if (m_bHideRoot && (m_model->rowCount() > 0)) {
        m_tree->setRootIndex(m_model->index(0,0));
    }
}

void FileSystemWidget::showHideFiles(bool b)
{
    if (isShowHideFiles() == b) {
        return;
    }
    QDir::Filters filters = m_model->filter();
    if (b) {
        filters |= QDir::Hidden;
    } else {
        filters ^= QDir::Hidden;
    }
    m_model->setFilter(filters);
    if (m_bHideRoot) {
        m_tree->expand(this->rootIndex());
    } else {
        m_tree->expandToDepth(0);
    }
}

bool FileSystemWidget::isShowHideFiles() const
{
    return m_model->filter() & QDir::Hidden;
}

void FileSystemWidget::reloadDirectory(QString dir)
{
    SymbolTreeState state;
    m_tree->saveState(&state);
    m_model->reloadDirectory(dir);
    m_tree->loadState(m_model,&state);
}

QDir FileSystemWidget::contextDir() const
{
    if (m_contextInfo.isDir()) {
        return m_contextInfo.filePath();
    }
    return m_contextInfo.dir();
}

QFileInfo FileSystemWidget::contextFileInfo() const
{
    return m_contextInfo;
}

void FileSystemWidget::openEditor()
{
    if (m_contextInfo.isFile()) {
        m_liteApp->fileManager()->openEditor(m_contextInfo.filePath());
    }
}

void FileSystemWidget::newFile()
{
    QDir dir = contextDir();

    CreateFileDialog dlg(m_liteApp->mainWindow());
    dlg.setDirectory(dir.path());
    if (dlg.exec() == QDialog::Rejected) {
        return;
    }
    QString fileName = dlg.getFileName();
    if (!fileName.isEmpty()) {
        QString filePath = QFileInfo(dir,fileName).filePath();
        if (QFile::exists(filePath)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create File"),
                                     tr("A file with that name already exists!"));
        } else {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.close();
                if (dlg.isOpenEditor()) {
                    m_liteApp->fileManager()->openEditor(filePath,true);
                }
            } else {
                QMessageBox::information(m_liteApp->mainWindow(),tr("Create File"),
                                         tr("Failed to create the file!"));
            }
        }
    }
}

void FileSystemWidget::newFileWizard()
{
    QString filePath;
    QString projPath;
    QFileInfo info = contextFileInfo();
    QDir dir = contextDir();
    if (!info.isFile()) {
        filePath = dir.absolutePath();
        projPath = dir.absolutePath();
    } else {
        filePath = dir.absolutePath();
        dir.cdUp();
        projPath = dir.absolutePath();
    }
    m_liteApp->fileManager()->execFileWizard(projPath,filePath);
}

void FileSystemWidget::renameFile()
{
    QFileInfo info = contextFileInfo();
    if (!info.isFile()) {
        return;
    }
    QString fileName = QInputDialog::getText(m_liteApp->mainWindow(),
                                             tr("Rename File"),tr("New Name:"),
                                             QLineEdit::Normal,info.fileName());
    if (!fileName.isEmpty() && fileName != info.fileName()) {
        QDir dir = contextDir();
#ifdef Q_OS_WIN
        if (!MoveFileW(info.filePath().toStdWString().c_str(),QFileInfo(dir,fileName).filePath().toStdWString().c_str())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename File"),
                                     tr("Failed to rename the file!"));
        }
#else
        if (!QFile::rename(info.filePath(),QFileInfo(dir,fileName).filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename File"),
                                     tr("Failed to rename the file!"));
        }
#endif
    }
}

void FileSystemWidget::removeFile()
{
    QFileInfo info = contextFileInfo();
    if (!info.isFile()) {
        return;
    }
	
    int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("Delete File"),
                          tr("Are you sure that you want to permanently delete this file?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!QFile::remove(info.filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete File"),
                                     tr("Failed to delete the file!"));
        }
    }
}

void FileSystemWidget::newFolder()
{
    QDir dir = contextDir();

    CreateDirDialog dlg(m_liteApp->mainWindow());
    dlg.setDirectory(dir.path());
    if (dlg.exec() == QDialog::Rejected) {
        return;
    }

    QString folderName = dlg.getDirName();
    if (!folderName.isEmpty()) {
        if (!dir.entryList(QStringList() << folderName,QDir::Dirs).isEmpty()) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create Folder"),
                                     tr("A folder with that name already exists!"));
        } else if (!dir.mkpath(folderName)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create Folder"),
                                     tr("Failed to create the folder!"));
        }
    }
}

void FileSystemWidget::renameFolder()
{
    QFileInfo info = contextFileInfo();
    if (!info.isDir()) {
        return;
    }

    QString folderName = QInputDialog::getText(m_liteApp->mainWindow(),
                                               tr("Rename Folder"),tr("Folder Name"),
                                               QLineEdit::Normal,info.fileName());
    if (!folderName.isEmpty() && folderName != info.fileName()) {
        QDir dir = contextDir();
        dir.cdUp();
#ifdef Q_OS_WIN
        QString _old = info.filePath();
        QString _new = dir.path()+"/"+folderName;
        if (!MoveFileW(_old.toStdWString().c_str(),_new.toStdWString().c_str())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename Folder"),
                                     tr("Failed to rename the folder!"));
        }
#else
        if (!dir.rename(info.fileName(),folderName)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename Folder"),
                                     tr("Failed to rename the folder!"));
        }
#endif
    }
}

void FileSystemWidget::removeFolder()
{
    QFileInfo info = contextFileInfo();
    if (!info.isDir()) {
        return;
    }
	
    int ret = QMessageBox::warning(m_liteApp->mainWindow(),tr("Delete Folder"),
                          tr("Are you sure that you want to permanently delete this folder and all of its contents?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QDir dir = info.dir();
        m_model->removeWatcher(info.filePath());
        if (!dir.rmdir(info.fileName())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete Folder"),
                                     tr("Failed to delete the folder!"));
            m_model->addWatcher(info.filePath());
        }
    }
}

void FileSystemWidget::openExplorer()
{
    QDir dir = contextDir();
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir.path()));
}

void FileSystemWidget::viewGodoc()
{
    QDir dir = contextDir();
    LiteApi::IGolangDoc *doc = LiteApi::findExtensionObject<LiteApi::IGolangDoc*>(m_liteApp,"LiteApi.IGolangDoc");
    if (doc) {
        QUrl url;
        url.setScheme("pdoc");
        url.setPath(dir.path());
        doc->openUrl(url);
        doc->activeBrowser();
    }
}

void FileSystemWidget::addFolder()
{
#if QT_VERSION >= 0x050000
        static QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
        static QString home = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
    QString folder = QFileDialog::getExistingDirectory(this,tr("Add Folder"),home);
    if (folder.isEmpty()) {
        return;
    }
    this->addRootPath(folder);
    QDir dir(folder);
    if (dir.cdUp()) {
        home = dir.path();
    }
}

void FileSystemWidget::closeFolder()
{
//    if (m_contextInfo.exists() && !m_contextInfo.isDir()) {
//        return;
//    }
    this->m_model->removeRootPath(m_contextInfo.filePath());
}

void FileSystemWidget::closeAllFolders()
{
    this->m_model->clear();
}

void FileSystemWidget::openShell()
{
    QDir dir = contextDir();
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    QString shell = env.value("LITEIDE_SHELL");
    if (!shell.isEmpty()) {
        foreach (QString info, shell.split(";",QString::SkipEmptyParts)) {
            QStringList ar = info.split(" ",QString::SkipEmptyParts);
            if (ar.size() >= 1) {
                QString cmd = FileUtil::lookPath(ar[0],LiteApi::getCurrentEnvironment(m_liteApp),false);
                if (!cmd.isEmpty()) {
                    QString path = dir.path();
                    ar.pop_front();
#ifdef Q_OS_MAC
                    ar.push_back(path);
#endif
#ifdef Q_OS_WIN
    if (path.length() == 2 && path.right(1) == ":") {
        path += "/";
    }
#endif
                    QProcess::startDetached(cmd,ar,path);
                    return;
                }
            }
        }
        return;
    }
    QString cmd = env.value("LITEIDE_TERM");
    QStringList args = env.value("LITEIDE_TERMARGS").split(" ",QString::SkipEmptyParts);
    qDebug() << cmd;
    QString path = dir.path();
#ifdef Q_OS_MAC
    args.append(path);
#endif
#ifdef Q_OS_WIN
    if (path.length() == 2 && path.right(1) == ":") {
        path += "/";
    }
#endif
    QProcess::startDetached(cmd,args,path);
}

void FileSystemWidget::treeViewContextMenuRequested(const QPoint &pos)
{
    //QMenu *pop = new QMenu(this);
    //connect(pop,SIGNAL(destroyed()),this,SLOT(destroy2()));
    QMenu menu(m_tree);
    LiteApi::FILESYSTEM_CONTEXT_FLAG flag = LiteApi::FILESYSTEM_ROOT;
    QModelIndex index = m_tree->indexAt(pos);
    if (index.isValid()) {
        FileNode *node = m_model->nodeFromIndex(index);
        if (node) {
            m_contextInfo = node->fileInfo();
            m_contextIndex = index;
            if (m_model->isRootPathNode(node)) {
                //contextMenu = m_folderMenu;
                flag = LiteApi::FILESYSTEM_ROOTFOLDER;
                //m_closeFolerAct->setVisible(true);
                //m_removeFolderAct->setVisible(false);
            } else if (node->isDir()) {
                //contextMenu = m_folderMenu;
                flag = LiteApi::FILESYSTEM_FOLDER;
                //m_closeFolerAct->setVisible(false);
                //m_removeFolderAct->setVisible(true);
            } else {
                //contextMenu = m_fileMenu;
                flag = LiteApi::FILESYSTEM_FILES;
            }
        }
    } else if (!m_bMultiDirMode) {
        m_contextIndex = this->rootIndex();
        FileNode *node = m_model->nodeFromIndex(m_contextIndex);
        if (node) {
            m_contextInfo = node->fileInfo();
        }
        flag = LiteApi::FILESYSTEM_ROOTFOLDER;
    }
    bool hasGo = false;
    if (!m_bMultiDirMode || (flag != LiteApi::FILESYSTEM_ROOT)) {
        foreach(QFileInfo info, contextDir().entryInfoList(QDir::Files)) {
            if (info.suffix() == "go") {
                hasGo = true;
            }
        }
    }
    //root folder
    if (flag == LiteApi::FILESYSTEM_ROOT) {
        if (m_bMultiDirMode) {
            menu.addAction(m_addFolderAct);
        } else {
            menu.addAction(m_newFileAct);
            menu.addAction(m_newFileWizardAct);
            menu.addAction(m_newFolderAct);
            menu.addSeparator();
            if (hasGo) {
                menu.addAction(m_viewGodocAct);
                menu.addSeparator();
            }
            menu.addAction(m_openShellAct);
            menu.addAction(m_openExplorerAct);
        }
    } else if (flag == LiteApi::FILESYSTEM_ROOTFOLDER) {
        menu.addAction(m_newFileAct);
        menu.addAction(m_newFileWizardAct);
        menu.addAction(m_newFolderAct);
        menu.addAction(m_renameFolderAct);
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
    menu.exec(m_tree->mapToGlobal(pos));
}

void FileSystemWidget::addRootPath(const QString &path)
{
    if (m_model->addRootPath(path)) {
        m_liteApp->fileManager()->addRecentFile(path,"folder");
        QModelIndex index = m_model->index(m_model->rowCount()-1,0,QModelIndex());
        if (index.isValid()) {
            m_tree->expand(index);
        }
    }
}

void FileSystemWidget::setRootPathList(const QStringList &pathList)
{
    m_model->setRootPathList(pathList);
    currentEditorChanged(m_liteApp->editorManager()->currentEditor());
}

void FileSystemWidget::setRootPath(const QString &path)
{
    m_model->setRootPath(path);
    if (m_bHideRoot) {
        m_tree->expand(this->rootIndex());
    } else {
        m_tree->expandToDepth(0);
    }
}

QString FileSystemWidget::rootPath() const
{
    QStringList paths = m_model->rootPathList();
    if (!paths.isEmpty()) {
        return paths.first();
    }
    return QString();
}

QStringList FileSystemWidget::rootPathList() const
{
    return m_model->rootPathList();
}

void FileSystemWidget::setStartIndex(const QModelIndex &index)
{
    QModelIndex oldIndex = m_model->startIndex();
    if (oldIndex != index) {
        m_model->setStartIndex(index);
        m_tree->update(oldIndex);
        m_tree->update(index);
        emit startPathChanged(m_model->filePath(index));
    }
}

QString FileSystemWidget::startPath() const
{
    QModelIndex index = m_model->startIndex();
    return m_model->filePath(index);
}

void FileSystemWidget::pathIndexChanged(const QModelIndex & index)
{
    FileNode *node = m_model->nodeFromIndex(index);
    if (node) {
        QFileInfo info = node->fileInfo();
        QModelIndex newIndex = index;
        if (info.isDir()) {
            newIndex = index;
        } else {
            newIndex = index.parent();
        }
        this->setStartIndex(newIndex);
    }
}

void FileSystemWidget::openPathIndex(const QModelIndex &index)
{
    FileNode *node = m_model->nodeFromIndex(index);
    if (!node) {
        return;
    }
    if (node->isFile()) {
        //QFileInfo info(node->path());
        QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(node->path());
        if (mimeType.startsWith("text/") || mimeType.startsWith("application/")) {
            m_liteApp->fileManager()->openEditor(node->path());
            return;
        }
//        QString cmd = FileUtil::lookPathInDir(info.fileName(),info.path());
//        if (cmd == node->path()) {
//            LiteApi::ILiteBuild *build = LiteApi::getLiteBuild(m_liteApp);
//            if (build) {
//                build->executeCommand(info.fileName(),QString(),info.path());
//                return;
//            }
//        }
        m_liteApp->fileManager()->openEditor(node->path(),true);
    }
}

void FileSystemWidget::currentEditorChanged(LiteApi::IEditor* /*editor*/)
{
    /*
    if (!m_syncEditor->isChecked()) {
        return;
    }
    if (editor && !editor->filePath().isEmpty()) {
        QModelIndex index = m_model->findPath(editor->filePath());
        if (index.isValid()) {
            m_tree->setCurrentIndex(index);
            m_tree->scrollTo(index,QAbstractItemView::EnsureVisible);
            if (m_syncProject->isChecked()) {
                setStartIndex(index.parent());
            }
        }
    }
    */
}

void FileSystemWidget::syncEditor(bool b)
{
    if (!b) {
       // m_syncProject->setChecked(false);
    }
}

void FileSystemWidget::expandStartPath(QString)
{
    m_tree->scrollTo(m_model->startIndex(),QAbstractItemView::EnsureVisible);
}
