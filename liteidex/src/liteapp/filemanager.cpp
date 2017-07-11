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
// Module: filemanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filemanager.h"
#include "newfiledialog.h"
#include "fileutil/fileutil.h"
#include "liteenvapi/liteenvapi.h"
#include "folderview/folderlistview.h"
#include "folderview/multifolderview.h"
#include "liteapp_global.h"

#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegExp>
#include <QMenu>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QTimer>
#include <QDesktopServices>
#include <QDir>
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

bool FileManager::initWithApp(IApplication *app)
{
    if (!IFileManager::initWithApp(app)) {
        return false;
    }

//#ifdef Q_OS_MAC
//    m_folderListView = new FolderListView(true,m_liteApp);
//#else
//    m_folderListView = new FolderListView(false,m_liteApp);
//#endif

    m_folderListView = new MultiFolderView(m_liteApp);

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

    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_folderListView,"folders",tr("Folders"),false,actions);

    m_fileWatcher = new QFileSystemWatcher(this);
    connect(m_fileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(fileChanged(QString)));

    m_newFileDialog = 0;
    m_initPath = m_liteApp->settings()->value("FileManager/initpath",QDir::homePath()).toString();
    connect(m_folderListView,SIGNAL(aboutToShowContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)),this,SIGNAL(aboutToShowFolderContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)));
    connect(m_folderListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedFolderView(QModelIndex)));
    connect(m_folderListView,SIGNAL(enterKeyPressed(QModelIndex)),this,SLOT(enterKeyPressedFolderView(QModelIndex)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    m_fileWatcherAutoReload = m_liteApp->settings()->value(LITEAPP_FILEWATCHERAUTORELOAD,false).toBool();

    connect(m_syncEditorAct,SIGNAL(triggered(bool)),this,SLOT(triggeredSyncEditor(bool)));
    bool b = m_liteApp->settings()->value("FileManager/synceditor",false).toBool();
    if (b) {
        m_syncEditorAct->setChecked(true);
    }

    return true;
}

FileManager::FileManager()
    : m_newFileDialog(0),
      m_folderListView(0),
      m_checkBlockActivated(false),
      m_checkOnFocusChange(false)
{
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onApplicationFocusChange()));
}

FileManager::~FileManager()
{
    delete m_fileWatcher;
    m_liteApp->settings()->setValue("FileManager/initpath",m_initPath);
    m_liteApp->settings()->setValue("FileManager/synceditor",m_syncEditorAct->isChecked());
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSHOWHIDENFILES,m_showHideFilesAct->isChecked());
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSHOWDETAILS,m_showDetailsAct->isChecked());
    if (m_newFileDialog) {
        delete m_newFileDialog;
    }
    if (m_folderListView) {
        delete m_folderListView;
    }    
    delete m_filterMenu;
}

bool FileManager::findProjectTargetInfo(const QString &fileName, QMap<QString,QString>& targetInfo) const
{
    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(fileName);
    if (m_liteApp->projectManager()->mimeTypeList().contains(mimeType)) {
        QList<IProjectFactory*> factoryList = m_liteApp->projectManager()->factoryList();
        foreach(LiteApi::IProjectFactory *factory, factoryList) {
            if (factory->mimeTypes().contains(mimeType)) {
                bool ret = factory->findTargetInfo(fileName,mimeType,targetInfo);
                if (ret) {
                    return true;
                }
            }
        }
    }
    return false;
}


QString FileManager::openAllTypeFilter() const
{
    QStringList types;
    QStringList filter;
    foreach (IMimeType *mimeType, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        QStringList patterns = mimeType->allPatterns();
        types.append(patterns);
        filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(patterns.join(" ")));
    }
    types.removeDuplicates();
    filter.removeDuplicates();
    if (!types.isEmpty()) {
        QString all = QString(tr("All Support Files (%1)")).arg(types.join(" "));
        filter.insert(0,all);
    }
    filter.append(tr("All Files (*)"));
    return filter.join(";;");
}

QString FileManager::openProjectTypeFilter() const
{
    QStringList types;
    QStringList filter;
    QStringList projectMimeTypes = m_liteApp->projectManager()->mimeTypeList();
    foreach (IMimeType *mimeType, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        QStringList patterns = mimeType->allPatterns();
        if (projectMimeTypes.contains(mimeType->type())) {
            types.append(patterns);
            filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(patterns.join(" ")));
        }
    }
    types.removeDuplicates();
    filter.removeDuplicates();
    if (!types.isEmpty()) {
        QString all = QString(tr("All Support Files (%1)")).arg(types.join(" "));
        filter.insert(0,all);
    }
    filter.append(tr("All Files (*)"));
    return filter.join(";;");
}

