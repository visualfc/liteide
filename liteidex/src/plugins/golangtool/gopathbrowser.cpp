/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: gopathbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "gopathbrowser.h"
#include "gopathmodel.h"
#include "liteapi/litefindobj.h"
#include "../filebrowser/createdirdialog.h"
#include "../filebrowser/createfiledialog.h"
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

GopathBrowser::GopathBrowser(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_widget = new QWidget;

    m_toolBar = new QToolBar;
    m_toolBar->setIconSize(QSize(16,16));

    m_syncEditor = new QAction(QIcon(":/images/synceditor.png"),tr("Sync Editor"),this);
    m_syncEditor->setCheckable(true);
    m_syncProject = new QAction(QIcon(":/images/syncproject.png"),tr("Sync Project"),this);
    m_syncProject->setCheckable(true);

    m_startPathLabel = new QLabel;

    m_toolBar->addAction(m_syncEditor);
    m_toolBar->addAction(m_syncProject);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_startPathLabel);

    m_pathTree = new QTreeView;
    m_pathTree->setHeaderHidden(true);
    m_model = new GopathModel(this);
    m_pathTree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pathTree->setModel(m_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_pathTree);
    m_widget->setLayout(layout);

    m_pathList = m_liteApp->settings()->value("golangtool/gopath").toStringList();

    //connect(m_pathTree->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(pathIndexChanged(QModelIndex)));
    connect(m_pathTree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openPathIndex(QModelIndex)));
    LiteApi::IEnvManager* envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    connect(envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(reloadEnv()));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    m_fileMenu = new QMenu(m_widget);
    m_folderMenu = new QMenu(m_widget);

    m_setStartAct = new QAction(tr("Set Activate Project"),this);
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

    m_folderMenu->addAction(m_setStartAct);
    m_folderMenu->addSeparator();
    m_folderMenu->addAction(m_newFileAct);
    m_folderMenu->addAction(m_newFileWizardAct);
    m_folderMenu->addAction(m_newFolderAct);
    m_folderMenu->addAction(m_renameFolderAct);
    m_folderMenu->addAction(m_removeFolderAct);
    m_folderMenu->addSeparator();
    m_folderMenu->addAction(m_openShellAct);
    m_folderMenu->addAction(m_openExplorerAct);

    connect(m_startPathLabel,SIGNAL(linkActivated(QString)),this,SLOT(expandStartPath(QString)));
    connect(m_syncEditor,SIGNAL(triggered(bool)),this,SLOT(syncEditor(bool)));
    connect(m_syncProject,SIGNAL(triggered(bool)),this,SLOT(syncProject(bool)));
    connect(m_setStartAct,SIGNAL(triggered()),this,SLOT(setActivate()));
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

    connect(m_pathTree,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(treeViewContextMenuRequested(QPoint)));

    bool b = m_liteApp->settings()->value("GolangTool/synceditor",true).toBool();
    if (b) {
        m_syncEditor->toggle();
    }
    b = m_liteApp->settings()->value("GolangTool/syncproject",false).toBool();
    if (b) {
        m_syncProject->toggle();
    }
    m_startPathLabel->setText("null project");
}

GopathBrowser::~GopathBrowser()
{
    m_liteApp->settings()->setValue("GolangTool/synceditor",m_syncEditor->isChecked());
    m_liteApp->settings()->setValue("GolangTool/syncproject",m_syncProject->isChecked());
    m_liteApp->settings()->setValue("golangtool/gopath",m_pathList);
    delete m_widget;
}

QDir GopathBrowser::contextDir() const
{
    if (m_contextInfo.isDir()) {
        return m_contextInfo.filePath();
    }
    return m_contextInfo.dir();
}

QFileInfo GopathBrowser::contextFileInfo() const
{
    return m_contextInfo;
}

void GopathBrowser::setActivate()
{
    setStartIndex(m_contextIndex);
}

void GopathBrowser::openEditor()
{
    if (m_contextInfo.isFile()) {
        m_liteApp->fileManager()->openEditor(m_contextInfo.filePath());
    }
}

void GopathBrowser::newFile()
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

void GopathBrowser::newFileWizard()
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

void GopathBrowser::renameFile()
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
        if (!QFile::rename(info.filePath(),QFileInfo(dir,fileName).filePath())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename File"),
                                     tr("Failed to rename the file!"));
        }
    }
}

void GopathBrowser::removeFile()
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

void GopathBrowser::newFolder()
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

void GopathBrowser::renameFolder()
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
        if (!dir.rename(info.fileName(),folderName)) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Rename Folder"),
                                     tr("Failed to rename the folder!"));
        }
    }
}

