/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: sqltablemodel.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "sqltablemodel.h"
#include <QColor>
#include <QDebug>
#include <QSqlRecord>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

SqlTableModel::SqlTableModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent,db)
{
    m_blobColor = QColor(Qt::darkYellow);
}

QVariant SqlTableModel::data(const QModelIndex &idx, int role) const
{
    QVariant var = QSqlTableModel::data(idx,Qt::DisplayRole);
    if (var.isNull()) {
        if (role == Qt::BackgroundColorRole)
            return QVariant(m_blobColor);
        if (role == Qt::ToolTipRole)
            return QVariant(tr("NULL value"));
        if (role == Qt::DisplayRole)
            return QVariant(tr("{null}"));
        if (role == Qt::EditRole)
            return QSqlTableModel::data(idx, Qt::DisplayRole);
        if (role == Qt::UserRole+1)
            return QVariant("{blob}");

    } else if (var.type() == QVariant::ByteArray) {
        if (role == Qt::BackgroundColorRole)
            return QVariant(m_blobColor);
        if (role == Qt::ToolTipRole)
            return QVariant(tr("BLOB value"));
        if (role == Qt::DisplayRole)
            return QVariant(tr("{blob}"));
        if (role == Qt::EditRole)
            return QSqlTableModel::data(idx, Qt::DisplayRole);
        if (role == Qt::UserRole+1)
            return QVariant("{blob}");

    }

    return QSqlTableModel::data(idx,role);
}
