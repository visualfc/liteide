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
// Module: golangfmtoption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangfmtoption.h"
#include "golangfmt_global.h"
#include "ui_golangfmtoption.h"
#include "memory.h"

GolangFmtOption::GolangFmtOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GolangFmtOption)
{
    ui->setupUi(m_widget);

    bool goimports = m_liteApp->settings()->value(GOLANGFMT_USEGOIMPORTS,false).toBool();
    bool diff = m_liteApp->settings()->value(GOLANGFMT_USEDIFF,true).toBool();
    bool autofmt = m_liteApp->settings()->value(GOLANGFMT_AUTOFMT,true).toBool();
    bool syncfmt = m_liteApp->settings()->value(GOLANGFMT_USESYNCFMT,true).toBool();
    int timeout = m_liteApp->settings()->value(GOLANGFMT_SYNCTIMEOUT,500).toInt();
    if (!diff) {
        autofmt = false;
    }
    ui->checkBoxUseGoimports->setChecked(goimports);
    ui->checkBoxDiff->setChecked(diff);
    ui->checkBoxAutoFmt->setChecked(autofmt);
    ui->enableSyncCheckBox->setChecked(syncfmt);
    ui->syncTimeoutLineEdit->setText(QString("%1").arg(timeout));

    connect(ui->checkBoxDiff,SIGNAL(toggled(bool)),ui->checkBoxAutoFmt,SLOT(setEnabled(bool)));
    connect(ui->checkBoxDiff,SIGNAL(clicked(bool)),ui->checkBoxAutoFmt,SLOT(setChecked(bool)));
}

GolangFmtOption::~GolangFmtOption()
{
    delete m_widget;
    delete ui;
}

QWidget *GolangFmtOption::widget()
{
    return m_widget;
}

QString GolangFmtOption::name() const
{
    return "GolangFmt";
}

QString GolangFmtOption::mimeType() const
{
    return "option/golangfmt";
}

void GolangFmtOption::apply()
{
    bool goimports = ui->checkBoxUseGoimports->isChecked();
    bool diff = ui->checkBoxDiff->isChecked();
    bool autofmt = ui->checkBoxAutoFmt->isChecked();
    bool syncfmt = ui->enableSyncCheckBox->isChecked();
    if (!diff) {
        autofmt = false;
    }
    m_liteApp->settings()->setValue(GOLANGFMT_USEGOIMPORTS,goimports);
    m_liteApp->settings()->setValue(GOLANGFMT_USEDIFF,diff);
    m_liteApp->settings()->setValue(GOLANGFMT_AUTOFMT,autofmt);
    m_liteApp->settings()->setValue(GOLANGFMT_USESYNCFMT,syncfmt);
    int timeout = ui->syncTimeoutLineEdit->text().toInt();
    if (timeout < 500) {
        timeout = 500;
    }
    ui->syncTimeoutLineEdit->setText(QString("%1").arg(timeout));
    m_liteApp->settings()->setValue(GOLANGFMT_SYNCTIMEOUT,timeout);
}
