/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: selectexternaldialog.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "selectexternaldialog.h"
#include "ui_selectexternaldialog.h"
#include <QFileDialog>
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

SelectExternalDialog::SelectExternalDialog(LiteApi::IApplication *app, QWidget *parent) :
    QDialog(parent),
    m_liteApp(app),ui(new Ui::SelectExternalDialog)
{
    ui->setupUi(this);
}

SelectExternalDialog::~SelectExternalDialog()
{

    delete ui;
}

QString SelectExternalDialog::getCmd() const
{
    return ui->cmdLineEdit->text();
}

QString SelectExternalDialog::getArgs() const
{
    return ui->argsLineEdit->text();
}

QString SelectExternalDialog::getWork() const
{
    return ui->workLineEdit->text();
}

void SelectExternalDialog::loadSetting()
{
    ui->cmdLineEdit->setText(m_liteApp->settings()->value("litedebug/external/cmd").toString());
    ui->argsLineEdit->setText(m_liteApp->settings()->value("litedebug/external/args").toString());
    ui->workLineEdit->setText(m_liteApp->settings()->value("litedebug/external/work").toString());
}

void SelectExternalDialog::saveSetting()
{
    m_liteApp->settings()->setValue("litedebug/external/cmd",getCmd());
    m_liteApp->settings()->setValue("litedebug/external/args",getArgs());
    m_liteApp->settings()->setValue("litedebug/external/work",getWork());
}

void SelectExternalDialog::on_cmbPushButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Select Executable"));
    if (!filePath.isEmpty()) {
        ui->cmdLineEdit->setText(filePath);
        QFileInfo info(filePath);
        ui->workLineEdit->setText(info.path());
    }
}

void SelectExternalDialog::on_workPushButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,tr("Select the working directory:"));
    if (!path.isEmpty()) {
        ui->workLineEdit->setText(path);
    }
}
