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
// Module: golangdocoption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangdocoption.h"
#include "ui_golangdocoption.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangDocOption::GolangDocOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GolangDocOption)
{
    ui->setupUi(m_widget);

    bool defctx = m_liteApp->settings()->value("golangapi/defctx",true).toBool();
    bool std = m_liteApp->settings()->value("golangapi/std",false).toBool();
    ui->defContextCheckBox->setChecked(defctx);
    ui->onlyStdCheckBox->setChecked(std);
}

GolangDocOption::~GolangDocOption()
{
    delete m_widget;
    delete ui;
}

QWidget *GolangDocOption::widget()
{
    return m_widget;
}

QString GolangDocOption::name() const
{
    return "GolangDoc";
}

QString GolangDocOption::mimeType() const
{
    return "option/golangdoc";
}
void GolangDocOption::apply()
{
    bool defctx = ui->defContextCheckBox->isChecked();
    bool std = ui->onlyStdCheckBox->isChecked();
    m_liteApp->settings()->setValue("golangapi/defctx",defctx);
    m_liteApp->settings()->setValue("golangapi/std",std);
}
