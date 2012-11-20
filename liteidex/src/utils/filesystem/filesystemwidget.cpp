/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// date: 2012-4-26
// $Id: filesystemwidget.cpp,v 1.0 2012-4-26 visualfc Exp $

#include "filesystemwidget.h"
#include "liteenvapi/liteenvapi.h"
#include "litebuildapi/litebuildapi.h"
#include "fileutil/fileutil.h"
#include "../../plugins/filebrowser/createfiledialog.h"
#include "../../plugins/filebrowser/createdirdialog.h"

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

FileSystemWidget::FileSystemWidget(LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent),
    m_liteApp(app)
{
    m_tree = new SymbolTreeView;
    m_tree->setHeaderHidden(true);
    m_model = new FileSystemModel(this);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tree->setModel(m_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_tree);
    this->setLayout(layout);

    connect(m_tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openPathIndex(QModelIndex)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    m_fileMenu = new QMenu(this);
    m_folderMenu = new QMenu(this);

    m_openEditorAct = new QAction(tr("Open Editor"),this);
    m_newFileAct = new QAction(tr("New File"),this);
    m_newFileWizardAct = new QAction(tr("New File Wizard"),this);
    m_renameFileAct = new QAction(tr("Rename File"),this);
    m_removeFileAct = new QAction(tr("Remove File"),this);

    m_newFolderAct = new QAction(tr("New Folder"),this);
    m_renameFolderAct = new QAction(tr("Rename Folder"),this);
    m_removeFolderAct = new QAction(tr("Remove Folder"),this);

    m_openShellAct = new QAction(tr("Open Terminal Here"),this);
    m_openExplorerAct = new QAction(tr("Open Explorer Here"),this);

    m_fileMenu->addAction(m_openEditorAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_newFileAct);
    m_fileMenu->addAction(m_newFileWizardAct);
    m_fileMenu->addAction(m_renameFileAct);
    m_fileMenu->addAction(m_removeFileAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_openShellAct);
    m_fileMenu->addAction(m_openExplorerAct);

    m_folderMenu->addAction(m_newFileAct);
    m_folderMenu->addAction(m_newFileWizardAct);
    m_folderMenu->addAction(m_newFolderAct);
    m_folderMenu->addAction(m_renameFolderAct);
    m_folderMenu->addAction(m_removeFolderAct);
    m_folderMenu->addSeparator();
    m_folderMenu->addAction(m_openShellAct);
    m_folderMenu->addAction(m_openExplorerAct);


    connect(m_model->fileWatcher(),SIGNAL(directoryChanged(QString)),this,SLOT(directoryChanged(QString)));
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

    connect(m_tree,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(treeViewContextMenuRequested(QPoint)));
}

FileSystemWidget::~FileSystemWidget()
{
//    m_liteApp->settings()->setValue("GolangTool/synceditor",m_syncEditor->isChecked());
//    m_liteApp->settings()->setValue("GolangTool/syncproject",m_syncProject->isChecked());
//    m_liteApp->settings()->setValue("golangtool/gopath",m_pathList);
}

void FileSystemWidget::clear()
{
    m_model->clear();
}

void FileSystemWidget::directoryChanged(QString dir)
{
    SymbolTreeState state;
    m_tree->saveState(&state);
    m_model->directoryChanged(dir);
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

    CreateFileDialog dlg;
    dlg.setDirectory(dir.path());
    if (dlg.exec() == QDialog::Rejected) {
        return;
    }
    QString fileName = dlg.getFileName();
    if (!fileName.isEmpty()) {
        QString filePath = QFileInfo(dir,fileName).filePath();
        if (QFile::exists(filePath)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create File"),
                                     tr("The filename is exists!"));
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
                                             tr("Rename File"),tr("File Name"),
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

    int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("Remove File"),
                          tr("Confirm remove the file and continue"),
                          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (!QFile::remove(info.filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Remove File"),
                                     tr("Failed to remove the file!"));
        }
    }
}

void FileSystemWidget::newFolder()
{
    QDir dir = contextDir();

    CreateDirDialog dlg;
    dlg.setDirectory(dir.path());
    if (dlg.exec() == QDialog::Rejected) {
        return;
    }

    QString folderName = dlg.getDirPath();
    if (!folderName.isEmpty()) {
        if (!dir.entryList(QStringList() << folderName,QDir::Dirs).isEmpty()) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Create Folder"),
                                     tr("The folder name is exists!"));
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

    int ret = QMessageBox::warning(m_liteApp->mainWindow(),tr("Remove Folder"),
                          tr("Confirm remove the foler and continue"),
                          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        QDir dir = info.dir();
        m_model->fileWatcher()->removePath(info.filePath());
        if (!dir.rmdir(info.fileName())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Remove Folder"),
                                     tr("Failed to remove the folder!"));
            m_model->fileWatcher()->addPath(info.filePath());
        }
    }
}

void FileSystemWidget::openExplorer()
{
    QDir dir = contextDir();
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir.path()));
}

void FileSystemWidget::openShell()
{
    QDir dir = contextDir();
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    QString cmd = env.value("LITEIDE_TERM");
    QStringList args = env.value("LITEIDE_TERMARGS").split(" ");
    QString path = dir.path();
#ifdef Q_OS_WIN
    if (path.length() == 2 && path.right(1) == ":") {
        path += "/";
    }
#endif
    QProcess::startDetached(cmd,args,path);
}

void FileSystemWidget::treeViewContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = m_tree->indexAt(pos);
    if (!index.isValid()) {
        return;
    }
    FileNode *node = m_model->nodeFromIndex(index);
    if (!node) {
        return;
    }
    m_contextInfo = node->fileInfo();
    m_contextIndex = index;
    QMenu *contextMenu = 0;
    if (node->isDir()) {
        contextMenu = m_folderMenu;
    } else {
        contextMenu = m_fileMenu;
    }

    if (contextMenu && contextMenu->actions().count() > 0) {
        contextMenu->popup(m_tree->mapToGlobal(pos));
    }
}

void FileSystemWidget::addPathList(const QString &path)
{
    QStringList pathList = m_pathList;
    pathList.append(path);
    this->setPathList(pathList);
}

void FileSystemWidget::setPathList(const QStringList &pathList)
{
    m_model->setRootPathList(pathList);
    currentEditorChanged(m_liteApp->editorManager()->currentEditor());
}

void FileSystemWidget::setRootPath(const QString &path)
{
    m_model->setRootPath(path);
    m_tree->expand(m_model->startIndex());
}

QStringList FileSystemWidget::pathList() const
{
    return m_pathList;
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
        QFileInfo info(node->path());
        QString cmd = FileUtil::lookPathInDir(info.fileName(),LiteApi::getCurrentEnvironment(m_liteApp),info.path());
        if (cmd == node->path()) {
            LiteApi::ILiteBuild *build = LiteApi::getLiteBuild(m_liteApp);
            if (build) {
                build->executeCommand(info.fileName(),QString(),info.path());
                return;
            }
        }
        m_liteApp->fileManager()->openEditor(node->path(),true);
    }
}

void FileSystemWidget::currentEditorChanged(LiteApi::IEditor* editor)
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
