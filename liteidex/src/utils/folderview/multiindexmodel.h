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
// Module: multiindexmodel.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MULTIINDEXMODEL_H
#define MULTIINDEXMODEL_H

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QItemSelection>

#include "abstractmultiproxymodel.h"

class MultiIndexModelPrivate;
class MultiIndexModel : public AbstractMultiProxyModel
{
    Q_OBJECT
public:
    explicit MultiIndexModel(QObject* parent = 0);
    ~MultiIndexModel();

    virtual bool addSourceModel(QAbstractItemModel* sourceModel, const QModelIndex &sourceIndex);
    virtual bool removeSourceModel(QAbstractItemModel *sourceModel);
    virtual void removeAllSourceModel();

    void sort (int column, Qt::SortOrder order = Qt::AscendingOrder);

    int	sortRole() const;
    void setSortRole(int role);

    Qt::CaseSensitivity	sortCaseSensitivity() const;
    void setSortCaseSensitivity(Qt::CaseSensitivity cs);

    bool isSortLocaleAware() const;
    void setSortLocaleAware(bool on);

    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const;

    SourceModelIndex mapToSourceEx(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSourceEx(QAbstractItemModel *sourceModel, const QModelIndex &sourceIndex) const;

    QModelIndex parent(const QModelIndex& child) const;
    bool hasChildren(const QModelIndex &parent) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
    QModelIndex buddy(const QModelIndex &index) const;

    QItemSelection mapSelectionFromSource(const QItemSelection& selection) const;
    QItemSelection mapSelectionToSource(const QItemSelection& selection) const;
    QModelIndexList match(const QModelIndex& start, int role, const QVariant& value, int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const;

    bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

protected:
    MultiIndexModel(MultiIndexModelPrivate &dd, QObject* parent);
public:
    virtual bool lessThan(const QAbstractItemModel *sourceModel, const QModelIndex &left, const QModelIndex &right) const;
private:
    Q_DECLARE_PRIVATE(MultiIndexModel)
    Q_DISABLE_COPY(MultiIndexModel)
};


#endif // MULTIINDEXMODEL_H
