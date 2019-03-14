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
// Module: quickopenwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenwidget.h"
#include "liteapi/liteids.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QTreeView>
#include <QFocusEvent>
#include <QHeaderView>
#include <QDebug>
#include <QApplication>
#include <QToolBar>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QuickOpenWidget::QuickOpenWidget(LiteApi::IApplication *app, QWidget *parent) :
    QWidget(parent, Qt::Popup),// Qt::ToolTip | Qt::WindowStaysOnTopHint),
    m_liteApp(app)
{
    //this->setFocusPolicy(Qt::NoFocus);
    m_edit = new Utils::FilterLineEdit(300);
    m_view = new QTreeView;
    m_view->setHeaderHidden(true);
    m_view->setTextElideMode(Qt::ElideLeft);
#if QT_VERSION >= 0x050000
    m_view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_view->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_wrap = true;

    m_tmpToolBar = 0;

    m_layout = new QVBoxLayout;
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_edit);
    m_layout->addWidget(m_view);

    this->setMinimumWidth(600);
    this->setMinimumHeight(300);

    this->setLayout(m_layout);

    connect(m_edit,SIGNAL(filterChanged(QString)),this,SIGNAL(filterChanged(QString)));

    m_edit->installEventFilter(this);
}

void QuickOpenWidget::setModel(QAbstractItemModel *model,const QModelIndex &rootIndex)
{
    m_view->setModel(model);
    m_view->setRootIndex(rootIndex);
}

QLineEdit *QuickOpenWidget::editor()
{
    return m_edit;
}

QTreeView *QuickOpenWidget::view()
{
    return m_view;
}

void QuickOpenWidget::setTempToolBar(QToolBar *toolBar)
{
    if (m_tmpToolBar) {
        m_layout->removeWidget(m_tmpToolBar);
        m_tmpToolBar->hide();
    }
    m_tmpToolBar = toolBar;
    if (toolBar == 0) {
        return;
    }
    m_tmpToolBar->show();
    m_layout->insertWidget(0,m_tmpToolBar);
}

void QuickOpenWidget::hideEvent(QHideEvent *e)
{
    emit hideWidget();
    if (m_tmpToolBar) {
        m_layout->removeWidget(m_tmpToolBar);
        m_tmpToolBar->hide();
    }
    m_tmpToolBar = 0;
    QWidget::hideEvent(e);
}

void QuickOpenWidget::closeWidget()
{
    if (m_tmpToolBar) {
        m_layout->removeWidget(m_tmpToolBar);
    }
    m_tmpToolBar = 0;
    QWidget::close();
}

void QuickOpenWidget::showView(QPoint *pos)
{
    if (pos == 0) {
        QPoint pt(0,0);
        this->move(m_liteApp->editorManager()->widget()->mapToGlobal(pt));
    } else {
        this->move(pos->x(),pos->y());
    }
    m_edit->setFocus();
    this->show();
}

bool QuickOpenWidget::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        QAbstractItemModel *model = m_view->model();
        if (!model) {
            return false;
        }

        const int key = ke->key();
        switch (key) {
        case Qt::Key_Up: {
            QModelIndex index = m_view->indexAbove(m_view->currentIndex());
            if (!index.isValid()) {
                index = model->index(model->rowCount(m_view->rootIndex())-1,0,m_view->rootIndex());
            }

            m_view->setCurrentIndex(index);
            emit indexChanage(index);
            return true;
        }
        case Qt::Key_Down: {
            QModelIndex index = m_view->indexBelow(m_view->currentIndex());
            if (!index.isValid()) {
                index = model->index(0,0,m_view->rootIndex());
            }
            m_view->setCurrentIndex(index);
            emit indexChanage(index);
            return true;
        }
//        case Qt::Key_Return: {
//            QModelIndex index = m_view->currentIndex();
//            if (model->hasChildren(index)) {
//                m_view->setExpanded(index,!m_view->isExpanded(index));
//            } else {
//                emit indexEnter(index);
//            }
//            return true;
//        }
        }
    } else if (e->type() == QEvent::FocusOut) {
        if (QWidget::focusWidget() == m_view ) {
            m_edit->setFocus();
        }
        return true;
    }
    return QWidget::eventFilter(o,e);
}
