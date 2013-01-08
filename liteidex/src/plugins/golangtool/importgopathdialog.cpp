/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: importgopathdialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "importgopathdialog.h"
#include "ui_importgopathdialog.h"

#include <QFileDialog>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

ImportGopathDialog::ImportGopathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportGopathDialog)
{
    ui->setupUi(this);
    connect(ui->browserButton,SIGNAL(clicked()),this,SLOT(browser()));
    connect(ui->clearButton,SIGNAL(clicked()),ui->pathTextEdit,SLOT(clear()));
}

ImportGopathDialog::~ImportGopathDialog()
{
    delete ui;
}

void ImportGopathDialog::setSysPathList(const QStringList &pathList)
{
    foreach (QString path, pathList) {
        ui->sysPathTextEdit->appendPlainText(path);
    }
}

void ImportGopathDialog::setPathList(const QStringList &pathList)
{
    foreach (QString path, pathList) {
        ui->pathTextEdit->appendPlainText(path);
    }
}

QStringList ImportGopathDialog::pathList() const
{
    return ui->pathTextEdit->toPlainText().split("\n",QString::SkipEmptyParts);
}

void ImportGopathDialog::browser()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Load GOPATH Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui->pathTextEdit->appendPlainText(dir);
    }
}
