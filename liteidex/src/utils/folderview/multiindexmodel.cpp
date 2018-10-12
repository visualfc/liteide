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
// Module: multiindexmodel.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "multiindexmodel.h"

/****************************************************************************
**
** Copyright (C) 2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Stephen Kelly <stephen.kelly@kdab.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "multiindexmodel_p.h"

#include <QTime>
#include <QDebug>


class MultiIndexModelLessThan
{
public:
    inline MultiIndexModelLessThan(int column, const QModelIndex &parent,
                                       const QAbstractItemModel *source,
                                       const MultiIndexModel *proxy)
        : sort_column(column), source_parent(parent), source_model(source), proxy_model(proxy) {}

    inline bool operator()(int r1, int r2) const
    {
        QModelIndex i1 = source_model->index(r1, sort_column, source_parent);
        QModelIndex i2 = source_model->index(r2, sort_column, source_parent);
        return proxy_model->lessThan(source_model,i1, i2);
    }

private:
    int sort_column;
    QModelIndex source_parent;
    const QAbstractItemModel *source_model;
    const MultiIndexModel *proxy_model;
};

class MultiIndexModelGreaterThan
{
public:
    inline MultiIndexModelGreaterThan(int column, const QModelIndex &parent,
                                          const QAbstractItemModel *source,
                                          const MultiIndexModel *proxy)
        : sort_column(column), source_parent(parent),
          source_model(source), proxy_model(proxy) {}

    inline bool operator()(int r1, int r2) const
    {
        QModelIndex i1 = source_model->index(r1, sort_column, source_parent);
        QModelIndex i2 = source_model->index(r2, sort_column, source_parent);
        return proxy_model->lessThan(source_model,i2, i1);
    }

private:
    int sort_column;
    QModelIndex source_parent;
    const QAbstractItemModel *source_model;
    const MultiIndexModel *proxy_model;
};

QModelIndexPairList MultiIndexModelPrivate::store_persistent_indexes(const QModelIndexList &persistentList)
{
    Q_Q(MultiIndexModel);
    QModelIndexPairList source_indexes;
    foreach (QModelIndex proxy_index, persistentList) {
        SourceModelIndex index = q->mapToSourceEx(proxy_index);
        source_indexes.append(qMakePair(proxy_index, QPersistentModelIndex(index.index)));
    }
    return source_indexes;
}

void MultiIndexModelPrivate::update_persistent_indexes(const QModelIndexPairList &source_indexes)
{
    Q_Q(MultiIndexModel);
    QModelIndexList from, to;
    for (int i = 0; i < source_indexes.count(); ++i) {
        QModelIndex source_index = source_indexes.at(i).second;
        QModelIndex old_proxy_index = source_indexes.at(i).first;
        if (source_index.isValid()) {
            QAbstractItemModel *model = (QAbstractItemModel *)source_index.model();
            createMapping(model,source_index.parent(),false);
            //createMapping(model,source_index,true);
            QModelIndex proxy_index = q->mapFromSourceEx(model,source_index);
            from << old_proxy_index;
            to << proxy_index;
        } else {
            from << old_proxy_index;
            to << QModelIndex();
        }
    }
    q->changePersistentIndexList(from, to);
}


void MultiIndexModelPrivate::sort_source_rows(QAbstractItemModel *model,
    QVector<int> &source_rows, const QModelIndex &source_parent) const
{
    Q_Q(const MultiIndexModel);
    if (source_sort_column >= 0) {
        if (sort_order == Qt::AscendingOrder) {
            MultiIndexModelLessThan lt(source_sort_column, source_parent, model, q);
            qStableSort(source_rows.begin(), source_rows.end(), lt);
        } else {
            MultiIndexModelGreaterThan gt(source_sort_column, source_parent, model, q);
            qStableSort(source_rows.begin(), source_rows.end(), gt);
        }
    } else { // restore the source model order
        qStableSort(source_rows.begin(), source_rows.end());
    }
}


/*!
    \since 4.8
    \class MultiIndexModel
    \brief The MultiIndexModel class proxies its source model unmodified

    \ingroup model-view

    MultiIndexModel can be used to forward the structure of a source model exactly, with no sorting, filtering or other transformation.
    This is similar in concept to an identity matrix where A.I = A.

    Because it does no sorting or filtering, this class is most suitable to proxy models which transform the data() of the source model.
    For example, a proxy model could be created to define the font used, or the background colour, or the tooltip etc. This removes the
    need to implement all data handling in the same class that creates the structure of the model, and can also be used to create
    re-usable components.

    This also provides a way to change the data in the case where a source model is supplied by a third party which can not be modified.

    \snippet doc/src/snippets/code/src_gui_itemviews_MultiIndexModel.cpp 0

    \sa QAbstractProxyModel, {Model/View Programming}, QAbstractItemModel

*/