QString FileManager::openEditorTypeFilter() const
{
    QStringList types;
    QStringList filter;
    QStringList projectMimeTypes = m_liteApp->editorManager()->mimeTypeList();
    foreach (IMimeType *mimeType, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        if (projectMimeTypes.contains(mimeType->type())) {
            QStringList patterns = mimeType->allPatterns();
            types.append(patterns);
            filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(patterns.join(" ")));
        }
    }
    types.removeDuplicates();
    filter.removeDuplicates();
    if (!types.isEmpty()) {
        QString all = QString(tr("All Support Files (%1)")).arg(types.join(" "));
        filter.insert(0,all);
    }
    filter.append(tr("All Files (*)"));
    return filter.join(";;");
}

QStringList FileManager::folderList() const
{
    return m_folderListView->rootPathList();
}

void FileManager::setFolderList(const QStringList &folders)
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

void FileManager::addFolderList(const QString &folder)
{
    if (!m_folderListView->addRootPath(folder)) {
        return;
    }
    m_toolWindowAct->setChecked(true);
    m_liteApp->recentManager()->addRecent(folder,"folder");
    m_folderListView->expandFolder(folder,true);
}

IApplication* FileManager::openFolderInNewWindow(const QString &folder)
{
    IApplication *app = m_liteApp->newInstance("dir:"+QDir(folder).dirName());
    app->fileManager()->setFolderList(QStringList() << folder);
    return app;
}

void FileManager::newFile()
{
    QString projPath;
    QString filePath;
    IProject *project = m_liteApp->projectManager()->currentProject();
    if (project) {
        QFileInfo info(project->filePath());
        if (info.isDir()) {
            projPath = info.filePath();
        } else {
            projPath = info.path();
        }
    }
    IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor && !editor->filePath().isEmpty()) {
        filePath = QFileInfo(editor->filePath()).absolutePath();
    }
    if (filePath.isEmpty()) {
#if QT_VERSION >= 0x050000
        filePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
        filePath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
    }
    if (projPath.isEmpty()) {
#if QT_VERSION >= 0x050000
        projPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
        projPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif
    }
    execFileWizard(projPath,filePath);
}

void FileManager::openFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(m_liteApp->mainWindow(),
           tr("Open Project or File"), m_initPath,openAllTypeFilter());
    if (fileNames.isEmpty())
        return;
    foreach (QString fileName, fileNames) {
        if (openFile(fileName)) {
            m_initPath = QFileInfo(fileName).canonicalPath();
        }
    }
}

void FileManager::openFolder()
{
     QString folder = QFileDialog::getExistingDirectory(m_liteApp->mainWindow(),
           tr("Select a folder:"), m_initPath);
    if (!folder.isEmpty()) {
        QDir dir(folder);
        if (dir.cdUp()) {
            m_initPath = dir.path();
        }
        this->addFolderList(folder);
    }
}

void FileManager::newInstance()
{
    m_liteApp->newInstance("");
}

void FileManager::openFolderNewWindow()
{
    QString folder = QFileDialog::getExistingDirectory(m_liteApp->mainWindow(),
          tr("Select a folder:"), m_initPath);
   if (!folder.isEmpty()) {
       QDir dir(folder);
       if (dir.cdUp()) {
           m_initPath = dir.path();
       }
       this->openFolderInNewWindow(folder);
   }
}

void FileManager::closeAllFolders()
{
    m_folderListView->closeAllFolders();
}

void FileManager::openEditors()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(m_liteApp->mainWindow(),
           tr("Open Files"), m_initPath,openEditorTypeFilter());
    if (fileNames.isEmpty())
        return;
    foreach (QString fileName, fileNames) {
        if (openEditor(fileName)) {
            m_initPath = QFileInfo(fileName).canonicalPath();
        }
    }
}

void FileManager::openProjects()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(m_liteApp->mainWindow(),
           tr("Open Project"), m_initPath,openProjectTypeFilter());
    if (fileNames.isEmpty())
        return;
    foreach (QString fileName, fileNames) {
        if (openProject(fileName)) {
            m_initPath = QFileInfo(fileName).canonicalPath();
        }
    }
}

