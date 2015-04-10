/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: aboutdialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "memory.h"

AboutDialog::AboutDialog(LiteApi::IApplication *app,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->verLabel->setText(app->ideVersion());
    ui->qtVerLabel->setText(QString(tr("Based on Qt %1 (%2 bit)")).arg(QLatin1String(qVersion())).
                            arg(QString::number(QSysInfo::WordSize)));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
