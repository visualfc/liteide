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
// Module: abstractmultiproxymodel_p.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef ABSTRACTMULTIPROXYMODEL_P_H
#define ABSTRACTMULTIPROXYMODEL_P_H

#include <QAbstractItemModel>
#include "abstractmultiproxymodel.h"

class AbstractMultiProxyModel;
class AbstractMultiProxyModelPrivate : public QObject //: public QAbstractItemModelPrivate
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(AbstractMultiProxyModel)
public:
    AbstractMultiProxyModelPrivate() {}
    QList<SourceModelIndex> indexList;
    inline bool indexValid(const QModelIndex &index) const {
         return (index.row() >= 0) && (index.column() >= 0) && (index.model() == q_func());
    }
    inline bool containsModel(QAbstractItemModel *model)
    {
        foreach (SourceModelIndex index, indexList) {
            if (index.model == model) {
                return true;
            }
        }
        return false;
    }
public slots:
    virtual void _q_sourceModelDestroyed();
public:
    QAbstractItemModel *q_ptr;
};

#endif // ABSTRACTMULTIPROXYMODEL_P_H

