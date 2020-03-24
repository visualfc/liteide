/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: openeditorswidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "openeditorswidget.h"
#include <QCoreApplication>
#include <QSortFilterProxyModel>
#include <QHeaderView>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

OpenEditorsWidget::OpenEditorsWidget(LiteApi::IApplication *app)
    : m_liteApp(app)
{
    //m_model = new Core::ProxyModel(this);

    setWindowTitle(tr("Open Documents"));
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setCloseButtonVisible(true);

    m_model = new QSortFilterProxyModel(this);
    this->setModel(m_model);

    setContextMenuPolicy(Qt::CustomContextMenu);

    this->setEditTriggers(QTreeView::NoEditTriggers);

    this->header()->setCascadingSectionResizes( true );
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(updateCurrentItem(LiteApi::IEditor*)));
    connect(this,SIGNAL(activated(QModelIndex)),this,SLOT(handleActivated(QModelIndex)));
    connect(this,SIGNAL(closeActivated(QModelIndex)),this,SLOT(closeDocument(QModelIndex)));
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuRequested(QPoint)));
}

void OpenEditorsWidget::setEditorModel(QAbstractItemModel *model)
{
    m_model->setSourceModel(model);
}

void OpenEditorsWidget::handleActivated(const QModelIndex &index)
{
    if (index.column() == 0) {
        activateEditor(index);
    } else if (index.column() == 1) { // the funky close button
        closeDocument(index);

        // work around a bug in itemviews where the delegate wouldn't get the QStyle::State_MouseOver
        QPoint cursorPos = QCursor::pos();
        QWidget *vp = viewport();
        QMouseEvent e(QEvent::MouseMove, vp->mapFromGlobal(cursorPos), cursorPos, Qt::NoButton, 0, 0);
        QCoreApplication::sendEvent(vp, &e);
    }
}

void OpenEditorsWidget::updateCurrentItem(LiteApi::IEditor *editor)
{
    QModelIndex index = indexFromEditor(editor);
    if (!index.isValid()) {
        clearSelection();
        return;
    }
    setCurrentIndex(index);
    selectionModel()->select(currentIndex(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    scrollTo(currentIndex());
}

void OpenEditorsWidget::contextMenuRequested(QPoint /*pos*/)
{

}

void OpenEditorsWidget::activateEditor(const QModelIndex &index)
{
    LiteApi::IEditor *editor = editorFormIndex(index);
    if (editor) {
        m_liteApp->editorManager()->setCurrentEditor(editor);
    }
}

void OpenEditorsWidget::closeDocument(const QModelIndex &index)
{
    LiteApi::IEditor *editor = editorFormIndex(index);
    if (editor) {
        m_liteApp->editorManager()->closeEditor(editor);
    }
    updateCurrentItem(m_liteApp->editorManager()->currentEditor());
}

LiteApi::IEditor *OpenEditorsWidget::editorFormIndex(const QModelIndex &index)
{
    QString filePath = index.data(Qt::ToolTipRole).toString();
    if (filePath.isEmpty()) {
        return 0;
    }
    return m_liteApp->editorManager()->findEditor(filePath,true);
}

QModelIndex OpenEditorsWidget::indexFromEditor(LiteApi::IEditor *editor)
{
    QModelIndex r;
    if (!editor) {
        return r;
    }
    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return r;
    }
    for (int i = 0; i < model()->rowCount(); i++) {
        QModelIndex index = model()->index(i,0);
        if (index.data(Qt::ToolTipRole).toString() == filePath) {
            return index;
        }
    }
    return r;
}
