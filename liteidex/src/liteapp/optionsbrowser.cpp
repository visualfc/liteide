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
// Module: optionsbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: optionsbrowser.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "optionsbrowser.h"
#include "ui_optionswidget.h"

#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


OptionsBrowser::OptionsBrowser(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IBrowserEditor(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::OptionsWidget)
{
    ui->setupUi(m_widget);
    connect(ui->listWidget,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));
    connect(ui->applayButton,SIGNAL(clicked()),this,SLOT(applayButton()));
}

OptionsBrowser::~OptionsBrowser()
{
    delete ui;
    delete m_widget;
}

QWidget *OptionsBrowser::widget()
{
    return m_widget;
}

QString OptionsBrowser::name() const
{
    return tr("Options");
}

QString OptionsBrowser::mimeType() const
{
    return "browser/options";
}

void OptionsBrowser::addOption(LiteApi::IOption *opt)
{
    if (!opt || !opt->widget()) {
        return;
    }

    QListWidgetItem *item = new QListWidgetItem;
    item->setIcon(opt->icon());
    item->setText(opt->name());
    item->setTextAlignment(Qt::AlignLeft);// | Qt::AlignHCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    ui->listWidget->addItem(item);
    ui->stackedWidget->addWidget(opt->widget());
    m_widgetOptionMap.insert(item,opt);
    if (ui->listWidget->count() == 1) {
        ui->listWidget->setCurrentItem(item);
    }
}

void OptionsBrowser::itemSelectionChanged()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) {
        return;
    }
    LiteApi::IOption *opt = m_widgetOptionMap.value(item);
    if (opt) {
        ui->stackedWidget->setCurrentWidget(opt->widget());
        ui->infoLabel->setText(QString("Name : %1    MimeType : %2").arg(opt->name()).arg(opt->mimeType()));
    }
}

void OptionsBrowser::applayButton()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) {
        return;
    }
    LiteApi::IOption *opt = m_widgetOptionMap.value(item);
    if (opt) {
        opt->apply();
        emit applyOption(opt->mimeType());
    }
}
