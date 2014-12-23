/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: symboltreeview.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SYMBOLTREEVIEW_H
#define SYMBOLTREEVIEW_H

#include <QTreeView>

struct SymbolTreeState
{
    QList<QStringList> expands;
    QStringList cur;
    int         vbar;
    int         hbar;
};

class SymbolTreeView : public QTreeView
{
    Q_OBJECT
public:
    SymbolTreeView(QWidget *parent = 0);
    QModelIndex topViewIndex();
    QList<QModelIndex> expandIndexs() const;
    virtual void reset();
    void saveState(SymbolTreeState *state);
    void loadState(QAbstractItemModel *model,SymbolTreeState *state);
signals:
    void currentIndexChanged(const QModelIndex &current, const QModelIndex &previous);
protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
#ifdef Q_WS_MAC
    void keyPressEvent(QKeyEvent *event);
#endif
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void getTreeExpands(const QModelIndex &parent, QList<QModelIndex> &list) const;
};

#endif //SYMBOLTREEVIEW_H
