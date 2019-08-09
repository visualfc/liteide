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
// Module: basefolderview.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "basefoldeview.h"
#include "liteenvapi/liteenvapi.h"
#include "fileutil/fileutil.h"
#include "folderdialog.h"

#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QProcess>
#include <QInputDialog>
#include <QLineEdit>
#include <QUrl>
#include <QToolBar>
#include <QFileSystemWatcher>
#include <QFileDialog>
#include <QHeaderView>
#include <QProcess>
#include <QProcessEnvironment>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QFile>
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

BaseFolderView::BaseFolderView(LiteApi::IApplication *app, QWidget *parent) :
    SymbolTreeView(parent), m_liteApp(app)
{
    this->setHeaderHidden(true);
#if QT_VERSION >= 0x050000
    this->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    this->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    this->header()->setStretchLastSection(false);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_openBundleAct = new QAction(tr("Open Application"),this);
    m_openInNewWindowAct = new QAction(tr("Open In New Window"),this);
    m_openEditorAct = new QAction(tr("Open File"),this);
    m_newFileAct = new QAction(tr("New File..."),this);
    m_newFileWizardAct = new QAction(tr("New File Wizard..."),this);
    m_renameFileAct = new QAction(tr("Rename File..."),this);
    m_removeFileAct = new QAction(tr("Delete File"),this);

    m_newFolderAct = new QAction(tr("New Folder..."),this);
    m_renameFolderAct = new QAction(tr("Rename Folder..."),this);
    m_removeFolderAct = new QAction(tr("Delete Folder"),this);

#if defined(Q_OS_WIN)
    m_openExplorerAct = new QAction(tr("Show in Explorer"),this);
#elif defined(Q_OS_MAC)
    m_openExplorerAct = new QAction(tr("Show in Finder"),this);
#else
    m_openExplorerAct = new QAction(tr("Show Containing Folder"),this);
#endif

#ifdef Q_OS_WIN
    m_openShellAct = new QAction(tr("Open Command Prompt Here"),this);
#else
    m_openShellAct = new QAction(tr("Open Terminal Here"),this);
#endif

    m_openFolderAct = new QAction(tr("Open Folder..."),this);
    m_reloadFolderAct = new QAction(tr("Reload Folder"),this);
    m_closeFolderAct = new QAction(tr("Close Folder"),this);

    m_closeAllFoldersAct = new QAction(tr("Close All Folders"),this);

    m_copyFileAct = new QAction(tr("Copy"),this);
    m_pasteFileAct = new QAction(tr("Paste"),this);

    connect(m_openBundleAct,SIGNAL(triggered()),this,SLOT(openBundle()));
    connect(m_openInNewWindowAct,SIGNAL(triggered()),this,SLOT(openInNewWindow()));
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
    connect(m_openFolderAct,SIGNAL(triggered()),this,SLOT(openFolder()));
    connect(m_closeFolderAct,SIGNAL(triggered()),this,SLOT(closeFolder()));
    connect(m_reloadFolderAct,SIGNAL(triggered()),this,SLOT(reloadFolder()));
    connect(m_closeAllFoldersAct,SIGNAL(triggered()),this,SLOT(closeAllFolders()));
    connect(m_copyFileAct,SIGNAL(triggered()),this,SLOT(copyFile()));
    connect(m_pasteFileAct,SIGNAL(triggered()),this,SLOT(pasteFile()));
}

QDir BaseFolderView::contextDir() const
{
    if (m_contextInfo.isDir()) {
        return m_contextInfo.filePath();
    }
    return m_contextInfo.dir();
}

QFileInfo BaseFolderView::contextFileInfo() const
{
    return m_contextInfo;
}

void BaseFolderView::openBundle()
{
    if (m_contextInfo.isBundle()) {
        if (QFileInfo("/usr/bin/open").exists()) {
            QStringList args;
            args << m_contextInfo.filePath();
            QProcess::execute(QLatin1String("/usr/bin/open"), args);
        }
    }
}

void BaseFolderView::openInNewWindow()
{
    if (m_contextInfo.isDir()) {
        m_liteApp->fileManager()->openFolderInNewWindow(m_contextInfo.filePath());
    }
}

void BaseFolderView::openEditor()
{
    if (m_contextInfo.isFile()) {
        m_liteApp->fileManager()->openEditor(m_contextInfo.filePath());
    }
}

