/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: symboltreeview.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "symboltreeview.h"
#include <QHeaderView>
#include <QFocusEvent>
#include <QScrollBar>
#include <QAbstractItemModel>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


static QStringList stringListFromIndex(const QModelIndex &index)
{
    QStringList list;
    if (!index.isValid())
        return list;
    list.append(stringListFromIndex(index.parent()));
    list.append(index.data().toString());
    return list;
}

static QModelIndex indexFromStringList(QAbstractItemModel *model, QStringList &list, const QModelIndex & parent = QModelIndex())
{
    if (list.isEmpty())
        return QModelIndex();
    QString text = list.front();
    for (int i = 0; i < model->rowCount(parent); i++) {
        QModelIndex child = model->index(i,0,parent);
        if (child.data().toString() == text) {
            list.pop_front();
            if (list.isEmpty()) {
                return child;
            } else {
                QModelIndex next = indexFromStringList(model,list,child);
                if (next.isValid())
                    return next;
                else
                    return child;
            }
        }
    }
    return QModelIndex();
}

SymbolTreeView::SymbolTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
   // setFrameStyle(QFrame::NoFrame);
    setIndentation(indentation() * 9/10);
    {
        QHeaderView *treeHeader = header();
        treeHeader->setVisible(false);
#if QT_VERSION >= 0x050000
        treeHeader->setSectionResizeMode(QHeaderView::Stretch);
#else
        treeHeader->setResizeMode(QHeaderView::Stretch);
#endif
        treeHeader->setStretchLastSection(true);
    }
    setContextMenuPolicy(Qt::CustomContextMenu);
    setUniformRowHeights(true);
    setTextElideMode(Qt::ElideNone);
//        setExpandsOnDoubleClick(false);
    setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(this, SIGNAL(collapsed(QModelIndex)), this, SLOT(collapsedTree(QModelIndex)));
    connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(expandedTree(QModelIndex)));
}

void SymbolTreeView::focusInEvent(QFocusEvent *event)
{
    if (event->reason() != Qt::PopupFocusReason)
        QTreeView::focusInEvent(event);
}

void SymbolTreeView::focusOutEvent(QFocusEvent *event)
{
    if (event->reason() != Qt::PopupFocusReason)
        QTreeView::focusOutEvent(event);
}

#ifdef Q_WS_MAC
void SymbolTreeView::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Return
            || event->key() == Qt::Key_Enter)
            && event->modifiers() == 0
            && currentIndex().isValid()) {
        emit activated(currentIndex());
        return;
    }
    QTreeView::keyPressEvent(event);
}
#endif

QModelIndex SymbolTreeView::topViewIndex()
{
    return indexAt(QPoint(1,1));
}

void SymbolTreeView::expandedTree(const QModelIndex &index)
{
    m_expandIndexs.insert(index);
}

void SymbolTreeView::collapsedTree(const QModelIndex &index)
{
    m_expandIndexs.remove(index);
}

void SymbolTreeView::reset()
{
    QTreeView::reset();
    //setRootIndex(model()->index(0,0));
    //this->setRootIndex(QModelIndex());
    m_expandIndexs.clear();
}

const QSet<QModelIndex> & SymbolTreeView::expandIndexs() const
{
    return m_expandIndexs;
}

QSet<QModelIndex> & SymbolTreeView::expandIndexs()
{
    return m_expandIndexs;
}

void SymbolTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QTreeView::currentChanged(current,previous);
    emit currentIndexChanged(current, previous);
}

void SymbolTreeView::saveState(SymbolTreeState *state)
{
    if (!state) {
        return;
    }
    state->expands.clear();
    QSetIterator<QModelIndex> i(this->expandIndexs());
    while (i.hasNext()) {
        state->expands.append(stringListFromIndex(i.next()));
    }
    state->cur = stringListFromIndex(this->currentIndex());
    state->vbar = verticalScrollBar()->value();
    state->hbar = horizontalScrollBar()->value();

}

void SymbolTreeView::loadState(QAbstractItemModel *model,SymbolTreeState *state)
{
    //load state
    //this->expandToDepth(0);

    QListIterator<QStringList> ie(state->expands);
    while (ie.hasNext()) {
        QStringList expandPath = ie.next();
        QModelIndex expandIndex = indexFromStringList(model,expandPath);
        if (expandIndex.isValid()) {
            this->setExpanded(expandIndex,true);
        }
    }

    QModelIndex curIndex = indexFromStringList(model,state->cur);
    if (curIndex.isValid()) {
        this->setCurrentIndex(curIndex);
    }

    verticalScrollBar()->setValue(state->vbar);
    horizontalScrollBar()->setValue(state->hbar);
}
