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
// Module: optionsbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

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


OptionsBrowser::OptionsBrowser(LiteApi::IApplication *app, QWidget *parent) :
    QDialog(parent),
    m_liteApp(app),
    ui(new Ui::OptionsWidget)
{
    ui->setupUi(this);
    connect(ui->listWidget,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));
    connect(ui->buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(clicked(QAbstractButton*)));
}

OptionsBrowser::~OptionsBrowser()
{
    delete ui;
    //delete m_widget;
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
    QLayout *layout = opt->widget()->layout();
    if (layout) {
        layout->setMargin(0);
    }
    ui->listWidget->addItem(item);
    ui->stackedWidget->addWidget(opt->widget());
    m_widgetOptionMap.insert(item,opt);
}

int OptionsBrowser::execute()
{
    if (ui->listWidget->count() >= 1) {
        ui->listWidget->setCurrentItem(ui->listWidget->item(0));
        this->setMinimumHeight(600);
#ifdef Q_OS_MAC
        this->setMinimumWidth(900);
#endif
    }
    return exec();
}

void OptionsBrowser::itemSelectionChanged()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) {
        return;
    }
    LiteApi::IOption *opt = m_widgetOptionMap.value(item);
    if (opt) {
        opt->active();
        ui->stackedWidget->setCurrentWidget(opt->widget());
        ui->infoLabel->setText(QString("Name : %1    MimeType : %2").arg(opt->name()).arg(opt->mimeType()));
        opt->widget()->updateGeometry();
    }
}

void OptionsBrowser::clicked(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole(button);
    if (role == QDialogButtonBox::AcceptRole) {
        this->applay();
        this->accept();
    } else if (role == QDialogButtonBox::RejectRole) {
        this->reject();
    } else if (role == QDialogButtonBox::ApplyRole) {
        this->applay();
    }
}

void OptionsBrowser::applay()
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
