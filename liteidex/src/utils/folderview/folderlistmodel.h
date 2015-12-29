/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: folderlistmodel.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FOLDERLISTMODEL_H
#define FOLDERLISTMODEL_H

#include <QAbstractItemModel>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QItemSelection>

class QFileSystemWatcher;

struct SourceModel
{
    QFileSystemModel* model;
    QString      rootPath;
    QString      watchPath;
    QModelIndex  rootIndex;
    QModelIndex  rootSourceIndex;
};

class FolderListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit FolderListModel(QObject *parent = 0);
    virtual ~FolderListModel();
    void clear();
    QModelIndex addRootPath(const QString &path);
    void removeRootPath(const QString &path);
    void removeRoot(const QModelIndex &index);
    void reloadRoot(const QModelIndex &index);
    bool isRootPath(const QString &path);
    QList<QModelIndex> rootIndexs() const;
    QStringList rootPathList() const;
    QString filePath(const QModelIndex &index) const;
    QString fileName(const QModelIndex &index) const;
    QFileInfo fileInfo(const QModelIndex &index) const;
    bool isDir(const QModelIndex &index) const;
    QModelIndex mkdir(const QModelIndex &parent,const QString &name);
    bool rmdir(const QModelIndex &index);
    bool remove(const QModelIndex &index);
    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;
    void setNameFilters(const QStringList &filters);
    QStringList nameFilters() const;
    void setNameFilterDisables(bool enable);
    bool nameFilterDisables() const;
    void setResolveSymlinks(bool enable);
    bool resolveSymlinks() const;
    bool isRootIndex(const QModelIndex &index) const;
    void setWatcherRoot(bool b);
    bool isWatcherRoot() const;
    QList<QModelIndex> indexForPath(const QString &path) const;
protected:
    QFileSystemModel *findSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
    QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const;
    QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const;
    bool isRootSourceIndex(const QModelIndex &sourceIndex) const;
public:
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool setItemData(const QModelIndex& index, const QMap<int, QVariant> &roles);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    QSize span(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex buddy(const QModelIndex &index) const;

    bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
public slots:
    void directoryChanged(const QString &path);

    void sourceRowsAboutToBeInserted(const QModelIndex &,int,int);
    void sourceRowsInserted(const QModelIndex &,int,int);
    void sourceRowsAboutToBeRemoved(const QModelIndex &,int,int);
    void sourceRowsRemoved(const QModelIndex &,int,int);
    void sourceRowsAboutToBeMoved(const QModelIndex &,int,int,const QModelIndex &,int);
    void sourceRowsMoved(const QModelIndex &,int,int,const QModelIndex &,int);

    void sourceColumnsAboutToBeInserted(const QModelIndex &,int,int);
    void sourceColumnsInserted(const QModelIndex &,int,int);
    void sourceColumnsAboutToBeRemoved(const QModelIndex &,int,int);
    void sourceColumnsRemoved(const QModelIndex &,int,int);
    void sourceColumnsAboutToBeMoved(const QModelIndex &,int,int,const QModelIndex &,int);
    void sourceColumnsMoved(const QModelIndex &,int,int,const QModelIndex &,int);

    void sourceDataChanged(const QModelIndex &,const QModelIndex &);
    void sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);

    void sourceLayoutAboutToBeChanged();
    void sourceLayoutChanged();
    void sourceModelAboutToBeReset();
    void sourceModelReset();

protected:
    QList<SourceModel> m_modelList;
    mutable QHash<qint64,QAbstractItemModel*> m_indexMap;
    bool ignoreNextLayoutAboutToBeChanged;
    bool ignoreNextLayoutChanged;
    QList<QPersistentModelIndex> layoutChangePersistentIndexes;
    QModelIndexList proxyIndexes;
    QFileSystemWatcher *m_watcher;
    QDir::Filters       m_filters;
    QStringList         m_nameFilters;
    bool                m_resolveSymlinks;
    bool                m_nameFilterDisables;
    bool                m_bWatcherRoot;
};

#endif // FOLDERLISTMODEL_H
