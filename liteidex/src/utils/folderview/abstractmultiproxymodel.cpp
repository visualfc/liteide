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
// Module: abstractmultiproxymodel.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "abstractmultiproxymodel.h"
#include "abstractmultiproxymodel_p.h"

#include <QSize>
#include <QStringList>
#include <QDebug>

//detects the deletion of the source model
void AbstractMultiProxyModelPrivate::_q_sourceModelDestroyed()
{
    //model = QAbstractItemModelPrivate::staticEmptyModel();
    Q_Q(AbstractMultiProxyModel);
    QAbstractItemModel *sourceModel = (QAbstractItemModel*)sender();
    q->removeSourceModel(sourceModel);
}

/*!
    Constructs a proxy model with the given \a parent.
*/

AbstractMultiProxyModel::AbstractMultiProxyModel(QObject *parent)
    :QAbstractItemModel(parent)
{
    d_ptr = new AbstractMultiProxyModelPrivate;
    d_ptr->q_ptr = this;
    //setSourceModel(QAbstractItemModelPrivate::staticEmptyModel());
}

/*!
    \internal
*/

AbstractMultiProxyModel::AbstractMultiProxyModel(AbstractMultiProxyModelPrivate &dd, QObject *parent)
    : QAbstractItemModel(parent), d_ptr(&dd)
{    
    d_ptr->q_ptr = this;

}

/*!
    Destroys the proxy model.
*/
AbstractMultiProxyModel::~AbstractMultiProxyModel()
{

}

/*!
    Sets the given \a sourceModel to be processed by the proxy model.
*/

bool AbstractMultiProxyModel::addSourceModel(QAbstractItemModel *sourceModel,  const QModelIndex &sourceIndex)
{
    Q_D(AbstractMultiProxyModel);
    if (d->containsModel(sourceModel)) {
        return false;
    }
//    if (sourceModel) {
//        connect(sourceModel,SIGNAL(destroyed(QObject*)),d,SLOT(_q_sourceModelDestroyed()));
//    }
    d->indexList.push_back(SourceModelIndex(sourceModel,sourceIndex));
    return true;
}

bool AbstractMultiProxyModel::removeSourceModel(QAbstractItemModel *sourceModel)
{
    Q_D(AbstractMultiProxyModel);
    QMutableListIterator<SourceModelIndex> i(d->indexList);
    int n = 0;
    while (i.hasNext()) {
        SourceModelIndex index = i.next();
        if (index.model == sourceModel) {
            this->beginRemoveRows(QModelIndex(),n,n);
            disconnect(sourceModel,0,this,0);
            i.remove();
            this->endRemoveRows();
            return true;
        }
        n++;
    }    
    return false;
}

void AbstractMultiProxyModel::removeAllSourceModel()
{
    this->beginResetModel();
    Q_D(AbstractMultiProxyModel);
    foreach (SourceModelIndex si, d->indexList) {
        disconnect(si.model,0,this,0);
    }
    d->indexList.clear();
    this->endResetModel();
}

QList<SourceModelIndex> AbstractMultiProxyModel::sourceModelIndexlList() const
{
    Q_D(const AbstractMultiProxyModel);
    return d->indexList;
}

QList<QAbstractItemModel *> AbstractMultiProxyModel::sourceModelList() const
{
    Q_D(const AbstractMultiProxyModel);
    QList<QAbstractItemModel*> modelList;
    foreach (SourceModelIndex i, d->indexList) {
        modelList.push_back(i.model);
    }
    return modelList;
}

/*!
    \reimp
 */
bool AbstractMultiProxyModel::submit()
{
    Q_D(AbstractMultiProxyModel);
    if (d->indexList.isEmpty()) {
        return false;
    }
    foreach (SourceModelIndex source, d->indexList) {
        source.model->submit();
    }
    return true;
}

/*!
    \reimp
 */
