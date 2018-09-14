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
// Module: setupgopathdialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "setupgopathdialog.h"
#include "ui_setupgopathdialog.h"
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

SetupGopathDialog::SetupGopathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupGopathDialog)
{
    ui->setupUi(this);
    ui->cmbGoModule->addItems(QStringList() << "auto" << "on" << "off");
    ui->cmbGoModule->setCurrentIndex(0);
    ui->cmbGoModule->setEnabled(false);
    connect(ui->browserButton,SIGNAL(clicked()),this,SLOT(browser()));
    connect(ui->clearButton,SIGNAL(clicked()),ui->litePathTextEdit,SLOT(clear()));
    connect(ui->chkUseSysGopath,SIGNAL(toggled(bool)),ui->sysPathTextEdit,SLOT(setEnabled(bool)));
    connect(ui->chkUseLiteGopath,SIGNAL(toggled(bool)),ui->litePathTextEdit,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoModule,SIGNAL(toggled(bool)),ui->cmbGoModule,SLOT(setEnabled(bool)));
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

void SetupGopathDialog::setUseSysGopath(bool b)
{
    ui->chkUseSysGopath->setChecked(b);
}

bool SetupGopathDialog::isUseSysGopath() const
{
    return ui->chkUseSysGopath->isChecked();
}

void SetupGopathDialog::setUseLiteGopath(bool b)
{
    ui->chkUseLiteGopath->setChecked(b);
}

bool SetupGopathDialog::isUseLiteGopath() const
{
    return ui->chkUseLiteGopath->isChecked();
}

void SetupGopathDialog::setUseGoModule(bool b)
{
    ui->chkUseGoModule->setChecked(b);
}

bool SetupGopathDialog::isUseGoModule() const
{
    return ui->chkUseGoModule->isChecked();
}

void SetupGopathDialog::setSysGoModuleInfo(const QString &value)
{
    ui->sysGoModuleInfo->setText(value);
}

void SetupGopathDialog::setGo111Module(const QString &value)
{
    ui->cmbGoModule->setCurrentText(value);
}

QString SetupGopathDialog::go111Module() const
{
    return ui->cmbGoModule->currentText();
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

