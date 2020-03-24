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
// Module: golangastoption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangastoption.h"
#include "ui_golangastoption.h"
#include "golangast_global.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangAstOption::GolangAstOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GolangAstOption)
{
    ui->setupUi(m_widget);
}

GolangAstOption::~GolangAstOption()
{
    delete m_widget;
    delete ui;
}

QWidget *GolangAstOption::widget()
{
    return m_widget;
}

QString GolangAstOption::name() const
{
    return "GolangAst";
}

QString GolangAstOption::mimeType() const
{
    return OPTION_GOLANGAST;
}

void GolangAstOption::load()
{
    ui->checkQuickSymbolImportPath->setChecked(m_liteApp->settings()->value(GOLANGAST_QUICKOPNE_SYMBOL_IMPORTPATH,true).toBool());
    ui->checkQuickSymbolMatchCase->setChecked(m_liteApp->settings()->value(GOLANGAST_QUICKOPEN_SYMBOL_MATCHCASE,false).toBool());
}

void GolangAstOption::save()
{
    m_liteApp->settings()->setValue(GOLANGAST_QUICKOPNE_SYMBOL_IMPORTPATH,ui->checkQuickSymbolImportPath->isChecked());
    m_liteApp->settings()->setValue(GOLANGAST_QUICKOPEN_SYMBOL_MATCHCASE,ui->checkQuickSymbolMatchCase->isChecked());
}
