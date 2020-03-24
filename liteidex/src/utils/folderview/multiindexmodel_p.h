/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
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
// Module: multiindexmodel_p.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MULTIINDEXMODEL_P_H
#define MULTIINDEXMODEL_P_H

#include <QList>
#include <QVector>
#include <QPair>
#include <QModelIndex>
#include "abstractmultiproxymodel_p.h"
#include "multiindexmodel.h"

typedef QList<QPair<QModelIndex, QPersistentModelIndex> > QModelIndexPairList;

struct Mapping
{
    int          rowCount;
    QVector<int> source_rows;      //source rows
    QVector<int> proxy_rows;       //proxy rows
    QModelIndex  sourceParent;    //source parent
    QAbstractItemModel *sourceModel;
};

class MultiIndexModel;
class MultiIndexModelPrivate : public AbstractMultiProxyModelPrivate
{
   Q_OBJECT
public:
  MultiIndexModelPrivate()
  {
      source_sort_column = -1;
      sort_order = Qt::AscendingOrder;
      sort_casesensitivity = Qt::CaseSensitive;
      sort_role = Qt::DisplayRole;
      sort_localeaware = false;
  }

  Q_DECLARE_PUBLIC(MultiIndexModel)

  mutable QMap<QAbstractItemModel*, QMap<QModelIndex,Mapping*> > modelMapping;

  int sort_order;
  int source_sort_column;
  Qt::CaseSensitivity sort_casesensitivity;
  int sort_role;
  bool sort_localeaware;
  QModelIndexPairList saved_persistent_indexes;
  QList<Mapping*> removeMappingList;

public slots:
  void _q_sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
  void _q_sourceRowsInserted(const QModelIndex &parent, int start, int end);
  void _q_sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
  void _q_sourceRowsRemoved(const QModelIndex &parent, int start, int end);
  void _q_sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);
  void _q_sourceRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);

  void _q_sourceColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end);
  void _q_sourceColumnsInserted(const QModelIndex &parent, int start, int end);
  void _q_sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
  void _q_sourceColumnsRemoved(const QModelIndex &parent, int start, int end);
  void _q_sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);
  void _q_sourceColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest);

  void _q_sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
  void _q_sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);

  void _q_sourceLayoutAboutToBeChanged();
  void _q_sourceLayoutChanged();
  void _q_sourceModelAboutToBeReset();
  void _q_sourceModelReset();
protected:
   QMap<QModelIndex,Mapping*>::iterator createMapping(QAbstractItemModel *model, const QModelIndex &parent, bool forceUpdate, const QString &context = "update") const;
   QMap<QModelIndex,Mapping*>::iterator findOrCreateMapping(QAbstractItemModel *model, const QModelIndex &parent) const;
   void build_source_to_proxy_mapping(const QVector<int> &proxy_to_source, QVector<int> &source_to_proxy) const;
   void clearMapping();
   void deleteMapping(QAbstractItemModel *model);

   inline int isRootIndexRow(QAbstractItemModel *model, const QModelIndex &index) const
   {
       int row = 0;
       foreach (SourceModelIndex i, indexList) {
           if (i.model == model && i.index.internalId() == index.internalId()) {
               return row;
           }
           row++;
       }
       return -1;
   }

   void sort_source_rows(QAbstractItemModel *model, QVector<int> &source_rows, const QModelIndex &source_parent) const;
   QModelIndexPairList store_persistent_indexes(const QModelIndexList &persistentList);
   void update_persistent_indexes(const QModelIndexPairList &source_indexes);

public:
   void sort(QAbstractItemModel *sourceModel);
};

#endif // MULTIINDEXMODEL_P_H

