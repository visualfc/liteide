/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE. All rights reserved.
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
// Module: tableviewwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TABLEVIEWWIDGET_H
#define TABLEVIEWWIDGET_H

#include <QWidget>
#include <QSqlDatabase>

namespace Ui {
    class TableViewWidget;
}

class SqlTableModel;
class SqlItemDelegate;
class TableViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableViewWidget(QSqlDatabase &db, QWidget *parent = 0);
    ~TableViewWidget();
    void setTable(const QString &table);
private:
    Ui::TableViewWidget *ui;
    SqlTableModel      *m_model;
    SqlItemDelegate    *m_delegate;
};

#endif // TABLEVIEWWIDGET_H
