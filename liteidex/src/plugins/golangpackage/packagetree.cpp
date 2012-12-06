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
// Module: packagetree.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-19
// $Id: packagetree.cpp,v 1.0 2012-4-20 visualfc Exp $

#include "packagetree.h"

#include <QStandardItem>
#include <QStandardItemModel>
#include <QDir>
#include <QFileInfo>
#include "packagebrowser.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

PackageTree::PackageTree(QWidget *parent) :
    SymbolTreeView(parent),
    root(0)
{
    model = new QStandardItemModel(this);
    this->setModel(model);
}

void PackageTree::loadJson(const QMap<QString, QVariant> &jsonMap)
{
    SymbolTreeState state;
    this->saveState(&state);
    model->clear();
    QStandardItem *item = new QStandardItem(jsonMap.value("ImportPath").toString());

    QStandardItem *base = new QStandardItem("BaseInfo");
    item->appendRow(base);
    QDir dir(jsonMap.value("Dir").toString());
    foreach (QString key, jsonMap.keys()) {
        QVariant var = jsonMap.value(key);
        if (key.indexOf("Error") >= 0) {
            QString text = QString("%1 : true").arg(key);
            QStandardItem *ic = new QStandardItem(text);
            ic->setToolTip(text);
            base->appendRow(ic);
            continue;
        }
        if (var.type() == QVariant::String ||
            var.type() == QVariant::Bool) {
            QString text = QString("%1 : %2").arg(key).arg(var.toString());
            QStandardItem *ic = new QStandardItem(text);
            ic->setToolTip(text);
            base->appendRow(ic);
        } else if (var.type() == QVariant::List) {
            PackageType::ITEM_TYPE type = PackageType::ITEM_NONE;
            if (key.indexOf("Deps") >= 0) {
                type = PackageType::ITEM_DEP;
            } else if (key.indexOf("Imports") >= 0) {
                type = PackageType::ITEM_IMPORT;
            } else if (key.indexOf("Files") >= 0) {
                type = PackageType::ITEM_SOURCE;
            }
            QStandardItem *ic = new QStandardItem(key);

            foreach(QVariant v, var.toList()) {
                if (v.type() == QVariant::String) {
                    QStandardItem *iv = new QStandardItem(v.toString());
                    iv->setData(type,PackageType::RoleItem);
                    if (type == PackageType::ITEM_SOURCE) {
                        iv->setData(QFileInfo(dir,v.toString()).filePath(),PackageType::RolePath);
                    }
                    ic->appendRow(iv);
                }
            }
            item->appendRow(ic);
        }
    }

    model->appendRow(item);

    this->loadState(this->model,&state);

    fileList.clear();
    nameList.clear();

    QStringList nameFilter;
    nameFilter << "*.go" << "*.h" << "*.c" << "*.cpp" << "*.s";
    foreach(QFileInfo info, dir.entryInfoList(nameFilter,QDir::Files,QDir::Type|QDir::Name)) {
        fileList.append(info.filePath());
        nameList.append(info.fileName());
    }
}
