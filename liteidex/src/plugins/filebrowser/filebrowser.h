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
// Module: filebrowser.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include "liteapi/liteapi.h"
#include "folderview/folderview.h"
#include <QModelIndex>

class QComboBox;
class QLineEdit;
class FileBrowser : public QObject
{
    Q_OBJECT
public:
    explicit FileBrowser(LiteApi::IApplication *app,QObject *parent = 0);
    virtual ~FileBrowser();
public slots:
    void showHideFiles(bool b);
    bool isShowHideFiles() const;
    void visibilityChanged(bool);
    void activatedRoot(QString);
    void syncFileModel(bool);
    void currentEditorChanged(LiteApi::IEditor*);
    void aboutToShowContextMenu(QMenu* menu,LiteApi::FILESYSTEM_CONTEXT_FLAG flag,const QFileInfo &fileInfo);
protected slots:
    void setFolderToRoot();
    void cdUp();
    void openFolderInNewWindow();
    void addToFolders();
    void executeFile();
    void doubleClicked(const QModelIndex &index);
protected:
    void addFolderToRoot(const QString &path);
protected:
    LiteApi::IApplication   *m_liteApp;
    QWidget                 *m_widget;
    FolderView        *m_folderView;
    //QComboBox   *m_filterCombo;
    QComboBox   *m_rootCombo;
    //QToolBar    *m_filterToolBar;
    QToolBar    *m_rootToolBar;
    QAction *m_syncAct;
    QAction *m_showHideFilesAct;
    QMenu   *m_configMenu;
protected:
    QAction *m_toolWindowAct;
    QAction *m_cdupAct;
    QAction *m_setRootAct;
    QAction *m_openExplorerAct;
    QAction *m_openFolderInNewWindowAct;
    QAction *m_addToFoldersAct;
    QAction *m_executeFileAct;
};

#endif // FILEBROWSER_H
