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
// Module: multifoldermodel.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MULTIFOLDERMODEL_H
#define MULTIFOLDERMODEL_H

#include "multiindexmodel.h"
#include <QDir>
#include <QMap>
#include <QFileSystemModel>

class MultiFolderModel : public MultiIndexModel
{
    Q_OBJECT
public:
    explicit MultiFolderModel(QObject* parent = 0);
    virtual ~MultiFolderModel();
public:
    QModelIndex addRootPath(const QString &path);
    void removeRootPath(const QString &path);
    void removeRoot(const QModelIndex &index);

    bool isRootPath(const QString &path) const;
    bool isRootIndex(const QModelIndex &index) const;
    void clearAll();
    void reloadAll();

    QList<QModelIndex> rootIndexs() const;
    QStringList rootPathList() const;

    QString filePath(const QModelIndex &index) const;
    QString fileName(const QModelIndex &index) const;
    QFileInfo fileInfo(const QModelIndex &index) const;
    QString fileRootPath(const QModelIndex &index) const;

    bool isDir(const QModelIndex &index) const;
    qint64 size(const QModelIndex &index) const;
    QString type(const QModelIndex &index) const;
    QDateTime lastModified(const QModelIndex &index) const;
    QFile::Permissions permissions(const QModelIndex &index) const;

    bool remove(const QModelIndex &index);
    bool rmdir(const QModelIndex &index);

    QList<QModelIndex> indexForPath(const QString &path) const;
    QModelIndex indexForPath(QFileSystemModel* sourceModel,const QString &path) const;
signals:
    void directoryLoaded(QFileSystemModel*,QString);
public:
    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;

    void setSorting(QDir::SortFlags sort);
    QDir::SortFlags sorting() const;

    void setResolveSymlinks(bool enable);
    bool resolveSymlinks() const;

    void setReadOnly(bool enable);
    bool isReadOnly() const;

    void setNameFilterDisables(bool enable);
    bool nameFilterDisables() const;

    void setNameFilters(const QStringList &filters);
    QStringList nameFilters() const;

    void setShowDetails(bool b);
    bool isShowDetails() const;
public:
    virtual bool lessThan(const QAbstractItemModel *sourceModel, const QModelIndex &left, const QModelIndex &right) const;
    virtual bool filterAcceptsRow(const QAbstractItemModel *sourceModel, int source_row, const QModelIndex &source_parent) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
protected slots:
    void slotDirectoryLoaded(const QString &path);
protected:
    QDir::SortFlags m_sorts;
    QDir::Filters   m_filters;
    bool            m_resolveSymlinks;
    bool            m_isReadOnly;
    bool            m_nameFilterDisables;
    bool            m_isShowDetails;
    QStringList     m_nameFilters;
};

#endif // MULTIFOLDERMODEL_H
