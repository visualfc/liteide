/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2018 LiteIDE Team. All rights reserved.
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
// Module: folderwindow.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FOLDERWINDOW_H
#define FOLDERWINDOW_H

#include <QObject>
#include "folderview/multifolderview.h"
#include "filemanager.h"

class FolderWindow : public IFolderWindow
{
    Q_OBJECT
public:
    explicit FolderWindow(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~FolderWindow();
    virtual QWidget *widget();
    virtual QStringList folderList() const;
    virtual void setFolderList(const QStringList &folders);
    virtual void addFolderList(const QString &folder);
    virtual void closeAllFolders();
public slots:
    void showHideFiles(bool b);
    void doubleClickedFolderView(const QModelIndex &index);
    void enterKeyPressedFolderView(const QModelIndex &index);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void triggeredSyncEditor(bool b);
protected:
    bool isShowHideFiles() const;
    LiteApi::IApplication *m_liteApp;
    MultiFolderView *m_folderListView;
    QAction     *m_toolWindowAct;
    QMenu*       m_filterMenu;
    QAction*     m_showHideFilesAct;
    QAction*     m_showDetailsAct;
    QAction*     m_syncEditorAct;
};

#endif // FOLDERWINDOW_H