/*!
    Constructs an identity model with the given \a parent.
*/
MultiIndexModel::MultiIndexModel(QObject* parent)
  : AbstractMultiProxyModel(*new MultiIndexModelPrivate, parent)
{

}

/*! \internal
 */
MultiIndexModel::MultiIndexModel(MultiIndexModelPrivate &dd, QObject* parent)
  : AbstractMultiProxyModel(dd, parent)
{

}

bool MultiIndexModel::lessThan(const QAbstractItemModel */*sourceModel*/, const QModelIndex &left, const QModelIndex &right) const
{
    Q_D(const MultiIndexModel);
    QVariant l = (left.model() ? left.model()->data(left, d->sort_role) : QVariant());
    QVariant r = (right.model() ? right.model()->data(right, d->sort_role) : QVariant());
    switch (l.userType()) {
    case QVariant::Invalid:
        return (r.type() != QVariant::Invalid);
    case QVariant::Int:
        return l.toInt() < r.toInt();
    case QVariant::UInt:
        return l.toUInt() < r.toUInt();
    case QVariant::LongLong:
        return l.toLongLong() < r.toLongLong();
    case QVariant::ULongLong:
        return l.toULongLong() < r.toULongLong();
    case QMetaType::Float:
        return l.toFloat() < r.toFloat();
    case QVariant::Double:
        return l.toDouble() < r.toDouble();
    case QVariant::Char:
        return l.toChar() < r.toChar();
    case QVariant::Date:
        return l.toDate() < r.toDate();
    case QVariant::Time:
        return l.toTime() < r.toTime();
    case QVariant::DateTime:
        return l.toDateTime() < r.toDateTime();
    case QVariant::String:
    default:
        if (d->sort_localeaware)
            return l.toString().localeAwareCompare(r.toString()) < 0;
        else
            return l.toString().compare(r.toString(), d->sort_casesensitivity) < 0;
    }
    return false;
}

/*!
    Destroys this identity model.
*/
MultiIndexModel::~MultiIndexModel()
{
    Q_D(MultiIndexModel);
    d->clearMapping();
}

/*!
    \reimp
 */
int MultiIndexModel::columnCount(const QModelIndex& parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_D(const MultiIndexModel);
    if (d->indexList.isEmpty()) {
        return 0;
    }
    SourceModelIndex source = mapToSourceEx(parent);
    if (source.model == 0) {
        return d->indexList[0].model->columnCount();
    }
    return source.model->columnCount(source.index);
}

/*!
    \reimp
 */
bool MultiIndexModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == this : true);
   // Q_D(MultiIndexModel);
    SourceModelIndex source = mapToSourceEx(parent);
    return source.model->dropMimeData(data, action, row, column,source.index);
}

QMap<QModelIndex,Mapping*>::iterator MultiIndexModelPrivate::createMapping(QAbstractItemModel *model, const QModelIndex &parent, bool forceUpdate, const QString &/*context*/) const
{        
    QMap<QModelIndex,Mapping*>::iterator it = modelMapping[model].find(parent);
    if (it == modelMapping[model].end()) {
        Mapping *m = new Mapping;
        m->rowCount = 0;
        m->sourceParent = parent;
        m->sourceModel = model;
        it = modelMapping[model].insert(parent,m);
        forceUpdate = true;
    }
    if (forceUpdate) {
        Mapping *m = it.value();
        int rowCount = model->rowCount(parent);
        //qDebug() << context << parent.data() << rowCount;
        QVector<int> rows;
        rows.resize(rowCount);
        for (int i = 0; i < rowCount; i++) {
            rows[i] = i;
        }
        sort_source_rows(model,rows,parent);
        m->rowCount = rowCount;
        m->source_rows.swap(rows);
        m->proxy_rows.resize(rowCount);
        this->build_source_to_proxy_mapping(m->source_rows,m->proxy_rows);
    }
    return it;
}