void GopathBrowser::removeFolder()
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
        if (!dir.rmdir(info.fileName())) {
            QMessageBox::information(m_liteApp->mainWindow(),tr("Remove Folder"),
                                     tr("Failed to remove the folder!"));
        }
    }
}

void GopathBrowser::openExplorer()
{
    QDir dir = contextDir();
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir.path()));
}

void GopathBrowser::openShell()
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

QWidget *GopathBrowser::widget() const
{
    return m_widget;
}

void GopathBrowser::treeViewContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = m_pathTree->indexAt(pos);
    if (!index.isValid()) {
        return;
    }
    PathNode *node = m_model->nodeFromIndex(index);
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
        contextMenu->popup(m_pathTree->mapToGlobal(pos));
    }
}

void GopathBrowser::addPathList(const QString &path)
{
    QStringList pathList = m_pathList;
    pathList.append(path);
    this->setPathList(pathList);
}

void GopathBrowser::setPathList(const QStringList &pathList)
{
    m_pathList = pathList;
    QStringList allPathList = systemGopathList()+m_pathList;
    m_model->setPathList(allPathList);
    m_pathTree->setCurrentIndex(m_model->startIndex());
    m_pathTree->expand(m_model->startIndex());
    LiteApi::IEnvManager* envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    LiteApi::IEnv *env = envManager->currentEnv();
    if (env) {
#ifdef Q_OS_WIN
    env->environment().insert("LITEIDE_GOPATH",allPathList.join(";"));
#else
    env->environment().insert("LITEIDE_GOPATH",allPathList.join(":"));
#endif
    }
    currentEditorChanged(m_liteApp->editorManager()->currentEditor());
}

QStringList GopathBrowser::pathList() const
{
    return m_pathList;
}

QStringList GopathBrowser::systemGopathList() const
{
    LiteApi::IEnvManager* envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    QString gopath= envManager->currentEnvironment().value("GOPATH");
    if (gopath.isEmpty()) {
        gopath = envManager->currentEnvironment().value("GOROOT");
    }
    QStringList list;
 #ifdef Q_OS_WIN
     list = gopath.split(";",QString::SkipEmptyParts);
 #else
     list = gopath.split(":",QString::SkipEmptyParts);
 #endif
     return list;
}

void GopathBrowser::reloadEnv()
{
    this->setPathList(m_pathList);
}

void GopathBrowser::setStartIndex(const QModelIndex &index)
{
    QModelIndex oldIndex = m_model->startIndex();
    if (oldIndex != index) {
        m_model->setStartIndex(index);
        m_pathTree->update(oldIndex);
        m_pathTree->update(index);
        emit startPathChanged(m_model->filePath(index));
        PathNode *node = m_model->nodeFromIndex(index);
        if (node) {
            m_startPathLabel->setText(QString("<p><a href file://%1>%2</p>").arg(node->path()).arg(node->text()));
            m_startPathLabel->setToolTip(node->path());
        }
    }
}

QString GopathBrowser::startPath() const
{
    QModelIndex index = m_model->startIndex();
    return m_model->filePath(index);
}

void GopathBrowser::pathIndexChanged(const QModelIndex & index)
{
    PathNode *node = m_model->nodeFromIndex(index);
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

void GopathBrowser::openPathIndex(const QModelIndex &index)
{
    PathNode *node = m_model->nodeFromIndex(index);
    if (!node) {
        return;
    }
    if (node->isDir()) {
        this->setStartIndex(index);
    } else if (node->isFile()) {
        if (m_syncProject->isChecked()) {
            this->setStartIndex(index.parent());
        }
        m_liteApp->fileManager()->openEditor(node->path(),true);
    }
}

void GopathBrowser::currentEditorChanged(LiteApi::IEditor* editor)
{
    if (!m_syncEditor->isChecked()) {
        return;
    }
    if (editor && !editor->filePath().isEmpty()) {
        QModelIndex index = m_model->findPath(editor->filePath());
        if (index.isValid()) {
            m_pathTree->setCurrentIndex(index);
            m_pathTree->scrollTo(index,QAbstractItemView::EnsureVisible);
            if (m_syncProject->isChecked()) {
                setStartIndex(index.parent());
            }
        }
    }
}

void GopathBrowser::syncEditor(bool b)
{
    if (!b) {
        m_syncProject->setChecked(false);
    }
}

void GopathBrowser::syncProject(bool b)
{
    if (b) {
        m_syncEditor->setChecked(true);
    }
}

void GopathBrowser::expandStartPath(QString)
{
    m_pathTree->scrollTo(m_model->startIndex(),QAbstractItemView::EnsureVisible);
}
