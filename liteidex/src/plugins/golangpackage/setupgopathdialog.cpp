/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
#include "golangpackage_global.h"
#include <QFileDialog>
#include <QLineEdit>
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
    ui->cmbGoProxy->setEnabled(false);
    ui->editGoPrivate->setEnabled(false);
    ui->editGoNoProxy->setEnabled(false);
    ui->editGoNoSumdb->setEnabled(false);

    ui->cmbGoProxy->addItems(QStringList()
                            << "https://proxy.golang.org,direct"
                            << "https://goproxy.io"
                            << "https://goproxy.io,direct"
                            << "https://goproxy.cn"
                            << "https://goproxy.cn,direct");
    connect(ui->browserButton,SIGNAL(clicked()),this,SLOT(browser()));
    connect(ui->clearButton,SIGNAL(clicked()),ui->litePathTextEdit,SLOT(clear()));
    connect(ui->chkUseSysGopath,SIGNAL(toggled(bool)),ui->sysPathTextEdit,SLOT(setEnabled(bool)));
    connect(ui->chkUseLiteGopath,SIGNAL(toggled(bool)),ui->litePathTextEdit,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoModule,SIGNAL(toggled(bool)),ui->cmbGoModule,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoProxy,SIGNAL(toggled(bool)),ui->cmbGoProxy,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoPrivate,SIGNAL(toggled(bool)),ui->editGoPrivate,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoNoProxy,SIGNAL(toggled(bool)),ui->editGoNoProxy,SLOT(setEnabled(bool)));
    connect(ui->chkUseGoNoSumdb,SIGNAL(toggled(bool)),ui->editGoNoSumdb,SLOT(setEnabled(bool)));
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
    for (int i = 0; i < ui->cmbGoModule->count(); i++) {
        if (ui->cmbGoModule->itemText(i) == value) {
            ui->cmbGoModule->setCurrentIndex(i);
            break;
        }
    }
}

QString SetupGopathDialog::go111Module() const
{
    return ui->cmbGoModule->currentText();
}

QStringList SetupGopathDialog::litePathList() const
{
    return ui->litePathTextEdit->toPlainText().split("\n",QString::SkipEmptyParts);
}

void SetupGopathDialog::setUseGoProxy(bool b)
{
    ui->chkUseGoProxy->setChecked(b);
}

bool SetupGopathDialog::isUseGoProxy() const
{
    return ui->chkUseGoProxy->isChecked();
}

void SetupGopathDialog::setGoProxy(const QString &v)
{
    ui->cmbGoProxy->lineEdit()->setText(v);
}

QString SetupGopathDialog::goProxy() const
{
    return ui->cmbGoProxy->currentText();
}

void SetupGopathDialog::setUseGoPrivate(ENUM_GO_PRIVATE id, bool b)
{
    switch (id) {
    case GO_PRIVATE:
        ui->chkUseGoPrivate->setChecked(b);
        break;
    case GO_NOPROXY:
        ui->chkUseGoNoProxy->setChecked(b);
        break;
    case GO_NOSUMDB:
        ui->chkUseGoNoSumdb->setChecked(b);
        break;
    }
}

bool SetupGopathDialog::isUseGoPrivate(ENUM_GO_PRIVATE id) const
{
    switch (id) {
    case GO_PRIVATE:
        return  ui->chkUseGoPrivate->isChecked();
    case GO_NOPROXY:
        return  ui->chkUseGoNoProxy->isChecked();
    case GO_NOSUMDB:
        return  ui->chkUseGoNoSumdb->isChecked();
    }
    return  false;
}

void SetupGopathDialog::setGoPrivate(ENUM_GO_PRIVATE id, const QString &value)
{
    switch (id) {
    case GO_PRIVATE:
        ui->editGoPrivate->setText(value);
        break;
    case GO_NOPROXY:
        ui->editGoNoProxy->setText(value);
        break;
    case GO_NOSUMDB:
        ui->editGoNoSumdb->setText(value);
        break;
    }
}

QString SetupGopathDialog::goPrivate(ENUM_GO_PRIVATE id) const
{
    switch (id) {
    case GO_PRIVATE:
        return  ui->editGoPrivate->text();
    case GO_NOPROXY:
        return  ui->editGoNoProxy->text();
    case GO_NOSUMDB:
        return  ui->editGoNoSumdb->text();
    }
    return  QString();
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
