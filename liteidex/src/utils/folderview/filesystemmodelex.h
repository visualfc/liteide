/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
#include <QSortFilterProxyModel>

class FileSystemModelEx : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit FileSystemModelEx(QObject *parent = 0);
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void setShowDetails(bool b);
    bool isShowDetails() const;
public:
    friend class FolderListModel;
    static QString fileSize(qint64 bytes);
public:
    bool m_isShowDetails;
};

class FileSystemProxyModel : public QSortFilterProxyModel
{
public:
    FileSystemProxyModel(QObject *parent) :
        QSortFilterProxyModel(parent)
    {
    }
    virtual bool lessThan( const QModelIndex & left, const QModelIndex & right ) const
    {
        QFileSystemModel *model = static_cast<QFileSystemModel*>(this->sourceModel());
        QFileInfo l = model->fileInfo(left);
        QFileInfo r = model->fileInfo(right);
        if (l.isDir() && r.isFile()) {
            return true;
        } else if (l.isFile() && r.isDir()) {
            return false;
        }
#ifdef Q_OS_WIN
        if (l.filePath().length() <= 3 || r.filePath().length() <= 3) {
            return l.filePath().at(0) < r.filePath().at(0);
        }
#endif
        return (l.fileName().compare(r.fileName(),Qt::CaseInsensitive) < 0);
    }
};


#endif // FILESYSTEMMODELEX_H
