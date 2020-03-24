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
// Module: splitfolderwindow.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SPLITFOLDERWINDOW_H
#define SPLITFOLDERWINDOW_H

#include "filemanager.h"
#include "folderview/folderview.h"

class QStackedWidget;
class QTreeWidget;
class QSplitter;
class QStandardItemModel;
class SplitFolderView;
class SplitFolderWindow : public IFolderWindow
{
    Q_OBJECT
public:
    SplitFolderWindow(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~SplitFolderWindow();
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
    void currentIndexChanged(const QModelIndex &index,const QModelIndex &prev);
    void closeFolderIndex(const QModelIndex &index);
    void reloadFolderIndex(const QModelIndex &index);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void doubleClickedFolderView(const QModelIndex &index);
    void enterKeyPressedFolderView(const QModelIndex &index);
protected:
    LiteApi::IApplication *m_liteApp;
    QSplitter *m_spliter;
    SplitFolderView *m_tree;
    QStandardItemModel *m_model;
    QStackedWidget *m_stacked;
    QStringList m_folderList;
    QDir::Filters m_filters;
    bool m_bShowDetails;
    bool m_bSyncEditor;
protected:
    void addFolderImpl(const QString &folder);
    int findInStacked(const QModelIndex &index);
};

class SplitFolderView : public BaseFolderView
{
    Q_OBJECT
public:
    SplitFolderView(LiteApi::IApplication *app, QWidget *parent = 0);
    void addRootPath(const QString &folder);
    void clear();
signals:
    void closeFolderIndex(const QModelIndex &index);
    void reloadFolderIndex(const QModelIndex &index);
public slots:
    void customContextMenuRequested(const QPoint &pos);
    virtual void openFolder();
    virtual void closeFolder();
    virtual void reloadFolder();
protected:
    QStandardItemModel *m_model;
    QMenu *m_contextMenu;
};

#endif // SPLITFOLDERWINDOW_H
