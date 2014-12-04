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
// Module: folderview.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QTreeView>
#include <QFileSystemModel>
#include "basefoldeview.h"

class FolderView : public BaseFolderView
{
    Q_OBJECT
public:
    explicit FolderView(LiteApi::IApplication *app, QWidget *parent = 0);
    void setRootPath(const QString &path);
    QString rootPath() const;
    QFileSystemModel *model() const;
public slots:
    void customContextMenuRequested(const QPoint &pos);    
    virtual void removeFolder();
    virtual void removeFile();
protected:
    QFileSystemModel *m_model;
};

#endif // FOLDERVIEW_H
