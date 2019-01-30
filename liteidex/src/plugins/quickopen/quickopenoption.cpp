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
// Module: quickopenoption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenoption.h"
#include "ui_quickopenoption.h"
#include "quickopen_global.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QuickOpenOption::QuickOpenOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::QuickOpenOption)
{
    ui->setupUi(m_widget);
    ui->checkFilesMatchCase->setChecked(m_liteApp->settings()->value(QUICKOPNE_FILES_MATCHCASE,false).toBool());
    ui->checkEditorMatchCase->setChecked(m_liteApp->settings()->value(QUICKOPNE_EDITOR_MATCHCASE,false).toBool());
    ui->spinBoxFilesMaxCount->setValue(m_liteApp->settings()->value(QUICKOPEN_FILES_MAXCOUNT,100000).toInt());
}

QuickOpenOption::~QuickOpenOption()
{
    delete m_widget;
    delete ui;
}

QWidget *QuickOpenOption::widget()
{
    return m_widget;
}

QString QuickOpenOption::name() const
{
    return "QuickOpen";
}

QString QuickOpenOption::mimeType() const
{
    return OPTION_QUICKOPEN;
}
void QuickOpenOption::apply()
{
    m_liteApp->settings()->setValue(QUICKOPNE_FILES_MATCHCASE,ui->checkFilesMatchCase->isChecked());
    m_liteApp->settings()->setValue(QUICKOPNE_EDITOR_MATCHCASE,ui->checkEditorMatchCase->isChecked());
    m_liteApp->settings()->setValue(QUICKOPEN_FILES_MAXCOUNT,ui->spinBoxFilesMaxCount->value());
}
