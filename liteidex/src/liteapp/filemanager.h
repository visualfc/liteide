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
// Module: filemanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "liteapi/liteapi.h"
#include <QModelIndex>

using namespace LiteApi;

class QFileSystemWatcher;
class NewFileDialog;
class FolderListView;
class MultiFolderView;

class FileManager : public IFileManager
{
    Q_OBJECT
public:
    FileManager();
    virtual bool initWithApp(IApplication *app);
    virtual ~FileManager();
public:
    virtual void execFileWizard(const QString &projPath, const QString &filePath, const QString &gopath = QString());
    virtual bool openFile(const QString &fileName);
    virtual IEditor *openEditor(const QString &fileName, bool bActive = true, bool ignoreNavigationHistory = false);
    virtual IEditor *createEditor(const QString &contents, const QString &mimeType);
    virtual IEditor *createEditor(const QString &fileName);
    virtual IProject *openProject(const QString &fileName);
    virtual IProject *openProjectScheme(const QString &fileName, const QString &scheme);

    virtual QStringList schemeList() const;
    virtual void addRecentFile(const QString &fileName, const QString &scheme);
    virtual void removeRecentFile(const QString &fileName, const QString &scheme);
    virtual QStringList recentFiles(const QString &scheme) const;
    virtual bool findProjectTargetInfo(const QString &fileName, QMap<QString,QString>& targetInfo) const;
    //virtual IApplication* openFolderEx(const QString &folder);
    virtual QStringList folderList() const;
    virtual void setFolderList(const QStringList &folders);
    virtual void addFolderList(const QString &folder);
    virtual IApplication* openFolderInNewWindow(const QString &folder);
public:
    QString openAllTypeFilter() const;
    QString openProjectTypeFilter() const;
    QString openEditorTypeFilter() const;
    bool isShowHideFiles() const;
protected:
    QString schemeKey(const QString &scheme) const;
	QString schemeName(const QString &scheme) const;
    void updateFileState(const QString &fileName);
public slots:
    void updateRecentFileActions(const QString &scheme);
    void openRecentFile();
    void clearRecentFile();
    void cleanAllRecent();
    void newFile();
    void openFiles();
    void openFolder();
    void openFolderNewWindow();
    void closeAllFolders();
    void newInstance();
    void openEditors();
    void openProjects();
    void fileChanged(QString);
    void editorSaved(LiteApi::IEditor*);
    void editorCreated(LiteApi::IEditor*);
    void editorAboutToClose(LiteApi::IEditor*);
    void checkForReload();
    void applyOption(QString);
    void showHideFiles(bool);
    void activatedFolderView(const QModelIndex &index);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void triggeredSyncEditor(bool b);
protected:
    NewFileDialog        *m_newFileDialog;
    MultiFolderView     *m_folderListView;
    QFileSystemWatcher   *m_fileWatcher;
    QMap<QString,QDateTime> m_fileStateMap;
    QStringList          m_changedFiles;
    bool                 m_checkActivated;
    bool                 m_fileWatcherAutoReload;
    QAction              *m_recentSeparator;
    QMap<QString,QMenu*> m_schemeMenuMap;
    int         m_maxRecentFiles;
    QMenu       *m_recentMenu;
    QAction     *m_toolWindowAct;
    QString      m_initPath;
    QMenu*       m_filterMenu;
    QAction*     m_showHideFilesAct;
    QAction*     m_showDetailsAct;
    QAction*     m_syncEditorAct;
};

#endif // FILEMANAGER_H
