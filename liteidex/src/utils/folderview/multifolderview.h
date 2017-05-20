/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
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
// Module: multifolderview.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MULTIFOLDERVIEW_H
#define MULTIFOLDERVIEW_H

#include "basefoldeview.h"
#include "multifoldermodel.h"

class MultiFolderView : public BaseFolderView
{
public:
    Q_OBJECT
public:
    explicit MultiFolderView(LiteApi::IApplication *app, QWidget *parent = 0);
    virtual ~MultiFolderView();
    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;
    QFileInfo fileInfo(const QModelIndex &index) const;
public:
    bool addRootPath(const QString &path);
    void setRootPathList(const QStringList &pathList);
    QStringList rootPathList() const;
    void clear();
    void expandFolder(const QString &path, bool expand);
    QList<QModelIndex> indexForPath(const QString &path) const;
    bool isShowDetails() const;
public slots:
    void setShowDetails(bool b);
public slots:
    void customContextMenuRequested(const QPoint &pos);
    virtual void removeFolder();
    virtual void removeFile();
    virtual void openFolder();
    virtual void closeFolder();
    virtual void reloadFolder();
    virtual void closeAllFolders();
protected slots:
    void directoryLoaded(QFileSystemModel *model,const QString &path);
protected:
    MultiFolderModel *m_model;
    QMenu            *m_contextMenu;
};

#endif // MULTIFOLDERVIEW_H
