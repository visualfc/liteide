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
// Module: filesystemwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILESYSTEMWIDGET_H
#define FILESYSTEMWIDGET_H

#include "liteapi/liteapi.h"
#include "filesystemmodel.h"
#include "symboltreeview/symboltreeview.h"

#include <QModelIndex>
#include <QFileInfo>
#include <QDir>
#include <QLabel>

class FileSystemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystemWidget(bool bMultiDirMode, LiteApi::IApplication *app, QWidget *parent = 0);
    virtual ~FileSystemWidget();
    void setHideRoot(bool b);
    bool isHideRoot() const;
    QWidget *widget() { return this; }
    void setRootPathList(const QStringList &rootPathList);
    void addRootPath(const QString &path);
    QStringList rootPathList() const;
    void setStartIndex(const QModelIndex &index);
    void setRootPath(const QString &path);
    QString rootPath() const;
    QString startPath() const;
    void clear();
    SymbolTreeView *treeView() const;
    FileSystemModel *model() const;
    QModelIndex rootIndex() const;
signals:
    void directoryChanged();
public slots:
    void modelReset();
    void showHideFiles(bool b);
    bool isShowHideFiles() const;
    void reloadDirectory(QString);
    void pathIndexChanged(const QModelIndex & index);
    void openPathIndex(const QModelIndex &index);
    void currentEditorChanged(LiteApi::IEditor*);
    void treeViewContextMenuRequested(const QPoint &pos);
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
    void viewGodoc();
    void addFolder();
    void closeFolder();
    void closeAllFolders();
    void syncEditor(bool);
    void expandStartPath(QString);
signals:
    void aboutToShowContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info);
    void startPathChanged(const QString& path);
public:
    QFileInfo contextFileInfo() const;
    QDir contextDir() const;
private:
    LiteApi::IApplication *m_liteApp;
    SymbolTreeView  *m_tree;
    FileSystemModel *m_model;
    QFileInfo m_contextInfo;
    QModelIndex m_contextIndex;
//    QMenu   *m_fileMenu;
//    QMenu   *m_folderMenu;
//    QMenu   *m_rootMenu;
    QAction *m_openEditorAct;
    QAction *m_newFileAct;
    QAction *m_newFileWizardAct;
    QAction *m_removeFileAct;
    QAction *m_renameFileAct;
    QAction *m_newFolderAct;
    QAction *m_removeFolderAct;
    QAction *m_renameFolderAct;
    QAction *m_openShellAct;
    QAction *m_openExplorerAct;
    QAction *m_viewGodocAct;
    QAction *m_addFolderAct;
    QAction *m_closeFolerAct;
    QAction *m_closeAllFoldersAct;
protected:
    bool    m_bMultiDirMode;
    bool    m_bHideRoot;
};

#endif // FILESYSTEMWIDGET_H