void FileManager::execFileWizard(const QString &projPath, const QString &filePath, const QString &gopath)
{
    if (!m_newFileDialog) {
        m_newFileDialog = new NewFileDialog(m_liteApp->mainWindow());
        m_newFileDialog->loadTemplate(m_liteApp->resourcePath()+"/liteapp/template");
    }
    QStringList pathList = LiteApi::getGOPATH(m_liteApp,false);
    //pathList.append(LiteApi::getGOROOT(m_liteApp));
    pathList.removeDuplicates();
    m_newFileDialog->setPathList(pathList);
    if (!gopath.isEmpty()) {
        m_newFileDialog->setGopath(gopath);
    }
    m_newFileDialog->setFileLocation(filePath);
    m_newFileDialog->setProjectLocation(projPath);
    m_newFileDialog->updateLocation();
    if (pathList.isEmpty() && gopath.isEmpty()) {
        m_newFileDialog->setGopath(projPath);
    }

    if (m_newFileDialog->exec() == QDialog::Accepted) {
        //emit fileWizardFinished(m_newFileDialog->openPath(),m_newFileDialog->f)
        emit fileWizardFinished(m_newFileDialog->type(),m_newFileDialog->scheme(),m_newFileDialog->openPath());
        QMessageBox::StandardButton ret;
        ret = QMessageBox::question(m_liteApp->mainWindow(), tr("LiteIDE"),
                                    tr("Project '%1' has been created.\n"
                                       "Do you want to open it now?")
                                    .arg(m_newFileDialog->openFiles().join(" ")),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                    QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
            QString scheme = m_newFileDialog->scheme();
            if (scheme == "folder") {
                this->addFolderList(m_newFileDialog->openPath());
            }
            foreach(QString file, m_newFileDialog->openFiles()) {
                this->openFile(file);
            }
        }
    }
}

bool FileManager::openFile(const QString &fileName)
{
    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(fileName);
    if (m_liteApp->projectManager()->mimeTypeList().contains(mimeType)) {
        return openProject(fileName) != 0;
    } else {
        return openEditor(fileName) != 0;
    }
    return false;
}

IEditor *FileManager::createEditor(const QString &contents, const QString &mimeType)
{
    foreach(LiteApi::IEditorFactory *factory, m_liteApp->editorManager()->factoryList()) {
        if (factory->mimeTypes().contains(mimeType)) {
            LiteApi::IEditor *editor = factory->create(contents,mimeType);
            if (editor) {
                return editor;
            }
        }
    }
    return NULL;
}

IEditor *FileManager::createEditor(const QString &_fileName)
{
    QString fileName = QDir::fromNativeSeparators(_fileName);

    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(fileName);

    foreach(LiteApi::IEditorFactory *factory, m_liteApp->editorManager()->factoryList()) {
        if (factory->mimeTypes().contains(mimeType)) {
            LiteApi::IEditor *editor = factory->open(fileName,mimeType);
            if (editor) {
                return editor;
            }
        }
    }
    return NULL;
}


IEditor *FileManager::openEditor(const QString &_fileName, bool bActive, bool ignoreNavigationHistory)
{
    QString fileName = QDir::fromNativeSeparators(QDir::cleanPath(_fileName));

    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(fileName);

    IEditor *editor = m_liteApp->editorManager()->openEditor(fileName,mimeType);
    if (editor && bActive) {
        m_liteApp->editorManager()->setCurrentEditor(editor,ignoreNavigationHistory);
    }
    if (editor) {
        m_liteApp->recentManager()->addRecent(fileName,"file");
    } else {
        m_liteApp->recentManager()->removeRecent(fileName,"file");
    }
    return editor;
}

IProject *FileManager::openProject(const QString &_fileName)
{
    QString fileName = QDir::fromNativeSeparators(_fileName);
    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(fileName);
    IProject *proj = m_liteApp->projectManager()->openProject(fileName,mimeType);
    if (proj) {
        m_liteApp->recentManager()->addRecent(fileName,"proj");
    } else {
        m_liteApp->recentManager()->removeRecent(fileName,"proj");
    }
    return proj;
}