void MultiIndexModelPrivate::build_source_to_proxy_mapping(
    const QVector<int> &proxy_to_source, QVector<int> &source_to_proxy) const
{
    source_to_proxy.fill(-1);
    int proxy_count = proxy_to_source.size();
    for (int i = 0; i < proxy_count; ++i)
        source_to_proxy[proxy_to_source.at(i)] = i;
}

QMap<QModelIndex,Mapping*>::iterator MultiIndexModelPrivate::findOrCreateMapping(QAbstractItemModel *model, const QModelIndex &parent) const
{
    QMap<QModelIndex,Mapping*>::iterator it = modelMapping[model].find(parent);
    if (it == modelMapping[model].end()) {
        it = createMapping(model,parent,true);
    }
    if (parent.isValid()) {
        QModelIndex source_grand_parent = parent.parent();
        findOrCreateMapping(model,source_grand_parent);
    }
    return it;
}

void MultiIndexModelPrivate::clearMapping()
{
    //Q_Q(MultiIndexModel);
    QMutableMapIterator<QAbstractItemModel*, QMap<QModelIndex,Mapping*> > i(modelMapping);
    while (i.hasNext()) {
        i.next();
        qDeleteAll(i.value());
    }
    modelMapping.clear();
}

void MultiIndexModelPrivate::deleteMapping(QAbstractItemModel *model)
{
    QMutableMapIterator<QAbstractItemModel*, QMap<QModelIndex,Mapping*> > i(modelMapping);
    while (i.hasNext()) {
        i.next();
        if (i.key() == model) {
            qDeleteAll(i.value());
            i.remove();
            break;
        }
    }
}

/*!
    \reimp
 */
QModelIndex MultiIndexModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    if (!hasIndex(row,column,parent)) {
        return QModelIndex();
    }
    Q_D(const MultiIndexModel);
    if (!parent.isValid()) {
        if (row < d->indexList.size()) {
            return createIndex(row,column);
        }
        return QModelIndex();
    }
    SourceModelIndex sourceParent = mapToSourceEx(parent);

    QMap<QModelIndex,Mapping*>::iterator it = d->createMapping(sourceParent.model,sourceParent.index,false,"index");
//    static int check = 1;
//    if (sourceParent.index.data() == "DataBase" && check) {
//        check = 0;
//        qDebug() << "index" << it.value()->source_rows << it.value()->proxy_rows;
//    }
//    QMap<QModelIndex,Mapping*>::iterator it = d->modelMapping[sourceParent.model].find(sourceParent.index);
//    if (it == d->modelMapping[sourceParent.model].end())  {
//        it = d->createMapping(sourceParent.model,sourceParent.index);
//    } else {
//        qDebug() << it.value()->proxy_rows;
//    }

    return createIndex(row,column,it.value());
}

/*!
    \reimp
 */
bool MultiIndexModel::insertColumns(int column, int count, const QModelIndex& parent)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    //Q_D(MultiIndexModel);
    SourceModelIndex source = mapToSourceEx(parent);
    return source.model->insertColumns(column, count, source.index);
}

/*!
    \reimp
 */
bool MultiIndexModel::insertRows(int row, int count, const QModelIndex& parent)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    //Q_D(MultiIndexModel);
    SourceModelIndex source = mapToSourceEx(parent);
    return source.model->insertRows(row, count, source.index);
}



/*!
    \reimp
 */
QItemSelection MultiIndexModel::mapSelectionFromSource(const QItemSelection& selection) const
{
    Q_D(const MultiIndexModel);
    QItemSelection proxySelection;

    qDebug() << "mapSelectionFromSource";

    if (d->indexList.isEmpty())
        return proxySelection;

    QItemSelection::const_iterator it = selection.constBegin();
    const QItemSelection::const_iterator end = selection.constEnd();
    for ( ; it != end; ++it) {
        const QItemSelectionRange range(mapFromSource(it->topLeft()), mapFromSource(it->bottomRight()));
        proxySelection.append(range);
    }

    return proxySelection;
}

/*!
    \reimp
 */
QItemSelection MultiIndexModel::mapSelectionToSource(const QItemSelection& selection) const
{
    Q_D(const MultiIndexModel);
    QItemSelection sourceSelection;

    qDebug() << "mapSelectionToSource";

    if (d->indexList.isEmpty())
        return sourceSelection;

    QItemSelection::const_iterator it = selection.constBegin();
    const QItemSelection::const_iterator end = selection.constEnd();
    for ( ; it != end; ++it) {
        //Q_ASSERT(it->model() == this);
        const QItemSelectionRange range(mapToSource(it->topLeft()), mapToSource(it->bottomRight()));
        sourceSelection.append(range);
    }

    return sourceSelection;
}

