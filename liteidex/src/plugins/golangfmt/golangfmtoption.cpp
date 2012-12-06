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
// Module: golangfmtoption.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-6-13
// $Id: golangfmtoption.cpp,v 1.0 2012-6-13 visualfc Exp $

#include "golangfmtoption.h"
#include "ui_golangfmtoption.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangFmtOption::GolangFmtOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GolangFmtOption)
{
    ui->setupUi(m_widget);

    bool diff = m_liteApp->settings()->value("golangfmt/diff",true).toBool();
    bool autofmt = m_liteApp->settings()->value("golangfmt/autofmt",true).toBool();
    bool autopop = m_liteApp->settings()->value("golangfmt/autopop",false).toBool();
    int timeout = m_liteApp->settings()->value("golangfmt/timeout",600).toInt();
    if (!diff) {
        autofmt = false;
    }
    ui->checkBoxDiff->setChecked(diff);
    ui->checkBoxAutoFmt->setChecked(autofmt);
    ui->checkBoxAutoPopMessage->setChecked(autopop);
    ui->timeoutLineEdit->setText(QString("%1").arg(timeout));

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
    bool diff = ui->checkBoxDiff->isChecked();
    bool autofmt = ui->checkBoxAutoFmt->isChecked();
    bool autopop = ui->checkBoxAutoPopMessage->isChecked();
    if (!diff) {
        autofmt = false;
    }
    m_liteApp->settings()->setValue("golangfmt/diff",diff);
    m_liteApp->settings()->setValue("golangfmt/autofmt",autofmt);
    m_liteApp->settings()->setValue("golangfmt/autopop",autopop);
    int timeout = ui->timeoutLineEdit->text().toInt();
    if (timeout < 50) {
        timeout = 600;
    }
    ui->timeoutLineEdit->setText(QString("%1").arg(timeout));
    m_liteApp->settings()->setValue("golangfmt/timeout",timeout);
}