void BaseFolderView::newFile()
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

void BaseFolderView::newFileWizard()
{
    QString filePath;
    QString projPath;
    QFileInfo info = m_contextInfo;
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

void BaseFolderView::renameFile()
{
    QFileInfo info = m_contextInfo;
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

void BaseFolderView::removeFile()
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
        if (!QFile::remove(info.filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete File"),
                                     tr("Failed to delete the file!"));
        }
    }
}

void BaseFolderView::newFolder()
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

void BaseFolderView::renameFolder()
{
    QFileInfo info = m_contextInfo;
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

void BaseFolderView::removeFolder()
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
        QDir dir = info.dir();
        if (!dir.rmdir(info.fileName())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Delete Folder"),
                                     tr("Failed to delete the folder!"));
        }
    }
}

void BaseFolderView::openExplorer()
{
    FileUtil::openInExplorer(contextFileInfo().filePath());
}

void BaseFolderView::openFolder()
{

}

void BaseFolderView::closeFolder()
{
}

void BaseFolderView::reloadFolder()
{

}

void BaseFolderView::closeAllFolders()
{
}

void BaseFolderView::copyFile()
{
    QFileInfo info = m_contextInfo;
    QClipboard *clip = qApp->clipboard();
    QMimeData *data = new QMimeData();
    data->setUrls(QList<QUrl>() << QUrl::fromLocalFile(info.filePath()));
    clip->setMimeData(data);
}

//static bool copy_dir(const QString &src, const QString &dest)
//{
//    QDir dir(src);
//    QDir destDir(dest);
//    foreach(QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
//        if (info.isFile() && !info.isSymLink()) {
//            QFile::copy(info.filePath(), QFileInfo(destDir,info.fileName()).filePath());
//        } else if (info.isDir()) {
//            destDir.mkdir(info.fileName());
//            if (QFileInfo(destDir,info.fileName()).isDir()) {
//                copy_dir(info.filePath(),QFileInfo(destDir,info.fileName()).filePath());
//            }
//        }
//    }
//    return true;
//}

bool BaseFolderView::copy_dir(const QString &src, const QString &dest_root)
{
    QDir dir(src);
    QDir destDir(dest_root);
    QString name = QFileInfo(src).fileName();
    destDir.mkdir(name);
    if (!destDir.cd(name)) {
        m_liteApp->appendLog("FolderView",QString("copy dir %1 false!").arg(src),true);
        return false;
    }
    foreach(QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
        if (info.isFile() && !info.isSymLink()) {
            bool b = QFile::copy(info.filePath(), QFileInfo(destDir,info.fileName()).filePath());
            if (!b) {
                m_liteApp->appendLog("FolderView",QString("copy file %1=>%2 false!").arg(info.filePath()).arg(destDir.absolutePath()),true);
            }
        } else if (info.isDir()) {
            copy_dir(info.filePath(),destDir.absolutePath());
        }
    }
    return true;
}

void BaseFolderView::pasteFile()
{
    QClipboard *clip = qApp->clipboard();
    const QMimeData *data =  clip->mimeData();
    if (!data) {
        return;
    }
    if (!data->hasUrls()) {
        return;
    }
    QDir dir = contextDir();
    foreach (QUrl url, data->urls()) {
        if (!url.isLocalFile()) {
            continue;
        }
        QString fileName = url.toLocalFile();
        QFileInfo info(fileName);
        if (!info.exists()) {
            continue;
        }
        if (info.isFile()) {
            bool b = QFile::copy(fileName,QFileInfo(dir,info.fileName()).filePath());
            if (!b) {
                m_liteApp->appendLog("FolderView",QString("copy file %1=>%2 false!").arg(fileName).arg(dir.absolutePath()),true);
            }
        } else if(info.isDir()) {
            copy_dir(fileName,dir.absolutePath());
        }
    }
}

bool BaseFolderView::canPasteFile()
{
    QClipboard *clip = qApp->clipboard();
    const QMimeData *data =  clip->mimeData();
    if (!data) {
        return false;
    }
    if (!data->hasUrls()) {
        return false;
    }
    foreach (QUrl url, data->urls()) {
        if (url.isLocalFile()) {
            return true;
        }
    }
    return false;
}

void BaseFolderView::openShell()
{
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    FileUtil::openInShell(env, contextDir().path());
}
