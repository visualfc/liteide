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
// Module: quickopenhelp.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenhelp.h"
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

QuickOpenHelp::QuickOpenHelp(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
}

QString QuickOpenHelp::id() const
{
    return "quickopen/help";
}

QString QuickOpenHelp::info() const
{
    return tr("Show Quick Open Help");
}

void QuickOpenHelp::activate()
{

}

QAbstractItemModel *QuickOpenHelp::model() const
{
    return m_model;
}

void QuickOpenHelp::updateModel()
{
    LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
    m_model->clear();
    QMapIterator<QString,LiteApi::IQuickOpen*> i(mgr->filterMap());
    while(i.hasNext()) {
        i.next();
        if (i.value() == this) {
            continue;
        }
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem(i.key()) << new QStandardItem(i.value()->info()));
    }
}

QModelIndex QuickOpenHelp::filterChanged(const QString &text)
{
    if (!text.isEmpty()) {
        LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
        LiteApi::IQuickOpen *p = mgr->findBySymbol(text);
        if (p) {
            mgr->showBySymbol(text);
        }
    }
    return m_model->index(0,0);
}

void QuickOpenHelp::indexChanged(const QModelIndex &/*index*/)
{
}

bool QuickOpenHelp::selected(const QString &text, const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QString key = index.data(0).toString();
    LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
    mgr->showBySymbol(key);
    return false;
}
