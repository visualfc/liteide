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
// Module: folderlistview.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FOLDERLISTVIEW_H
#define FOLDERLISTVIEW_H

#include "basefoldeview.h"
#include "folderlistmodel.h"

class FolderListView : public BaseFolderView
{
    Q_OBJECT
public:
    explicit FolderListView(LiteApi::IApplication *app, QWidget *parent = 0);
    FolderListModel *model() const;
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
    virtual void closeAllFolders();
protected:
    FolderListModel *m_model;
};

#endif // FOLDERLISTVIEW_H