//IApplication* FileManager::openFolderEx(const QString &folder)
//{
//    QDir dir(folder);
//    if (!dir.exists()) {
//        return m_liteApp;
//    }
//    if (m_folderWidget->rootPathList().isEmpty()) {
//        m_folderWidget->setRootPath(folder);
//    } else {
//        if (m_liteApp->settings()->value(LITEAPP_OPTNFOLDERINNEWWINDOW,true).toBool()) {
//            return this->openFolderInNewWindow(folder);
//        } else {
//            m_folderWidget->setRootPath(folder);
//        }
//    }
//    m_toolWindowAct->setChecked(true);
//    addRecentFile(folder,"folder");
//    return m_liteApp;
//}

IProject *FileManager::openProjectScheme(const QString &_fileName, const QString &scheme)
{
    QString fileName = QDir::fromNativeSeparators(_fileName);
    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByScheme(scheme);
    if (mimeType.isEmpty()) {
        return 0;
    }
    IProject *proj = m_liteApp->projectManager()->openProject(QDir::fromNativeSeparators(fileName),mimeType);
    if (proj) {
        m_liteApp->recentManager()->addRecent(fileName,scheme);
    } else {
        m_liteApp->recentManager()->removeRecent(fileName,scheme);
    }
    return proj;
}

void FileManager::applyOption(QString id)
{
    if (id != OPTION_LITEAPP) {
        return;
    }

    m_fileWatcherAutoReload = m_liteApp->settings()->value(LITEAPP_FILEWATCHERAUTORELOAD,false).toBool();
}

bool FileManager::isShowHideFiles() const
{
    return m_folderListView->filter() & QDir::Hidden;
}

void FileManager::showHideFiles(bool b)
{
    QDir::Filters filters = m_folderListView->filter();
    if (b) {
        filters |= QDir::Hidden;
    } else {
        filters ^= QDir::Hidden;
    }
    m_folderListView->setFilter(filters);
}

void FileManager::doubleClickedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QFileInfo info = m_folderListView->fileInfo(index);
    if (info.isFile()) {
        this->openEditor(info.filePath());
    }
}

void FileManager::enterKeyPressedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QFileInfo info = m_folderListView->fileInfo(index);
    if (info.isFile()) {
        this->openEditor(info.filePath());
    } else {
        m_folderListView->setExpanded(index,!m_folderListView->isExpanded(index));
    }
}

void FileManager::currentEditorChanged(IEditor *editor)
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

void FileManager::triggeredSyncEditor(bool b)
{
    if (b) {
        this->currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    }
}

void FileManager::updateFileState(const QString &fileName)
{
    if (fileName.isEmpty()) {
        return;
    }
    m_fileStateMap.insert(fileName,QFileInfo(fileName).lastModified());
    if (!m_fileWatcher->files().contains(fileName)) {
        m_fileWatcher->addPath(fileName);;
    }
}

void FileManager::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    QString fileName = editor->filePath();
    if (!fileName.isEmpty()) {
        updateFileState(fileName);
    }
}

void FileManager::editorAboutToClose(LiteApi::IEditor *editor)
{    
    if (!editor) {
        return;
    }
    QString fileName = editor->filePath();
    if (!fileName.isEmpty()) {
        m_fileStateMap.remove(fileName);
        m_changedFiles.remove(fileName);
        m_fileWatcher->removePath(fileName);
    }
}

void FileManager::editorSaved(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    updateFileState(editor->filePath());
}

void FileManager::fileChanged(QString fileName)
{
    const bool wasempty = m_changedFiles.isEmpty();
    if (m_fileStateMap.contains(fileName)) {
        m_changedFiles.insert(fileName);
    }

    if (wasempty && !m_changedFiles.isEmpty()) {
        QTimer::singleShot(200, this, SLOT(checkForReload()));
    }
}

void FileManager::onApplicationFocusChange()
{
    if (!m_checkOnFocusChange)
        return;
    m_checkOnFocusChange = false;
    checkForReload();
}

