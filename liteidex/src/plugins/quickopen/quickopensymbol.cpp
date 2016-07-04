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
// Module: quickopensymbol.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopensymbol.h"
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

QuickOpenSymbol::QuickOpenSymbol(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpenSymbol(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_model->appendRow(new QStandardItem("not find symbol"));
    m_id = "quickopen/symbol";
    m_info = tr("Open Symbol by Name");
}

QString QuickOpenSymbol::id() const
{
    return m_id;
}

QString QuickOpenSymbol::info() const
{
    return m_info;
}

void QuickOpenSymbol::activate()
{
    QString mimeType;
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor) {
        mimeType = editor->mimeType();
    }
    foreach (LiteApi::IDocumentSymbolFactory *factory, m_factoryList) {
        LiteApi::IDocumentSymbol *symbol = factory->create(mimeType);
        if (symbol) {
            m_symbol = symbol;
            break;
        }
    }
}

QAbstractItemModel *QuickOpenSymbol::model() const
{
    if (m_symbol) {
        return m_symbol->model();
    }
    return m_model;
}

void QuickOpenSymbol::updateModel()
{
    if (m_symbol) {
        m_symbol->updateModel();
    }
}

QModelIndex QuickOpenSymbol::filter(const QString &text)
{
    if (m_symbol) {
        return m_symbol->filter(text);
    }
    return QModelIndex();
}

bool QuickOpenSymbol::selected(const QString &text, const QModelIndex &index)
{
    if (m_symbol) {
        return m_symbol->selected(text,index);
    }
    return false;
}

void QuickOpenSymbol::addFactory(LiteApi::IDocumentSymbolFactory *factory)
{
    m_factoryList.push_back(factory);
}

void QuickOpenSymbol::setId(const QString &id)
{
    m_id = id;
}

void QuickOpenSymbol::setInfo(const QString &info)
{
    m_info = info;
}
