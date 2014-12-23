/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: folderlistview.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FOLDERLISTVIEW_H
#define FOLDERLISTVIEW_H

#include "basefoldeview.h"
#include "folderlistmodel.h"
#include <QFileInfo>

class QSortFilterProxyModel;
class FolderListView : public BaseFolderView
{
    Q_OBJECT
public:
    explicit FolderListView(bool proxyMode, LiteApi::IApplication *app, QWidget *parent = 0);
    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;
    QFileInfo fileInfo(const QModelIndex &index) const;
public:
    void addRootPath(const QString &path);
    void setRootPathList(const QStringList &pathList);
    QStringList rootPathList() const;
    void clear();
public slots:
    void customContextMenuRequested(const QPoint &pos);
    virtual void removeFolder();
    virtual void removeFile();
    virtual void addFolder();
    virtual void closeFolder();
    virtual void reloadFolder();
    virtual void closeAllFolders();
protected:
    QSortFilterProxyModel *m_proxy;
    FolderListModel *m_model;
};

#endif // FOLDERLISTVIEW_H
