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
// Module: abstractmultiproxymodel.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef ABSTRACTMULTIPROXYMODEL_H
#define ABSTRACTMULTIPROXYMODEL_H

#include <QAbstractItemModel>
#include <QItemSelection>

class AbstractMultiProxyModelPrivate;

struct SourceModelIndex
{
    SourceModelIndex() : model(0)
    {
    }
    SourceModelIndex(QAbstractItemModel *m, const QModelIndex &i) : model(m), index(i)
    {
    }
    bool isValid() {
        return model != 0 && index.isValid();
    }
    QAbstractItemModel *model;
    QModelIndex         index;
};

class AbstractMultiProxyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    AbstractMultiProxyModel(QObject *parent = 0);
    ~AbstractMultiProxyModel();

    virtual bool addSourceModel(QAbstractItemModel *sourceModel, const QModelIndex &sourceIndex);
    virtual bool removeSourceModel(QAbstractItemModel *sourceModel);
    virtual void removeAllSourceModel();

    QList<SourceModelIndex> sourceModelIndexlList() const;
    QList<QAbstractItemModel*> sourceModelList() const;

    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const = 0;
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const = 0;

    virtual SourceModelIndex mapToSourceEx(const QModelIndex &proxyIndex) const = 0;
    virtual QModelIndex mapFromSourceEx(QAbstractItemModel *sourceModel, const QModelIndex &sourceIndex) const = 0;

    virtual QItemSelection mapSelectionToSource(const QItemSelection &selection) const;
    virtual QItemSelection mapSelectionFromSource(const QItemSelection &selection) const;

    bool submit();
    void revert();

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool setItemData(const QModelIndex& index, const QMap<int, QVariant> &roles);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

    QModelIndex buddy(const QModelIndex &index) const;
    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    QSize span(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QMimeData* mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;

protected:
    AbstractMultiProxyModel(AbstractMultiProxyModelPrivate &, QObject *parent);

private:
    Q_DECLARE_PRIVATE(AbstractMultiProxyModel)
    Q_DISABLE_COPY(AbstractMultiProxyModel)
    Q_PRIVATE_SLOT(d_func(), void _q_sourceModelDestroyed())
protected:
    AbstractMultiProxyModelPrivate *d_ptr;
};


#endif // ABSTRACTMULTIPROXYMODEL_H
