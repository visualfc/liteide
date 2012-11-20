/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: pluginsdialog.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: pluginsdialog.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "pluginsdialog.h"
#include "ui_pluginsdialog.h"
#include "pluginmanager.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileInfo>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


PluginsDialog::PluginsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginsDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    m_model = new QStandardItemModel(0,6,this);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(1, Qt::Horizontal, "ID");
    m_model->setHeaderData(2, Qt::Horizontal, tr("Anchor"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Info"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Ver"));
    m_model->setHeaderData(5, Qt::Horizontal, tr("FileName"));
    ui->treeView->setModel(m_model);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView->setItemsExpandable(true);
    ui->treeView->setRootIsDecorated(false);
    ui->treeView->header()->setResizeMode(QHeaderView::ResizeToContents);
}

PluginsDialog::~PluginsDialog()
{
    delete ui;
}

void PluginsDialog::addPluginInfo(const LiteApi::PluginInfo *info, const QString &fileName)
{
    if (!info) {
        return;
    }
    QList<QStandardItem*> items;
    items.append(new QStandardItem(info->name()));
    items.append(new QStandardItem(info->id()));
    items.append(new QStandardItem(info->anchor()));
    items.append(new QStandardItem(info->info()));
    items.append(new QStandardItem(info->ver()));
    items.append(new QStandardItem(QFileInfo(fileName).fileName()));
    items.at(5)->setToolTip(fileName);
    m_model->appendRow(items);
}
