/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: tableviewwidget.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: tableviewwidget.cpp,v 1.0 2011-7-25 visualfc Exp $

#include "tableviewwidget.h"
#include "ui_tableviewwidget.h"

#include "sqltablemodel.h"
#include "sqlitemdelegate.h"
#include <QTableWidgetItem>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

TableViewWidget::TableViewWidget(QSqlDatabase &db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableViewWidget)
{
    ui->setupUi(this);
    m_model = new SqlTableModel(this,db);
    m_delegate = new SqlItemDelegate(this);

    ui->tableView->setModel(m_model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setItemDelegate(m_delegate);
}

TableViewWidget::~TableViewWidget()
{
    delete ui;
}

void TableViewWidget::setTable(const QString &table)
{
    m_model->setTable(table);
    m_model->select();

    if (m_model->columnCount() <= 0)
            return;

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    int total = 0;
    for (int i = 0; i < m_model->columnCount(); ++i)
        total += ui->tableView->columnWidth(i);

    if (total < ui->tableView->viewport()->width())
    {
        int extra = (ui->tableView->viewport()->width() - total)
                / m_model->columnCount();
        for (int i = 0; i < m_model->columnCount(); ++i)
            ui->tableView->setColumnWidth(i, ui->tableView->columnWidth(i) + extra);
    }
}
