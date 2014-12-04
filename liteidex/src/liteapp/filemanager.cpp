/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
#include "liteapp_global.h"

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

    m_folderListView = new FolderListView(m_liteApp);

    bool bShowHiddenFiles = m_liteApp->settings()->value(LITEAPP_FOLDERSHOWHIDENFILES,false).toBool();
    this->showHideFiles(bShowHiddenFiles);

    m_showHideFilesAct = new QAction(tr("Show Hidden Files"),this);
    m_showHideFilesAct->setCheckable(true);
    if (bShowHiddenFiles) {
        m_showHideFilesAct->setChecked(true);
    }
    connect(m_showHideFilesAct,SIGNAL(triggered(bool)),this,SLOT(showHideFiles(bool)));

    QList<QAction*> actions;
    m_configMenu = new QMenu(tr("Config"));
    m_configMenu->setIcon(QIcon("icon:images/config.png"));
    m_configMenu->addAction(m_showHideFilesAct);
    actions << m_configMenu->menuAction();

    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_folderListView,"folders",tr("Folders"),false,actions);

    m_fileWatcher = new QFileSystemWatcher(this);
    connect(m_fileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(fileChanged(QString)));

    m_maxRecentFiles = m_liteApp->settings()->value("LiteApp/MaxRecentFiles",16).toInt();
    m_newFileDialog = 0;
    m_recentMenu = m_liteApp->actionManager()->loadMenu("menu/recent");
    QAction *cleanAct = new QAction(tr("Clear History"),this);
    m_recentSeparator = m_recentMenu->addSeparator();
    m_recentMenu->addAction(cleanAct);
    foreach (QString key, this->schemeList()) {
        this->updateRecentFileActions(key);
    }
    m_initPath = m_liteApp->settings()->value("FileManager/initpath",QDir::homePath()).toString();
    connect(this,SIGNAL(recentFilesChanged(QString)),this,SLOT(updateRecentFileActions(QString)));
    connect(cleanAct,SIGNAL(triggered()),this,SLOT(cleanRecent()));
    connect(m_folderListView,SIGNAL(aboutToShowContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)),this,SIGNAL(aboutToShowFolderContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo)));
    connect(m_folderListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedFolderView(QModelIndex)));

    m_fileWatcherAutoReload = m_liteApp->settings()->value(LITEAPP_FILEWATCHERAUTORELOAD,false).toBool();

    return true;
}

FileManager::FileManager()
    : m_newFileDialog(0),
      m_folderListView(0),
      m_checkActivated(false)
{
}

FileManager::~FileManager()
{
    m_liteApp->actionManager()->removeMenu(m_recentMenu);
    delete m_fileWatcher;
    m_liteApp->settings()->setValue("FileManager/initpath",m_initPath);
    m_liteApp->settings()->setValue(LITEAPP_FOLDERSHOWHIDENFILES,m_showHideFilesAct->isChecked());
    if (m_newFileDialog) {
        delete m_newFileDialog;
    }
    if (m_folderListView) {
        delete m_folderListView;
    }    
    delete m_configMenu;
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
        types.append(mimeType->globPatterns());
        filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(mimeType->globPatterns().join(" ")));
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
        if (projectMimeTypes.contains(mimeType->type())) {
            types.append(mimeType->globPatterns());
            filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(mimeType->globPatterns().join(" ")));
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
            types.append(mimeType->globPatterns());
            filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(mimeType->globPatterns().join(" ")));
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
//    if (!m_folderListView->rootPathList().isEmpty()) {
//        m_toolWindowAct->setChecked(true);
//    }
}

void FileManager::addFolderList(const QString &folder)
{
    m_folderListView->addRootPath(folder);
    m_toolWindowAct->setChecked(true);
    addRecentFile(folder,"folder");
}

IApplication* FileManager::openFolderInNewWindow(const QString &folder)
{
    IApplication *app = m_liteApp->newInstance(false);
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
    m_liteApp->newInstance(false);
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
       IApplication *app = m_liteApp->newInstance(false);
       app->fileManager()->setFolderList(QStringList() << folder);
   }
}