class MyModel : public QAbstractProxyModel
{
    friend class MultiIndexModel;
};


/*!
    \reimp
 */
QModelIndex MultiIndexModel::mapToSource(const QModelIndex& proxyIndex) const
{
    return mapToSourceEx(proxyIndex).index;
}

SourceModelIndex MultiIndexModel::mapToSourceEx(const QModelIndex &proxyIndex) const
{
    Q_D(const MultiIndexModel);
    if (!proxyIndex.isValid())
        return SourceModelIndex();

    Q_ASSERT(proxyIndex.model() == this);

    Mapping *m = (Mapping*)proxyIndex.internalPointer();
    if (m == 0) {
        SourceModelIndex si = d->indexList[proxyIndex.row()];
        si.index = si.index.sibling(si.index.row(),proxyIndex.column());
        return si;
    }
    if (m->source_rows.size() <= proxyIndex.row()) {
        return SourceModelIndex();
    }

    int source_row = m->source_rows.at(proxyIndex.row());
    //qDebug() << source_row;
    QModelIndex index = m->sourceModel->index(source_row,proxyIndex.column(),m->sourceParent);
    return SourceModelIndex(m->sourceModel,index);
}

/*!
    \reimp
 */
QModelIndex MultiIndexModel::mapFromSource(const QModelIndex& sourceIndex) const
{    
    //Q_D(const MultiIndexModel);
    if (!sourceIndex.isValid())
        return QModelIndex();    
    QAbstractItemModel *model = (QAbstractItemModel *)sourceIndex.model();
    return mapFromSourceEx(model,sourceIndex);
}

QModelIndex MultiIndexModel::mapFromSourceEx(QAbstractItemModel *sourceModel, const QModelIndex &sourceIndex) const
{
    Q_D(const MultiIndexModel);
    if (sourceModel == 0) {
        return QModelIndex();
    }
    int row = d->isRootIndexRow(sourceModel,sourceIndex);
    if (row >= 0) {
        int column = sourceIndex.column();
        if (column < 0) {
            column = 0;
        }
        //d->createMapping(sourceModel,d->indexList[row].index);
        return createIndex(row,column);
    }

    if (!sourceIndex.isValid()) {
        return QModelIndex();
    }

    QModelIndex parent = sourceModel->parent(sourceIndex);

    QMap<QModelIndex,Mapping*>::iterator it = d->createMapping(sourceModel,parent,false,"map");// d->modelMapping[sourceModel].find(parent);
//    if (it == d->modelMapping[sourceModel].end()) {
//        it = d->createMapping(sourceModel,parent);
//    }
    Mapping *m = it.value();

    if (m->proxy_rows.size() <= sourceIndex.row()) {
        return QModelIndex();
    }

    int proxy_row = m->proxy_rows.at(sourceIndex.row());
   // qDebug() << sourceIndex.data() << proxy_row;
    return createIndex(proxy_row,sourceIndex.column(),m);
    //return QModelIndex();
}

bool MultiIndexModel::hasChildren(const QModelIndex &parent) const
{
    Q_D(const MultiIndexModel);
    if (!parent.isValid()) {
        return !d->indexList.isEmpty();
    }

    SourceModelIndex source = mapToSourceEx(parent);
    if (source.model == 0) {
        return false;
    }
//    //if (parent.isValid() && !source.index.isValid())
//    //    return false;
    if (!source.model->hasChildren(source.index))
        return false;

    if (source.model->canFetchMore(source.index))
        return true; //we assume we might have children that can be fetched

    return true;
   // Mapping *m = d->createMapping(source.model,source.index,false,"hasChildren").value();
   // return m->source_rows.count() != 0;
}

QModelIndex MultiIndexModel::buddy(const QModelIndex &index) const
{
    Q_D(const MultiIndexModel);
    if (!d->indexValid(index))
        return QModelIndex();

    SourceModelIndex source = mapToSourceEx(index);
    QModelIndex source_buddy = source.model->buddy(source.index);
    if (source.index == source_buddy)
        return index;
    return mapFromSourceEx(source.model,source_buddy);
}


/*!
    \reimp
 */
