/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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

using namespace LiteApi;

class NewFileDialog;
class QFileSystemWatcher;
struct FileStateItem;


class FileSystemWidget;
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
    virtual IEditor *openEditor(const QString &fileName, bool bActive = true);
    virtual IEditor *createEditor(const QString &contents, const QString &mimeType);
    virtual IEditor *createEditor(const QString &fileName);
    virtual IProject *openProject(const QString &fileName);
    virtual IProject *openProjectScheme(const QString &fileName, const QString &scheme);

    virtual QStringList schemeList() const;
    virtual void addRecentFile(const QString &fileName, const QString &scheme);
    virtual void removeRecentFile(const QString &fileName, const QString &scheme);
    virtual QStringList recentFiles(const QString &scheme) const;
    virtual bool findProjectTargetInfo(const QString &fileName, QMap<QString,QString>& targetInfo) const;
    virtual void openFolderEx(const QString &folder);
    virtual QStringList folderList() const;
    virtual void setFolderList(const QStringList &folders);
    virtual void addFolderList(const QStringList &folders);
    virtual void openFolderInNewWindow(const QString &folder);
public:
    QString openAllTypeFilter() const;
    QString openProjectTypeFilter() const;
    QString openEditorTypeFilter() const;
protected:
    QString schemeKey(const QString &scheme) const;
	QString schemeName(const QString &scheme) const;
    void updateFileState(const QString &fileName);
public slots:
    void updateRecentFileActions(const QString &scheme);
    void openRecentFile();
    void newFile();
    void openFiles();
    void openFolder();
    void openFolderNewWindow();
    void newInstance();
    void openEditors();
    void openProjects();
    void fileChanged(QString);
    void editorSaved(LiteApi::IEditor*);
    void editorCreated(LiteApi::IEditor*);
    void editorAboutToClose(LiteApi::IEditor*);
    void checkForReload();
    void cleanRecent();
    void applyOption(QString);
protected:
    NewFileDialog        *m_newFileDialog;
    FileSystemWidget     *m_folderWidget;
    QFileSystemWatcher   *m_fileWatcher;
    QMap<QString,QDateTime> m_fileStateMap;
    QStringList          m_changedFiles;
    bool                 m_checkActivated;
    QAction              *m_recentSeparator;
    QMap<QString,QMenu*> m_schemeMenuMap;
    int         m_maxRecentFiles;
    QMenu       *m_recentMenu;
    QString      m_initPath;
};

#endif // FILEMANAGER_H