void FileManager::addFolder()
{
    m_folderListView->addFolder();
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
    pathList.append(LiteApi::getGOROOT(m_liteApp));
    pathList.removeDuplicates();
    m_newFileDialog->setPathList(pathList);
    if (!gopath.isEmpty()) {
        m_newFileDialog->setGopath(gopath);
    }
    m_newFileDialog->setFileLocation(filePath);
    m_newFileDialog->setProjectLocation(projPath);
    m_newFileDialog->updateLocation();

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


IEditor *FileManager::openEditor(const QString &_fileName, bool bActive)
{
    QString fileName = QDir::fromNativeSeparators(QDir::cleanPath(_fileName));

    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(fileName);

    IEditor *editor = m_liteApp->editorManager()->openEditor(fileName,mimeType);
    if (editor && bActive) {
        m_liteApp->editorManager()->setCurrentEditor(editor);
    }
    if (editor) {
        addRecentFile(fileName,"file");
    } else {
        removeRecentFile(fileName,"file");
    }
    return editor;
}

IProject *FileManager::openProject(const QString &_fileName)
{
    QString fileName = QDir::fromNativeSeparators(_fileName);
    QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(fileName);
    IProject *proj = m_liteApp->projectManager()->openProject(fileName,mimeType);
    if (proj) {
        addRecentFile(fileName,"proj");
    } else {
        removeRecentFile(fileName,"proj");
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
       addRecentFile(fileName,scheme);
    } else {
       removeRecentFile(fileName,scheme);
    }
    return proj;
}

QString FileManager::schemeKey(const QString &scheme) const
{
    return QString("Recent1/%1").arg(scheme);
}

QString FileManager::schemeName(const QString &scheme) const
{
	if (scheme == "session") return tr("Session");
    else if (scheme == "proj") return tr("Project");
    else if (scheme == "file") return tr("File");
    else if (scheme == "folder") return tr("Folder");
	else return scheme;
}

QStringList FileManager::recentFiles(const QString &scheme) const
{
    return m_liteApp->settings()->value(schemeKey(scheme)).toStringList();;
}

void FileManager::cleanRecent()
{
    QStringList keyList = this->schemeList();
    foreach(QString key, keyList) {
        m_liteApp->settings()->remove(schemeKey(key));
    }
    foreach(QString key, keyList) {
        emit recentFilesChanged(key);
    }
}

void FileManager::applyOption(QString id)
{
    if (id != OPTION_LITEAPP) {
        return;
    }

    m_fileWatcherAutoReload = m_liteApp->settings()->value(LITEAPP_FILEWATCHERAUTORELOAD,false).toBool();
    m_maxRecentFiles = m_liteApp->settings()->value(LITEAPP_MAXRECENTFILES,16).toInt();
    foreach (QString scheme, this->schemeList()) {
        QString key = schemeKey(scheme);
        QStringList files = m_liteApp->settings()->value(key).toStringList();
        while (files.size() > m_maxRecentFiles) {
            files.removeLast();
        }
        m_liteApp->settings()->setValue(key, files);
        emit recentFilesChanged(scheme);
    }
}

bool FileManager::isShowHideFiles() const
{
    return m_folderListView->model()->filter() & QDir::Hidden;
}

void FileManager::showHideFiles(bool b)
{
    QDir::Filters filters = m_folderListView->model()->filter();
    if (b) {
        filters |= QDir::Hidden;
    } else {
        filters ^= QDir::Hidden;
    }
    m_folderListView->model()->setFilter(filters);
}
void FileManager::doubleClickedFolderView(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    QFileInfo info = m_folderListView->model()->fileInfo(index);
    if (info.isFile()) {
        this->openEditor(info.filePath());
    }
}

void FileManager::updateRecentFileActions(const QString &scheme)
{
    QMenu *menu = m_schemeMenuMap.value(scheme);
    if (!menu) {
		QString name = schemeName(scheme);
        QAction *act = new QAction(name,this);
        m_recentMenu->insertAction(m_recentSeparator,act);
        menu = new QMenu(scheme,m_recentMenu);
        act->setMenu(menu);
        m_schemeMenuMap.insert(scheme,menu);
    }
    if (!menu) {
        return;
    }
    menu->clear();;
    int count = 0;
    foreach (QString file, this->recentFiles(scheme)) {
        if (count++ > m_maxRecentFiles) {
            return;
        }
        QAction *act = new QAction(file,menu);
        menu->addAction(act);
        act->setData(scheme);
        connect(act,SIGNAL(triggered()),this,SLOT(openRecentFile()));
    }
}

void FileManager::openRecentFile()
{
    QAction *act = (QAction*)sender();
    if (!act) {
        return;
    }
    QString scheme = act->data().toString();
    QString fileName = act->text();
    if (scheme.isEmpty()) {
        return;
    }
    if (scheme == "file" || scheme == "proj") {
        this->openFile(fileName);
    } else if (scheme == "folder") {
        this->addFolderList(fileName);
    } else {
        this->openProjectScheme(fileName,scheme);
    }
}

QStringList FileManager::schemeList() const
{
    QStringList list;
    m_liteApp->settings()->beginGroup("Recent1");
    foreach (QString key, m_liteApp->settings()->childKeys() ) {
        list.append(key);
    }
    m_liteApp->settings()->endGroup();
    return list;
}

void FileManager::addRecentFile(const QString &_fileName, const QString &scheme)
{
    QString fileName = QDir::toNativeSeparators(_fileName);
    QString key = schemeKey(scheme);
    QStringList files = m_liteApp->settings()->value(key).toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > m_maxRecentFiles) {
        files.removeLast();
    }

    m_liteApp->settings()->setValue(key, files);

    emit recentFilesChanged(scheme);
}

void FileManager::removeRecentFile(const QString &_fileName, const QString &scheme)
{
    QString fileName = QDir::toNativeSeparators(_fileName);
    QString key = schemeKey(scheme);
    QStringList files = m_liteApp->settings()->value(key).toStringList();
    files.removeAll(fileName);
    m_liteApp->settings()->setValue(key, files);

    emit recentFilesChanged(scheme);
}

void FileManager::updateFileState(const QString &fileName)
{
    if (fileName.isEmpty()) {
        return;
    }
    m_fileStateMap.insert(fileName,QFileInfo(fileName).lastModified());
}

void FileManager::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    QString fileName = editor->filePath();
    if (!fileName.isEmpty()) {
        updateFileState(fileName);
        m_fileWatcher->addPath(fileName);
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
        m_changedFiles.removeAll(fileName);
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
    if (!m_changedFiles.contains(fileName)) {
        m_changedFiles.append(fileName);
    }
    if (wasempty && !m_changedFiles.isEmpty() && !m_checkActivated) {
        m_checkActivated = true;
        QTimer::singleShot(200, this, SLOT(checkForReload()));
    }
}

void FileManager::checkForReload()
{
    int lastReloadRet = QMessageBox::Yes;
    int lastCloseRet = QMessageBox::Yes;
begin:
    QStringList files = m_changedFiles;
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
    if (!m_changedFiles.isEmpty()) {
        goto begin;
    }
    m_checkActivated = false;
}


