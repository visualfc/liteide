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
// Module: filesystemmodelex.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILESYSTEMMODELEX_H
#define FILESYSTEMMODELEX_H

#include <QFileSystemModel>

class FolderListModel;
class FileSystemModelEx : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileSystemModelEx(QObject *parent = 0);
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &index, int role) const;
    friend class FolderListModel;
    static QString fileSize(qint64 bytes);
};

#endif // FILESYSTEMMODELEX_H
