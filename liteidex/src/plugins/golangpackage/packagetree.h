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
// Module: packagetree.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef PACKAGETREE_H
#define PACKAGETREE_H

#include "symboltreeview/symboltreeview.h"

class QStandardItemModel;
class QStandardItem;
class PackageTree : public SymbolTreeView
{
    Q_OBJECT
public:
    explicit PackageTree(QWidget *parent = 0);
    void loadJson(const QMap<QString,QVariant> &json);
public:
    QStandardItem *root;
    QStandardItem *src;
    QStandardItem *imports;
    QStandardItem *deps;
    QStandardItemModel *model;
    QStringList fileList;
    QStringList nameList;
};

#endif // PACKAGETREE_H