QModelIndexList MultiIndexModel::match(const QModelIndex& start, int role, const QVariant& value, int hits, Qt::MatchFlags flags) const
{
    Q_D(const MultiIndexModel);
    Q_ASSERT(start.isValid() ? start.model() == this : true);
    if (d->indexList.isEmpty())
        return QModelIndexList();

    SourceModelIndex source = mapToSourceEx(start);
    const QModelIndexList sourceList = source.model->match(source.index, role, value, hits, flags);
    QModelIndexList::const_iterator it = sourceList.constBegin();
    const QModelIndexList::const_iterator end = sourceList.constEnd();
    QModelIndexList proxyList;
    for ( ; it != end; ++it)
        proxyList.append(mapFromSourceEx(source.model,*it));
    return proxyList;
}

/*!
    \reimp
 */
QModelIndex MultiIndexModel::parent(const QModelIndex& child) const
{
    Q_ASSERT(child.isValid() ? child.model() == this : true);
    //Q_D(const MultiIndexModel);
    if (!child.isValid()) {
        return QModelIndex();
    }
//    SourceModelIndex index = this->mapToSourceEx(child);
//    if (index.model == 0) {
//        return QModelIndex();
//    }
//    return mapFromSourceEx(index.model,index.index.parent());
    Mapping *m = (Mapping*)child.internalPointer();
    if (m == 0) {
        return QModelIndex();
    }
//    if (m->sourceModel != child.model()) {
//        qDebug() << "error parent" << child;
//    }
    //qDebug() << m->sourceParent.data().toString();
    return mapFromSourceEx(m->sourceModel,m->sourceParent);
//    const QModelIndex sourceIndex = mapToSource(child);
//    foreach (QModelIndex index, d->indexList) {
//        if (index == sourceIndex) {
//            return QModelIndex();
//        }
//    }
//    const QModelIndex sourceParent = sourceIndex.parent();
//    return mapFromSource(sourceParent);
}

/*
QModelIndex MultiProxyModel::parent(const QModelIndex &child) const
{
    Q_D(const MultiProxyModel);
    if (!d->indexValid(child))
        return QModelIndex();
    IndexMap::const_iterator it = d->index_to_iterator(child);
    Q_ASSERT(it != d->source_index_mapping.constEnd());
    QModelIndex source_parent = it.key();
    QModelIndex proxy_parent = mapFromSource(source_parent);
    return proxy_parent;
}
*/

/*!
    \reimp
 */
bool MultiIndexModel::removeColumns(int column, int count, const QModelIndex& parent)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    //Q_D(MultiIndexModel);
    SourceModelIndex source = mapToSourceEx(parent);
    return source.model->removeColumns(column, count, source.index);
}

/*!
    \reimp
 */
bool MultiIndexModel::removeRows(int row, int count, const QModelIndex& parent)
{
   // Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    //Q_D(MultiIndexModel);
    SourceModelIndex source = mapToSourceEx(parent);
    return source.model->removeRows(row, count, source.index);
}

/*!
    \reimp
 */
int MultiIndexModel::rowCount(const QModelIndex& parent) const
{
    Q_D(const MultiIndexModel);
    if (!parent.isValid()) {
        return d->indexList.size();
    }
    //qDebug() << parent.isValid() << parent.data().toString();
    //Q_ASSERT(parent.isValid() ? parent.model() == this : true);
     SourceModelIndex source = mapToSourceEx(parent);
     return source.model->rowCount(source.index);
     QMap<QModelIndex,Mapping*>::iterator it = d->createMapping(source.model,source.index,false,"rowCount");
     return it.value()->source_rows.count();
     //qDebug() << source.model->rowCount(source.index);
//     return source.model->rowCount(source.index);
//     QMap<QModelIndex,Mapping*>::iterator it = d->createMapping(source.model,source.index);
//     qDebug() << parent.column() << parent << source.index << source.index.data() <<  it.value()->source_rows;
//     return it.value()->source_rows.count();
    //qDebug() << source_parent.isValid();
//     int rows = d->model->rowCount(source_parent);
//     qDebug() << "rowCount" << rows << source_parent.data().toString();
//     return rows;
//    QModelIndex source_parent = mapToSource(parent);
//    qDebug() << source_parent.isValid();
//    if (parent.isValid() && !source_parent.isValid())
//        return 0;
//    QMap<QModelIndex,Mapping*>::iterator it = d->findOrCreateMapping(source_parent);
//    return it.value()->source_rows.count();
}

/*!
    \reimp
 */