void AbstractMultiProxyModel::revert()
{
    Q_D(AbstractMultiProxyModel);
    foreach (SourceModelIndex source, d->indexList) {
        source.model->revert();
    }
}


/*!
  \fn QModelIndex AbstractMultiProxyModel::mapToSource(const QModelIndex &proxyIndex) const

  Reimplement this function to return the model index in the source model that
  corresponds to the \a proxyIndex in the proxy model.

  \sa mapFromSource()
*/

/*!
  \fn QModelIndex AbstractMultiProxyModel::mapFromSource(const QModelIndex &sourceIndex) const

  Reimplement this function to return the model index in the proxy model that
  corresponds to the \a sourceIndex from the source model.

  \sa mapToSource()
*/

/*!
  Returns a source selection mapped from the specified \a proxySelection.

  Reimplement this method to map proxy selections to source selections.
 */
QItemSelection AbstractMultiProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    QModelIndexList proxyIndexes = proxySelection.indexes();
    QItemSelection sourceSelection;
    for (int i = 0; i < proxyIndexes.size(); ++i) {
        const QModelIndex proxyIdx = mapToSource(proxyIndexes.at(i));
        if (!proxyIdx.isValid())
            continue;
        sourceSelection << QItemSelectionRange(proxyIdx);
    }
    return sourceSelection;
}

/*!
  Returns a proxy selection mapped from the specified \a sourceSelection.

  Reimplement this method to map source selections to proxy selections.
*/
QItemSelection AbstractMultiProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    QModelIndexList sourceIndexes = sourceSelection.indexes();
    QItemSelection proxySelection;
    for (int i = 0; i < sourceIndexes.size(); ++i) {
        const QModelIndex srcIdx = mapFromSource(sourceIndexes.at(i));
        if (!srcIdx.isValid())
            continue;
        proxySelection << QItemSelectionRange(srcIdx);
    }
    return proxySelection;
}

/*!
    \reimp
 */
QVariant AbstractMultiProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
   // Q_D(const AbstractMultiProxyModel);
    SourceModelIndex source = mapToSourceEx(proxyIndex);
    if (!source.index.isValid() && proxyIndex.column() == 0 && role ==  Qt::DisplayRole) {
        return "Root";
    }
    return source.model->data(source.index,role);
}

/*!
    \reimp
 */
QVariant AbstractMultiProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const AbstractMultiProxyModel);
    if (d->indexList.isEmpty()) {
        return QVariant();
    }
    return d->indexList[0].model->headerData(section,orientation,role);
//    int sourceSection;
//    SourceIndex si;
//    if (orientation == Qt::Horizontal) {
//        const QModelIndex proxyIndex = index(0, section);
//        si = mapToSourceEx(proxyIndex);
//        sourceSection = si.index.column();
//    } else {
//        const QModelIndex proxyIndex = index(section, 0);
//        si = mapToSourceEx(proxyIndex);
//        sourceSection = si.index.row();
//    }
//    if (si.model == 0) {
//        qDebug() << "error";
//        return d->modelList[0]->headerData(section,orientation,role);
//    }
//    return si.model->headerData(sourceSection, orientation, role);
}

/*!
    \reimp
 */
QMap<int, QVariant> AbstractMultiProxyModel::itemData(const QModelIndex &proxyIndex) const
{
    return QAbstractItemModel::itemData(proxyIndex);
}

/*!
    \reimp
 */
Qt::ItemFlags AbstractMultiProxyModel::flags(const QModelIndex &index) const
{
    //Q_D(const AbstractMultiProxyModel);
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    SourceModelIndex source = mapToSourceEx(index);
    return source.model->flags(source.index);
}

/*!
    \reimp
 */
bool AbstractMultiProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //Q_D(AbstractMultiProxyModel);
    SourceModelIndex source = mapToSourceEx(index);
    return source.model->setData(source.index,value,role);
}

/*!
    \reimp
 */
bool AbstractMultiProxyModel::setItemData(const QModelIndex &index, const QMap< int, QVariant >& roles)
{
    return QAbstractItemModel::setItemData(index, roles);
}

