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
    init(true);
}


SymbolTreeView::SymbolTreeView(bool bResizeToContents, QWidget *parent)
    : QTreeView(parent)
{
    init(bResizeToContents);
}

void SymbolTreeView::init(bool bResizeToContents)
{
    m_bClickedItem = false;
    m_hsbPos = 0;
    setEditTriggers(QAbstractItemView::NoEditTriggers);
   // setFrameStyle(QFrame::NoFrame);
    setIndentation(indentation() * 9/10);
    {
        this->setHeaderHidden(true);
        if (bResizeToContents) {
#if QT_VERSION >= 0x050000
            this->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
            this->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
            this->header()->setStretchLastSection(false);
        }
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    setContextMenuPolicy(Qt::CustomContextMenu);
    setUniformRowHeights(true);
    setTextElideMode(Qt::ElideNone);
//        setExpandsOnDoubleClick(false);
    setAttribute(Qt::WA_MacShowFocusRect, false);
    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(clickedItem(QModelIndex)));
    connect(this->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(hsbValueChanged(int)));
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

QModelIndex SymbolTreeView::topViewIndex()
{
    return indexAt(QPoint(1,1));
}

void SymbolTreeView::reset()
{
    QTreeView::reset();
    //setRootIndex(model()->index(0,0));
    //this->setRootIndex(QModelIndex());
}

void SymbolTreeView::getTreeExpands(const QModelIndex &parent, QList<QModelIndex> &list) const
{
    for (int i = 0; i < this->model()->rowCount(parent); i++) {
        QModelIndex index = this->model()->index(i,0,parent);
        if (this->isExpanded(index)) {
            list.append(index);
            getTreeExpands(index,list);
        }
    }
}

void SymbolTreeView::clickedItem(QModelIndex)
{
    this->m_bClickedItem = true;
    this->m_hsbPos = this->horizontalScrollBar()->sliderPosition();
}

void SymbolTreeView::hsbValueChanged(int)
{
    if (this->m_bClickedItem) {
        this->m_bClickedItem = false;
        this->horizontalScrollBar()->setValue(this->m_hsbPos);
    }
}

QList<QModelIndex> SymbolTreeView::expandIndexs() const
{
    QList<QModelIndex> expands;
    getTreeExpands(QModelIndex(),expands);
    return expands;
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

    foreach (QModelIndex index, this->expandIndexs()) {
        state->expands.append(stringListFromIndex(index));
    }

    state->cur = stringListFromIndex(this->currentIndex());
    state->vbar = verticalScrollBar()->value();
    state->hbar = horizontalScrollBar()->value();
}

void SymbolTreeView::loadState(QAbstractItemModel *model,SymbolTreeState *state)
{
    //load state
    //this->expandToDepth(0)
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
    if (state->vbar != -1) {
        verticalScrollBar()->setValue(state->vbar);
    }
    if (state->hbar != -1) {
        horizontalScrollBar()->setValue(state->hbar);
    }
}

void SymbolTreeView::keyPressEvent(QKeyEvent *event)
{
    // Note: This always eats the event
    // whereas QAbstractItemView never eats it
    if ((event->key() == Qt::Key_Return
            || event->key() == Qt::Key_Enter)
            && event->modifiers() == 0
            && QTreeView::currentIndex().isValid()
            && QTreeView::state() != QAbstractItemView::EditingState) {
        emit QTreeView::activated(QTreeView::currentIndex());
        emit enterKeyPressed(QTreeView::currentIndex());
        return;
    }
    QTreeView::keyPressEvent(event);
}