bool MultiIndexModel::addSourceModel(QAbstractItemModel* sourceModel, const QModelIndex &sourceIndex)
{
    Q_D(MultiIndexModel);
    if (!sourceModel || d->containsModel(sourceModel) ) {
        return false;
    }

    int n = d->indexList.size();

    this->beginInsertRows(QModelIndex(),n,n);

    AbstractMultiProxyModel::addSourceModel(sourceModel,sourceIndex);

    //beginResetModel();

    connect(sourceModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)),
            d_func(),SLOT(_q_sourceRowsAboutToBeInserted(const QModelIndex &, int, int)));
    connect(sourceModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            d_func(),SLOT(_q_sourceRowsInserted(const QModelIndex &, int, int)));
    connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
            d_func(),SLOT(_q_sourceRowsAboutToBeRemoved(const QModelIndex &, int, int)));
    connect(sourceModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            d_func(),SLOT(_q_sourceRowsRemoved(const QModelIndex &, int, int)));
    connect(sourceModel, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            d_func(),SLOT(_q_sourceRowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(sourceModel, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            d_func(),SLOT(_q_sourceRowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(sourceModel, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)),
            d_func(),SLOT(_q_sourceColumnsAboutToBeInserted(const QModelIndex &, int, int)));
    connect(sourceModel, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            d_func(),SLOT(_q_sourceColumnsInserted(const QModelIndex &, int, int)));
    connect(sourceModel, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)),
            d_func(),SLOT(_q_sourceColumnsAboutToBeRemoved(const QModelIndex &, int, int)));
    connect(sourceModel, SIGNAL(columnsRemoved(const QModelIndex &, int, int)),
            d_func(),SLOT(_q_sourceColumnsRemoved(const QModelIndex &, int, int)));
    connect(sourceModel, SIGNAL(columnsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            d_func(),SLOT(_q_sourceColumnsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(sourceModel, SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            d_func(),SLOT(_q_sourceColumnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(sourceModel, SIGNAL(modelAboutToBeReset()),
            d_func(),SLOT(_q_sourceModelAboutToBeReset()));
    connect(sourceModel, SIGNAL(modelReset()),
            d_func(),SLOT(_q_sourceModelReset()));
    connect(sourceModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            d_func(),SLOT(_q_sourceDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(sourceModel, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            d_func(),SLOT(_q_sourceHeaderDataChanged(Qt::Orientation,int,int)));
    connect(sourceModel, SIGNAL(layoutAboutToBeChanged()),
            d_func(),SLOT(_q_sourceLayoutAboutToBeChanged()));
    connect(sourceModel, SIGNAL(layoutChanged()),
            d_func(),SLOT(_q_sourceLayoutChanged()));

    //endResetModel();
    this->endInsertRows();

    return true;
}

bool MultiIndexModel::removeSourceModel(QAbstractItemModel *sourceModel)
{
    Q_D(MultiIndexModel);
    bool b = AbstractMultiProxyModel::removeSourceModel(sourceModel);
    if (b) {
        d->deleteMapping(sourceModel);
    }
    return b;
}

void MultiIndexModel::removeAllSourceModel()
{
    Q_D(MultiIndexModel);
    AbstractMultiProxyModel::removeAllSourceModel();
    d->clearMapping();
}

void MultiIndexModel::sort(int column, Qt::SortOrder order)
{
    Q_D(MultiIndexModel);
    d->sort_order = order;
    d->source_sort_column = column;
}

void MultiIndexModel::setSortRole(int role)
{
    Q_D(MultiIndexModel);
    d->sort_role = role;
}

Qt::CaseSensitivity MultiIndexModel::sortCaseSensitivity() const
{
    Q_D(const MultiIndexModel);
    return d->sort_casesensitivity;
}

void MultiIndexModel::setSortCaseSensitivity(Qt::CaseSensitivity cs)
{
    Q_D(MultiIndexModel);
    d->sort_casesensitivity = cs;
}

bool MultiIndexModel::isSortLocaleAware() const
{
    Q_D(const MultiIndexModel);
    return d->sort_localeaware;
}

void MultiIndexModel::setSortLocaleAware(bool on)
{
    Q_D(MultiIndexModel);
    d->sort_localeaware = on;
}

int MultiIndexModel::sortRole() const
{
    Q_D(const MultiIndexModel);
    return d->sort_role;
}

void MultiIndexModelPrivate::_q_sourceColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == model : true);
    Q_Q(MultiIndexModel);
    QAbstractItemModel *model = (QAbstractItemModel*)sender();
    q->beginInsertColumns(q->mapFromSourceEx(model,parent), start, end);
}

void MultiIndexModelPrivate::_q_sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == model : true);
    Q_Q(MultiIndexModel);
    QAbstractItemModel *model = (QAbstractItemModel*)sender();
    q->beginMoveColumns(q->mapFromSourceEx(model,sourceParent), sourceStart, sourceEnd, q->mapFromSourceEx(model,destParent), dest);
}

void MultiIndexModelPrivate::_q_sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == model : true);
    Q_Q(MultiIndexModel);
    QAbstractItemModel *model = (QAbstractItemModel*)sender();
    q->beginRemoveColumns(q->mapFromSourceEx(model,parent), start, end);
}