/*!
    \reimp
 */
bool AbstractMultiProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    //Q_D(AbstractMultiProxyModel);
    int sourceSection;
    SourceModelIndex source;
    if (orientation == Qt::Horizontal) {
        const QModelIndex proxyIndex = index(0, section);
        source = mapToSourceEx(proxyIndex);
        sourceSection = source.index.column();
    } else {
        const QModelIndex proxyIndex = index(section, 0);
        source = mapToSourceEx(proxyIndex);
        sourceSection = source.index.row();
    }
    return source.model->setHeaderData(sourceSection, orientation, value, role);
    //return d->model->setHeaderData(sourceSection, orientation, value, role);
}

/*!
    \reimp
    \since 4.8
 */
QModelIndex AbstractMultiProxyModel::buddy(const QModelIndex &index) const
{
    //Q_D(const AbstractMultiProxyModel);
    SourceModelIndex source = mapToSourceEx(index);
    return mapFromSourceEx(source.model,source.model->buddy(source.index));
}

/*!
    \reimp
    \since 4.8
 */
bool AbstractMultiProxyModel::canFetchMore(const QModelIndex &parent) const
{
    //Q_D(const AbstractMultiProxyModel);
    SourceModelIndex source = mapToSourceEx(parent);
    if (source.model == 0) {
        return false;
    }
    return source.model->canFetchMore(source.index);
}

/*!
    \reimp
    \since 4.8
 */
void AbstractMultiProxyModel::fetchMore(const QModelIndex &parent)
{
    //Q_D(AbstractMultiProxyModel);
    SourceModelIndex source = mapToSourceEx(parent);
    source.model->fetchMore(source.index);
    //d->model->fetchMore(mapToSource(parent));
}

/*!
    \reimp
    \since 4.8
 */
void AbstractMultiProxyModel::sort(int column, Qt::SortOrder order)
{
    Q_D(AbstractMultiProxyModel);
    //d->model->sort(column, order);
    foreach (SourceModelIndex source, d->indexList) {
        source.model->sort(column,order);
    }
}

/*!
    \reimp
    \since 4.8
 */
QSize AbstractMultiProxyModel::span(const QModelIndex &index) const
{
    //Q_D(const AbstractMultiProxyModel);
    SourceModelIndex source = mapToSourceEx(index);
    return source.model->span(source.index);
}

/*!
    \reimp
    \since 4.8
 */
bool AbstractMultiProxyModel::hasChildren(const QModelIndex &parent) const
{
    //Q_D(const AbstractMultiProxyModel);
    SourceModelIndex source = mapToSourceEx(parent);
    return source.model->hasChildren(source.index);
}

/*!
    \reimp
    \since 4.8
 */
QMimeData* AbstractMultiProxyModel::mimeData(const QModelIndexList &indexes) const
{
    Q_D(const AbstractMultiProxyModel);
    if (d->indexList.isEmpty()) {
        return QAbstractItemModel::mimeData(indexes);
    }

    QModelIndexList list;
    foreach(const QModelIndex &index, indexes)
        list << mapToSource(index);

    return d->indexList[0].model->mimeData(list);
}

/*!
    \reimp
    \since 4.8
 */
QStringList AbstractMultiProxyModel::mimeTypes() const
{
    Q_D(const AbstractMultiProxyModel);
    if (d->indexList.isEmpty()) {
        return QAbstractItemModel::mimeTypes();
    }
    return d->indexList[0].model->mimeTypes();
}

/*!
    \reimp
    \since 4.8
 */
Qt::DropActions AbstractMultiProxyModel::supportedDropActions() const
{
    Q_D(const AbstractMultiProxyModel);
    if (d->indexList.isEmpty()) {
        return QAbstractItemModel::supportedDropActions();
    }
    return d->indexList[0].model->supportedDropActions();
}

#include "moc_abstractmultiproxymodel.cpp"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end
