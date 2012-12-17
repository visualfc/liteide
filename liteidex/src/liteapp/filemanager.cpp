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
// Module: filemanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filemanager.h"
#include "newfiledialog.h"
#include "fileutil/fileutil.h"
#include "liteenvapi/liteenvapi.h"
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

    m_fileWatcher = new QFileSystemWatcher(this);
    connect(m_fileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(fileChanged(QString)));

    m_maxRecentFiles = m_liteApp->settings()->value("LiteApp/MaxRecentFiles",16).toInt();
    m_newFileDialog = 0;
    m_recentMenu = m_liteApp->actionManager()->loadMenu("menu/recent");
    QAction *cleanAct = new QAction(tr("Clean All"),this);
    m_recentSeparator = m_recentMenu->addSeparator();
    m_recentMenu->addAction(cleanAct);
    foreach (QString key, this->schemeList()) {
        this->updateRecentFileActions(key);
    }
    m_initPath = m_liteApp->settings()->value("FileManager/initpath",QDir::homePath()).toString();
    connect(this,SIGNAL(recentFilesChanged(QString)),this,SLOT(updateRecentFileActions(QString)));
    connect(cleanAct,SIGNAL(triggered()),this,SLOT(cleanRecent()));
    return true;
}

FileManager::FileManager()
    : m_newFileDialog(0),
      m_checkActivated(false)
{
}

FileManager::~FileManager()
{
    qDeleteAll(m_schemeMenuMap);
    m_liteApp->actionManager()->removeMenu(m_recentMenu);
    delete m_fileWatcher;
    m_liteApp->settings()->setValue("FileManager/initpath",m_initPath);
    if (m_newFileDialog) {
        delete m_newFileDialog;
    }
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
        filePath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    }
    if (projPath.isEmpty()) {
        filePath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
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
           tr("Open Folder"), m_initPath);
    if (!folder.isEmpty()) {
        QDir dir(folder);
        if (dir.cdUp()) {
            m_initPath = dir.path();
        }
        this->openFolderProject(folder);
    }
}

void FileManager::newInstance()
{
    m_liteApp->newInstance(false);
}

void FileManager::openFolderNewInstance()
{
    QString folder = QFileDialog::getExistingDirectory(m_liteApp->mainWindow(),
          tr("Open Folder With New Instance"), m_initPath);
   if (!folder.isEmpty()) {
       QDir dir(folder);
       if (dir.cdUp()) {
           m_initPath = dir.path();
       }
       IApplication *app = m_liteApp->newInstance(false);
       app->fileManager()->openFolderProject(folder);
   }
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
    QStringList pathList = LiteApi::getGopathList(m_liteApp,false);
    pathList.append(LiteApi::getGoroot(m_liteApp));
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
                                    tr("Project '%1' is created.\n"
                                       "Do you want to load?")
                                    .arg(m_newFileDialog->openFiles().join(" ")),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                    QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
            QString scheme = m_newFileDialog->scheme();
            if (!scheme.isEmpty()) {
                m_liteApp->fileManager()->openProjectScheme(m_newFileDialog->openPath(),scheme);
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

IProject *FileManager::openFolderProject(const QString &folder)
{
    IProject *project = m_liteApp->projectManager()->openFolder(folder);
    if (project) {
        addRecentFile(folder,"folder");
    } else {
        removeRecentFile(folder,"folder");
    }
    return project;
}

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

void FileManager::updateRecentFileActions(const QString &scheme)
{
    QMenu *menu = m_schemeMenuMap.value(scheme);
    if (!menu) {
        QAction *act = new QAction(scheme,this);
        m_recentMenu->insertAction(m_recentSeparator,act);
        menu = new QMenu(scheme);
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
        return;
    }
    this->openProjectScheme(fileName,scheme);
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
    if (wasempty && !m_changedFiles.isEmpty()) {
        QTimer::singleShot(200, this, SLOT(checkForReload()));
    }
}

void FileManager::checkForReload()
{
    if (m_checkActivated) {
        return;
    }
    m_checkActivated = true;
    foreach (QString fileName, m_changedFiles) {
        if (!QFile::exists(fileName)) {
            //remove
            if (m_fileStateMap.contains(fileName)) {
                if (!fileName.isEmpty()) {
                    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,false);
                    if (editor) {
                        QString text = QString(tr("%1\nThis file has been removed. Do you want save to file or close editor?")).arg(fileName);
                        int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("LiteIDE X"),text,QMessageBox::Save |QMessageBox::Close | QMessageBox::Cancel,QMessageBox::Save);
                        if (ret == QMessageBox::Save) {
                            if (m_liteApp->editorManager()->saveEditor(editor)) {
                                m_fileWatcher->addPath(fileName);
                            }
                        } else if (ret == QMessageBox::Close) {
                            m_liteApp->editorManager()->closeEditor(editor);
                        }
                    }
                }
            }
        } else {
            if (m_fileStateMap.contains(fileName)) {
                LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,true);
                if (editor) {
                    QDateTime lastModified = QFileInfo(fileName).lastModified();
                    QDateTime modified = m_fileStateMap.value(fileName);
                    if (lastModified > modified) {
                        QString text = QString(tr("%1\nThis file has been modified outside of the liteide. Do you want to reload it?")).arg(fileName);
                        int ret = QMessageBox::question(m_liteApp->mainWindow(),"LiteIDE X",text,QMessageBox::Yes|QMessageBox::No);
                        if (ret == QMessageBox::Yes) {
                            editor->reload();
                        }
                    }
                }
            }
        }
    }
    m_changedFiles.clear();
    m_checkActivated = false;
}