void MultiIndexModelPrivate::_q_sourceColumnsInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == model : true);
    Q_Q(MultiIndexModel);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    q->endInsertColumns();
}

void MultiIndexModelPrivate::_q_sourceColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == model : true);
    Q_Q(MultiIndexModel);
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(destParent)
    Q_UNUSED(dest)

    q->endMoveColumns();
}

void MultiIndexModelPrivate::_q_sourceColumnsRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == model : true);
    Q_Q(MultiIndexModel);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    q->endRemoveColumns();
}

void MultiIndexModelPrivate::_q_sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    //Q_ASSERT(topLeft.isValid() ? topLeft.model() == model : true);
    //Q_ASSERT(bottomRight.isValid() ? bottomRight.model() == model : true);
    Q_Q(MultiIndexModel);

    QAbstractItemModel *model = (QAbstractItemModel*)sender();
    q->dataChanged(q->mapFromSourceEx(model,topLeft), q->mapFromSourceEx(model,bottomRight));
}

void MultiIndexModelPrivate::_q_sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    Q_Q(MultiIndexModel);
    q->headerDataChanged(orientation, first, last);
}

void MultiIndexModelPrivate::_q_sourceLayoutAboutToBeChanged()
{
//    if (ignoreNextLayoutAboutToBeChanged)
//        return;

//    Q_Q(MultiIndexModel);

//    foreach(const QPersistentModelIndex &proxyPersistentIndex, q->persistentIndexList()) {
//        proxyIndexes << proxyPersistentIndex;
//        Q_ASSERT(proxyPersistentIndex.isValid());
//        const QPersistentModelIndex srcPersistentIndex = q->mapToSource(proxyPersistentIndex);
//        Q_ASSERT(srcPersistentIndex.isValid());
//        layoutChangePersistentIndexes << srcPersistentIndex;
//    }

//    q->layoutAboutToBeChanged();
    Q_Q(MultiIndexModel);
    saved_persistent_indexes.clear();
    emit q->layoutAboutToBeChanged();
    QModelIndexList persistentList = q->persistentIndexList();
    if (persistentList.isEmpty())
        return;

//    QAbstractItemModel *model = (QAbstractItemModel*)sender();
//    qDebug() << "begin changed" << model;
    saved_persistent_indexes = store_persistent_indexes(persistentList);
}

void MultiIndexModelPrivate::_q_sourceLayoutChanged()
{
    Q_Q(MultiIndexModel);


    QAbstractItemModel *model = (QAbstractItemModel*)sender();

 //   qDebug() << "changed" << model;

    qDeleteAll(modelMapping[model]);
    modelMapping[model].clear();
    //modelMapping.remove(model);
    update_persistent_indexes(saved_persistent_indexes);
    saved_persistent_indexes.clear();
////    if (dynamic_sortfilter && update_source_sort_column()) {
////        //update_source_sort_column might have created wrong mapping so we have to clear it again
// //       qDeleteAll(sourceMapping);
////        sourceMapping.clear();
////    }

//    emit q->layoutChanged();
//    if (ignoreNextLayoutChanged)
//        return;

 //   Q_Q(MultiIndexModel);

//    for (int i = 0; i < proxyIndexes.size(); ++i) {
//        q->changePersistentIndex(proxyIndexes.at(i), q->mapFromSource(layoutChangePersistentIndexes.at(i)));
//    }

//    layoutChangePersistentIndexes.clear();
//    proxyIndexes.clear();

    q->layoutChanged();
}

void MultiIndexModelPrivate::_q_sourceModelAboutToBeReset()
{
    Q_Q(MultiIndexModel);
    q->beginResetModel();
}

void MultiIndexModelPrivate::_q_sourceModelReset()
{
    Q_Q(MultiIndexModel);

    q->endResetModel();
}

