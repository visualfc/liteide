/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE. All rights reserved.
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
// Module: golanglintoption.cpp
// Creator: Hai Thanh Nguyen <phaikawl@gmail.com>

#include "golanglintoption.h"
#include "golanglint_global.h"
#include "ui_golanglintoption.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangLintOption::GolangLintOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GolangLintOption)
{
    ui->setupUi(m_widget);

    int confidence = m_liteApp->settings()->value(GOLANGLINT_CONFIDENCE,0).toInt();
    int timeout = m_liteApp->settings()->value(GOLANGLINT_SYNCTIMEOUT,500).toInt();
    bool autolint = m_liteApp->settings()->value(GOLANGLINT_AUTOLINT,false).toBool();

    ui->checkBoxAutoLint->setChecked(autolint);
    ui->syncTimeoutLineEdit->setText(QString("%1").arg(timeout));
    ui->lintConfidenceValLabel->setText(QString("%1").arg((float)confidence/10));
    ui->confidenceSlider->setValue(confidence);
    connect(ui->confidenceSlider, SIGNAL(valueChanged(int)), this, SLOT(on_confidenceSlider_valueChanged(int)));
}

GolangLintOption::~GolangLintOption()
{
    delete m_widget;
    delete ui;
}

QWidget *GolangLintOption::widget()
{
    return m_widget;
}

QString GolangLintOption::name() const
{
    return "GolangLint";
}

QString GolangLintOption::mimeType() const
{
    return "option/golanglint";
}

void GolangLintOption::apply()
{
    bool autolint = ui->checkBoxAutoLint->isChecked();
    int confidence = ui->confidenceSlider->value();
    m_liteApp->settings()->setValue(GOLANGLINT_AUTOLINT,autolint);
    m_liteApp->settings()->setValue(GOLANGLINT_CONFIDENCE,confidence);
    int timeout = ui->syncTimeoutLineEdit->text().toInt();
    if (timeout < 500) {
        timeout = 500;
    }
    ui->syncTimeoutLineEdit->setText(QString("%1").arg(timeout));
    m_liteApp->settings()->setValue(GOLANGLINT_SYNCTIMEOUT,timeout);
}

void GolangLintOption::on_confidenceSlider_valueChanged(int value)
{
    ui->lintConfidenceValLabel->setText(QString("%1").arg((float)value/10));
}
