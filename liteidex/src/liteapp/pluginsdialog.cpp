/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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


PluginsDialog::PluginsDialog(LiteApi::IApplication *app, QWidget *parent) :
    QDialog(parent),
    m_liteApp(app),
    ui(new Ui::PluginsDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    m_model = new QStandardItemModel(0,6,this);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Enabled"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Details"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Author"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Version"));
    m_model->setHeaderData(5, Qt::Horizontal, tr("File Name"));
    ui->treeView->setModel(m_model);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView->setItemsExpandable(true);
    ui->treeView->setRootIsDecorated(false);
#if QT_VERSION >= 0x050000
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->treeView->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    connect(m_model,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(itemChanged(QStandardItem*)));
}

PluginsDialog::~PluginsDialog()
{
    delete ui;
}

void PluginsDialog::appendInfo(const LiteApi::PluginInfo *info)
{
    if (!info) {
        return;
    }
    QList<QStandardItem*> items;
    items.append(new QStandardItem(info->name()));
    QStandardItem *load = new QStandardItem();
    load->setCheckable(true);
    load->setData(info->id());
    if (info->isMustLoad()) {
        load->setEnabled(false);
    }
    bool b = m_liteApp->settings()->value(QString("liteapp/%1_load").arg(info->id()),true).toBool();
    if (b) {
        load->setCheckState(Qt::Checked);
    } else {
        load->setCheckState(Qt::Unchecked);
    }
    items.append(load);
    items.append(new QStandardItem(info->info()));
    items.append(new QStandardItem(info->anchor()));
    items.append(new QStandardItem(info->ver()));
    items.append(new QStandardItem(QFileInfo(info->filePath()).fileName()));
    items.at(5)->setToolTip(info->filePath());
    m_model->appendRow(items);
}

void PluginsDialog::itemChanged(QStandardItem *item)
{
    if (item && item->isCheckable()) {
        QString id = item->data(Qt::UserRole+1).toString();
        if (!id.isEmpty()) {
            m_liteApp->settings()->setValue(QString("liteapp/%1_load").arg(id),item->checkState() == Qt::Checked);
        }
    }
}
