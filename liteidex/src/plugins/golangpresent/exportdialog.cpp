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
// Module: exportdialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "exportdialog.h"
#include "ui_exportdialog.h"
#include "memory.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog),
    m_bView(false)
{
    ui->setupUi(this);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

QString ExportDialog::fileName() const
{
    return ui->fileName->text();
}

void ExportDialog::setFileName(const QString &text)
{
    ui->fileName->setText(text);
}

void ExportDialog::on_exportButton_clicked()
{
    m_bView = false;
    this->accept();
}

void ExportDialog::on_exportAndViewButton_clicked()
{
    m_bView = true;
    this->accept();
}
