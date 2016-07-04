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
// Module: quickopeneditor.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopeneditor.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QStandardItem>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QuickOpenEditor::QuickOpenEditor(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
}

QString QuickOpenEditor::id() const
{
    return "quickopen/editor";
}

QString QuickOpenEditor::info() const
{
    return tr("Open Editor by Name");
}

QAbstractItemModel *QuickOpenEditor::model() const
{
    return m_proxyModel;
}

void QuickOpenEditor::updateModel()
{
    m_model->clear();
    m_proxyModel->setFilterFixedString("");
    m_proxyModel->setFilterKeyColumn(2);

    foreach(LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
        if (editor->filePath().isEmpty()) {
            continue;
        }
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem(editor->name()) << new QStandardItem(editor->filePath()) );
    }
    m_proxyModel->sort(0);
}

QModelIndex QuickOpenEditor::filter(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);

    for(int i = 0; i < m_proxyModel->rowCount(); i++) {
        QModelIndex index = m_proxyModel->index(i,1);
        QString name = index.data().toString();
        if (name.contains(text)) {
            return index;
        }
    }
    if (m_proxyModel->rowCount() > 0)
        return m_proxyModel->index(0,0);
    return QModelIndex();
}

bool QuickOpenEditor::selected(const QString &text, const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QString filePath = m_proxyModel->index(index.row(),1).data().toString();
    if (!m_liteApp->fileManager()->openFile(filePath)) {
        return false;
    }
    return true;
}
