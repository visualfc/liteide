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
// Module: quickopenmimetype.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenmimetype.h"
#include <QStandardItemModel>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QuickOpenMimeType::QuickOpenMimeType(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpenMimeType(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_id = "quickopen/symbol";
    m_info = tr("Go to Symbol in File");
    m_message = tr("not found symbol");
}

QString QuickOpenMimeType::id() const
{
    return m_id;
}

QString QuickOpenMimeType::info() const
{
    return m_info;
}

QString QuickOpenMimeType::placeholderText() const
{
    return QString();
}

void QuickOpenMimeType::activate()
{
    QString mimeType;
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor) {
        mimeType = editor->mimeType();
    }
    foreach (LiteApi::IQuickOpenAdapter *factory, m_adapterList) {
        LiteApi::IQuickOpen *symbol = factory->load(mimeType);
        if (symbol) {
            m_symbol = symbol;
            m_symbol->activate();
            break;
        }
    }
}

QAbstractItemModel *QuickOpenMimeType::model() const
{
    if (m_symbol) {
        return m_symbol->model();
    }
    return m_model;
}

void QuickOpenMimeType::updateModel()
{
    if (m_symbol) {
        m_symbol->updateModel();
    } else {
        m_model->clear();
        m_model->appendRow(new QStandardItem(m_message));
    }
}

QModelIndex QuickOpenMimeType::filterChanged(const QString &text)
{
    if (m_symbol) {
        return m_symbol->filterChanged(text);
    }
    return QModelIndex();
}

void QuickOpenMimeType::indexChanged(const QModelIndex &index)
{
    if (m_symbol) {
        m_symbol->indexChanged(index);
    }
}

bool QuickOpenMimeType::selected(const QString &text, const QModelIndex &index)
{
    if (m_symbol) {
        return m_symbol->selected(text,index);
    }
    return false;
}

void QuickOpenMimeType::addAdapter(LiteApi::IQuickOpenAdapter *factory)
{
    m_adapterList.push_back(factory);
}

void QuickOpenMimeType::setId(const QString &id)
{
    m_id = id;
}

void QuickOpenMimeType::setInfo(const QString &info)
{
    m_info = info;
}

void QuickOpenMimeType::setNoFoundMessage(const QString &message)
{
    m_message = message;
}

void QuickOpenMimeType::cancel()
{
    if (m_symbol) {
        m_symbol->cancel();
    }
}
