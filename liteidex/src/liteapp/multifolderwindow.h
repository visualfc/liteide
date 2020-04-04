/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2018 LiteIDE. All rights reserved.
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
// Module: multifolderwindow.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FOLDERWINDOW_H
#define FOLDERWINDOW_H

#include <QObject>
#include "folderview/multifolderview.h"
#include "filemanager.h"

class MultiFolderWindow : public IFolderWindow
{
    Q_OBJECT
public:
    explicit MultiFolderWindow(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~MultiFolderWindow();
    virtual QString id() const;
    virtual QWidget *widget() const;
    virtual QStringList folderList() const;
    virtual void setFolderList(const QStringList &folders);
    virtual void addFolderList(const QString &folder);
    virtual void closeAllFolders();
    virtual void setShowHideFiles(bool b);
    virtual void setShowDetails(bool b);
    virtual void setSyncEditor(bool b);
public slots:
    void doubleClickedFolderView(const QModelIndex &index);
    void enterKeyPressedFolderView(const QModelIndex &index);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void aboutToShowFolderContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info);
protected:
    LiteApi::IApplication *m_liteApp;
    MultiFolderView *m_folderListView;
    bool m_bSyncEditor;
};

#endif // FOLDERWINDOW_H
