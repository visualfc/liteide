/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
    SymbolTreeState() : vbar(-1),hbar(-1)
    {
    }
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
    SymbolTreeView(bool bResizeToContents, QWidget *parent = 0);
protected:
    void init(bool bResizeToContents);
public:
    QModelIndex topViewIndex();
    QList<QModelIndex> expandIndexs() const;
    virtual void reset();
    void saveState(SymbolTreeState *state);
    void loadState(QAbstractItemModel *model,SymbolTreeState *state);
signals:
    void currentIndexChanged(const QModelIndex &current, const QModelIndex &previous);
    void enterKeyPressed(QModelIndex);
protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void getTreeExpands(const QModelIndex &parent, QList<QModelIndex> &list) const;
protected slots:
    void clickedItem(QModelIndex);
    void hsbValueChanged(int);
protected:
    bool m_bClickedItem;
    int  m_hsbPos;
};

#endif //SYMBOLTREEVIEW_H
