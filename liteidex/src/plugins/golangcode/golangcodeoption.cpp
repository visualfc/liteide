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
// Module: golangcodeoption.cpp
// Creator: Nik-U <niku@vaxxine.com>

#include "golangcodeoption.h"
#include "golangcode_global.h"
#include "ui_golangcodeoption.h"
#include "memory.h"

GolangCodeOption::GolangCodeOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GolangCodeOption)
{
    ui->setupUi(m_widget);

    bool close = m_liteApp->settings()->value(GOLANGCODE_EXITCLOSE,true).toBool();
    bool uppkg = m_liteApp->settings()->value(GOLANGCODE_AUTOBUILD,false).toBool();
    ui->exitCloseCheckBox->setChecked(close);
    ui->autoUpPkgCheckBox->setChecked(uppkg);
}

GolangCodeOption::~GolangCodeOption()
{
    delete m_widget;
    delete ui;
}

QWidget *GolangCodeOption::widget()
{
    return m_widget;
}

QString GolangCodeOption::name() const
{
    return "GolangCode";
}

QString GolangCodeOption::mimeType() const
{
    return "option/golangcode";
}

void GolangCodeOption::apply()
{
    bool close = ui->exitCloseCheckBox->isChecked();
    bool uppkg = ui->autoUpPkgCheckBox->isChecked();
    m_liteApp->settings()->setValue(GOLANGCODE_EXITCLOSE,close);
    m_liteApp->settings()->setValue(GOLANGCODE_AUTOBUILD,uppkg);
}
