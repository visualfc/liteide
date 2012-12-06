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
// Module: filebrowser.h
// Creator: visualfc <visualfc@gmail.com>



#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include "liteapi/liteapi.h"
#include <QModelIndex>

class QComboBox;
class QTreeView;
class QFileSystemModel;
class QSortFilterProxyModel;
class QFileInfo;
class QDir;
class QMenu;
class QLineEdit;

class FileBrowser : public QObject
{
    Q_OBJECT
public:
    explicit FileBrowser(LiteApi::IApplication *app,QObject *parent = 0);
    virtual ~FileBrowser();
public:
    static QString getShellCmd(LiteApi::IApplication *app);
    static QStringList getShellArgs(LiteApi::IApplication *app);
signals:

public slots:
    void visibilityChanged(bool);
    void doubleClickedTreeView(QModelIndex);
    void activatedFilter(QString);
    void activatedRoot(QString);
    void syncFileModel(bool);
    void currentEditorChanged(LiteApi::IEditor*);
    void treeViewContextMenuRequested(const QPoint &pos);
protected slots:
    void openFile();
    void openEditor();
    void newFile();
    void newFileWizard();
    void renameFile();
    void removeFile();
    void newFolder();
    void renameFolder();
    void removeFolder();
    void openShell();
    void openExplorer();
    void setFolderToRoot();
    void cdUp();
    void viewGodoc();
    void loadFolderProject();
    void commandReturn();
    void requestCommand();
protected:
    QFileInfo contextFileInfo() const;
    QDir contextDir() const;
    void showTreeViewContextMenu(const QPoint &globalPos, const QFileInfo &info);
    void addFolderToRoot(const QString &path);
protected:
    LiteApi::IApplication   *m_liteApp;
    QSortFilterProxyModel   *m_proxyModel;
    QWidget                 *m_widget;
    QTreeView               *m_treeView;
    QFileSystemModel        *m_fileModel;
    QComboBox   *m_filterCombo;
    QComboBox   *m_rootCombo;
    QLineEdit   *m_commandEdit;
    QToolBar    *m_filterToolBar;
    QToolBar    *m_rootToolBar;
    QAction *m_syncAct;
protected:
    QModelIndex m_contextIndex;
    QAction *m_toolWindowAct;
    QMenu   *m_fileMenu;
    QMenu   *m_folderMenu;
    QMenu   *m_rootMenu;
    QAction *m_openFileAct;
    QAction *m_openEditorAct;
    QAction *m_newFileAct;
    QAction *m_newFileWizardAct;
    QAction *m_removeFileAct;
    QAction *m_renameFileAct;
    QAction *m_newFolderAct;
    QAction *m_removeFolderAct;
    QAction *m_renameFolderAct;
    QAction *m_openShellAct;
    QAction *m_cdupAct;
    QAction *m_setRootAct;
    QAction *m_openExplorerAct;
    QAction *m_viewGodocAct;
    QAction *m_loadFolderAct;
};

#endif // FILEBROWSER_H