void MultiIndexModelPrivate::sort(QAbstractItemModel *sourceModel )
{
    Q_Q(MultiIndexModel);
    emit q->layoutAboutToBeChanged();
    QModelIndexPairList source_indexes = store_persistent_indexes(q->persistentIndexList());
    QMap<QModelIndex,Mapping*>::const_iterator it = modelMapping[sourceModel].constBegin();
    for (; it != modelMapping[sourceModel].constEnd(); ++it) {
        QModelIndex source_parent = it.key();
        Mapping *m = it.value();
        sort_source_rows(m->sourceModel, m->source_rows, source_parent);
        build_source_to_proxy_mapping(m->source_rows, m->proxy_rows);
    }
    update_persistent_indexes(source_indexes);
    emit q->layoutChanged();
}

void MultiIndexModelPrivate::_q_sourceRowsAboutToBeInserted(const QModelIndex &parent, int /*start*/, int /*end*/)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == model : true);
    //Q_Q(MultiIndexModel);
    QAbstractItemModel *model = (QAbstractItemModel *)sender();
    QMap<QModelIndex,Mapping*>::iterator it = modelMapping[model].find(parent);
    if (it == modelMapping[model].end()) {
        //this->createMapping(model,parent,false,"insert");
    }
}

void MultiIndexModelPrivate::_q_sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == model : true);
    Q_Q(MultiIndexModel);
    QAbstractItemModel *model = (QAbstractItemModel *)sender();

    q->beginMoveRows(q->mapFromSourceEx(model,sourceParent), sourceStart, sourceEnd, q->mapFromSourceEx(model,destParent), dest);
}

void MultiIndexModelPrivate::_q_sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == model : true);
    Q_Q(MultiIndexModel);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    QAbstractItemModel *model = (QAbstractItemModel *)sender();
    QMap<QModelIndex,Mapping*>::iterator it = modelMapping[model].find(parent);
    if (it != modelMapping[model].end()) {
        this->createMapping(model,parent,true,"insert");
        Mapping *m = it.value();
        QModelIndex proxyIndex = q->mapFromSourceEx(model,parent);
        QVector<int> all(end-start+1);
        for (int i = start; i <= end; i++) {
            all[i-start] = m->proxy_rows.at(i);
            //qDebug() << m->sourceModel->index(i,0,parent).data();
        }
        qStableSort(all);

        if (all.last()-all.first()+1 == all.size()) {
            q->beginInsertRows(proxyIndex,all.first(),all.last());
            q->endInsertRows();
        } else {
            foreach (int n, all) {
                q->beginInsertRows(proxyIndex, n, n);
                q->endInsertRows();
            }
        }
    }
}

void MultiIndexModelPrivate::_q_sourceRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == model : true);
    Q_Q(MultiIndexModel);
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(destParent)
    Q_UNUSED(dest)

    //todo
    q->endMoveRows();
}

void MultiIndexModelPrivate::_q_sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == model : true);
    Q_Q(MultiIndexModel);

    QAbstractItemModel *model = (QAbstractItemModel *)sender();
    QMap<QModelIndex,Mapping*>::iterator it = modelMapping[model].find(parent);
    if (it != modelMapping[model].end()) {
        Mapping *m = it.value();

        int n1 = m->proxy_rows.at(start);
        int n2 = m->proxy_rows.at(end);
        if (n1 > n2) {
            qSwap(n1,n2);
        }
        for (int i = start; i < end+1; i++) {
            QModelIndex child = m->sourceModel->index(i,0,m->sourceParent);
            QMap<QModelIndex,Mapping*>::iterator childIt = modelMapping[model].find(child);
            if (childIt != modelMapping[model].end()) {
                Mapping *cm = childIt.value();
                cm->sourceParent = QModelIndex();
                modelMapping[model].remove(child);
                removeMappingList.append(cm);
            }
        }
        q->beginRemoveRows(q->mapFromSourceEx(model,parent), n1,n2);
    }
}

void MultiIndexModelPrivate::_q_sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == model : true);
    Q_Q(MultiIndexModel);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)

    //update
    QAbstractItemModel *model = (QAbstractItemModel *)sender();
    QMap<QModelIndex,Mapping*>::iterator it = modelMapping[model].find(parent);
    if (it != modelMapping[model].end()) {
        this->createMapping(model,parent,true);
        q->endRemoveRows();
    }
}


#include "moc_multiindexmodel.cpp"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