void FileManager::checkForReload()
{
    if (m_changedFiles.isEmpty()) {
        return;
    }

    if (this->m_checkBlockActivated)
        return;
    if (QApplication::activeModalWidget()) {
        // We do not want to prompt for modified file if we currently have some modal dialog open.
        // There is no really sensible way to get notified globally if a window closed,
        // so just check on every focus change.
        m_checkOnFocusChange = true;
        return;
    }

    this->m_checkBlockActivated = true;

    int lastReloadRet = QMessageBox::Yes;
    int lastCloseRet = QMessageBox::Yes;
    QStringList files = m_changedFiles.toList();
    m_changedFiles.clear();
    foreach (QString fileName, files) {
        if (!QFile::exists(fileName)) {
            //remove
            if (m_fileStateMap.contains(fileName)) {
                if (!fileName.isEmpty()) {
                    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,false);
                    if (editor) {
                        // The file has been deleted.
                        // If the buffer is modified, ask the user what he wants to do.
                        // Otherwise, apply the default action : close the editor.
                        int ret = QMessageBox::Yes;
                        if (lastCloseRet != QMessageBox::YesToAll) {
                            if (m_fileWatcherAutoReload) {
                                if (editor->isModified() ) {
                                    QString text = QString(tr("%1\nThis file has been deleted from the drive,\n"
                                                              "but you have unsaved modifications in your LiteIDE editor.\n"
                                                              "\nDo you want to close the editor?"
                                                              "\nAnswering \"Yes\" will discard your unsaved changes.")).arg(fileName);
                                    ret = QMessageBox::question(m_liteApp->mainWindow(),tr("LiteIDE X"),text,QMessageBox::YesToAll|QMessageBox::Yes|QMessageBox::No);
                                }
                            } else {
                                QString text = QString(tr("%1\nThis file has been deleted from the drive.\n"
                                                          "\nDo you want to close the editor?")).arg(fileName);
                                ret = QMessageBox::question(m_liteApp->mainWindow(),tr("LiteIDE X"),text,QMessageBox::YesToAll|QMessageBox::Yes|QMessageBox::No);
                            }
                        }

                        if (ret == QMessageBox::Yes || ret == QMessageBox::YesToAll) {
                            m_liteApp->editorManager()->closeEditor(editor);
                            m_liteApp->appendLog("EditorManager",fileName+" remove",false);
                        }
                        if (ret == QMessageBox::YesToAll) {
                            lastCloseRet = QMessageBox::YesToAll;
                        }
                    }
                }
            }
        } else {
            if (m_fileStateMap.contains(fileName)) {
                LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,true);
                if (editor) {
                    // The file has been modified.
                    // If the buffer is modified, ask the user what he wants to do.
                    // Otherwise, apply the default action : reload the new content in the editor.
                    QDateTime lastModified = QFileInfo(fileName).lastModified();
                    QDateTime modified = m_fileStateMap.value(fileName);
                    if (!m_fileWatcher->files().contains(fileName)) {
                        m_fileWatcher->addPath(fileName);;
                    }
                    if (lastModified > modified) {
                        int ret = QMessageBox::Yes;
                        if (lastReloadRet != QMessageBox::YesToAll) {
                            if (m_fileWatcherAutoReload) {
                                if (editor->isModified()) {
                                    QString text = QString(tr("%1\nThis file has been modified on the drive,\n"
                                        "but you have unsaved modifications in your LiteIDE editor.\n"
                                        "\nDo you want to reload the file from disk?"
                                        "\nAnswering \"Yes\" will discard your unsaved changes.")).arg(fileName);
                                    ret = QMessageBox::question(m_liteApp->mainWindow(),tr("LiteIDE X"),text,QMessageBox::YesToAll|QMessageBox::Yes|QMessageBox::No);
                                }
                            } else {
                                QString text = QString(tr("%1\nThis file has been modified on the drive.\n"
                                    "\nDo you want to reload the file from disk?")).arg(fileName);
                                ret = QMessageBox::question(m_liteApp->mainWindow(),tr("LiteIDE X"),text,QMessageBox::YesToAll|QMessageBox::Yes|QMessageBox::No);
                            }
                        }
                        if (ret == QMessageBox::YesToAll || ret == QMessageBox::Yes) {
                            // If the file modification is the result of an internal Ctrl+S, do not reload
                            QDateTime lastModified = QFileInfo(fileName).lastModified();
                            QDateTime modified = m_fileStateMap.value(fileName);
                            if (lastModified != modified) {
                                editor->reload();
                                m_fileStateMap.insert(fileName,lastModified);
                                m_liteApp->appendLog("EditorManager",fileName+" reload",false);

                            }
                        }
                        if (ret == QMessageBox::YesToAll) {
                            lastReloadRet = QMessageBox::YesToAll;
                        }
                    }
                }
            }
        }
    }
    m_checkBlockActivated = false;
    QTimer::singleShot(200, this, SLOT(checkForReload()));
}


