/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
    QWidget(parent,Qt::Popup),// Qt::ToolTip | Qt::WindowStaysOnTopHint)
    m_liteApp(app)
{
    //this->setFocusPolicy(Qt::NoFocus);
    m_edit = new Utils::FancyLineEdit;
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

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_edit);
    layout->addWidget(m_view);

    this->setMinimumWidth(600);

    this->setLayout(layout);

    m_edit->installEventFilter(this);
}

void QuickOpenWidget::setModel(QAbstractItemModel *model)
{
    m_view->setModel(model);
}

QLineEdit *QuickOpenWidget::editor()
{
    return m_edit;
}

QTreeView *QuickOpenWidget::view()
{
    return m_view;
}

void QuickOpenWidget::hideEvent(QHideEvent *e)
{
    emit hidePopup();
    QWidget::hideEvent(e);
}

void QuickOpenWidget::showView()
{
    QToolBar *toolBar =  m_liteApp->actionManager()->loadToolBar(ID_TOOLBAR_STD);
    QRect rc = toolBar->frameGeometry();
    QPoint pt = rc.topRight();
    pt.rx() += 4;
    pt.ry() += 2;
    this->move(m_liteApp->mainWindow()->mapToGlobal(pt));
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

        int row = m_view->currentIndex().row();

        const int key = ke->key();
        switch (key) {
        case Qt::Key_Up: {
            row--;
            if (row < 0) {
                if (m_wrap) {
                    row = model->rowCount()-1;
                }
            }
            QModelIndex index = model->index(row,0);
            m_view->setCurrentIndex(index);
            emit indexChanage(index);
            return true;
        }
        case Qt::Key_Down: {
            row++;
            if (row >= model->rowCount()) {
                if (m_wrap) {
                    row = 0;
                }
            }
            QModelIndex index = model->index(row,0);
            m_view->setCurrentIndex(index);
            emit indexChanage(index);
            return true;
        }
        }
    } else if (e->type() == QEvent::FocusOut) {
        if (QWidget::focusWidget() == m_view ) {
            m_edit->setFocus();
        }
        return true;
    }
    return QWidget::eventFilter(o,e);
}
