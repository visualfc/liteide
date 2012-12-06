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
// Module: gopathbrowser.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOPATHBROWSER_H
#define GOPATHBROWSER_H

#include <QWidget>
#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QModelIndex>
#include <QFileInfo>
#include <QDir>
#include <QLabel>

class QTreeView;
class GopathModel;
class GopathBrowser : public QObject
{
    Q_OBJECT
    
public:
    explicit GopathBrowser(LiteApi::IApplication *app,QObject *parent = 0);
    ~GopathBrowser();
    QWidget *widget() const;
    void setPathList(const QStringList &pathList);
    void addPathList(const QString &path);
    QStringList pathList() const;
    QStringList systemGopathList() const;
    void setStartIndex(const QModelIndex &index);
    QString startPath() const;
public slots:
    void pathIndexChanged(const QModelIndex & index);
    void openPathIndex(const QModelIndex &index);
    void reloadEnv();
    void currentEditorChanged(LiteApi::IEditor*);
    void treeViewContextMenuRequested(const QPoint &pos);
    void setActivate();
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
    void syncEditor(bool);
    void syncProject(bool);
    void expandStartPath(QString);
signals:
    void startPathChanged(const QString& path);
protected:
    QFileInfo contextFileInfo() const;
    QDir contextDir() const;
private:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    QTreeView *m_pathTree;
    GopathModel *m_model;
    QStringList m_pathList;
    QFileInfo m_contextInfo;
    QModelIndex m_contextIndex;
    QMenu   *m_fileMenu;
    QMenu   *m_folderMenu;
    QAction *m_setStartAct;
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
    QToolBar *m_toolBar;
    QAction *m_syncEditor;
    QAction *m_syncProject;
    QLabel  *m_startPathLabel;
};

#endif // GOPATHBROWSER_H
