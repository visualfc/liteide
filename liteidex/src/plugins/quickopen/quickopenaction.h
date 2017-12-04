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
// Module: quickopenaction.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef QUICKOPENACTION_H
#define QUICKOPENACTION_H

#include "quickopenapi/quickopenapi.h"

class QStandardItem;
class QStandardItemModel;
class QSortFilterProxyModel;
class QuickOpenAction : public LiteApi::IQuickOpen
{
public:
    QuickOpenAction(LiteApi::IApplication *app, QObject *parent = 0);
    virtual QString id() const;
    virtual QString info() const;
    virtual void activate();
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filterChanged(const QString &text);
    virtual void indexChanged(const QModelIndex &index);
    virtual bool selected(const QString &text, const QModelIndex &index);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel    *m_model;
    QSortFilterProxyModel *m_proxyModel;
    QMap<QStandardItem*,QAction*> m_itemActionMap;
};

#endif // QUICKOPENACTION_H
