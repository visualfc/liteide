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
// Module: setupgopathdialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "setupgopathdialog.h"
#include "ui_setupgopathdialog.h"
#include <QFileDialog>
#include "memory.h"
SetupGopathDialog::SetupGopathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupGopathDialog)
{
    ui->setupUi(this);
    connect(ui->browserButton,SIGNAL(clicked()),this,SLOT(browser()));
    connect(ui->clearButton,SIGNAL(clicked()),ui->litePathTextEdit,SLOT(clear()));
}

SetupGopathDialog::~SetupGopathDialog()
{
    delete ui;
}


void SetupGopathDialog::setSysPathList(const QStringList &pathList)
{
    foreach (QString path, pathList) {
        ui->sysPathTextEdit->appendPlainText(path);
    }
}

void SetupGopathDialog::setLitePathList(const QStringList &pathList)
{
    foreach (QString path, pathList) {
        ui->litePathTextEdit->appendPlainText(path);
    }
}

QStringList SetupGopathDialog::litePathList() const
{
    return ui->litePathTextEdit->toPlainText().split("\n",QString::SkipEmptyParts);
}

void SetupGopathDialog::browser()
{
    static QString last = QDir::homePath();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose directory to add to GOPATH:"),
                                                    last,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        last = dir;
        ui->litePathTextEdit->appendPlainText(dir);
    }
}
