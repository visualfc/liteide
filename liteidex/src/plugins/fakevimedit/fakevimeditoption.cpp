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
// Module: fakevimeditoption.cpp
// Creator: jsuppe <jon.suppe@gmail.com>

#include "fakevimeditoption.h"
#include "ui_fakevimeditoption.h"
#include "fakevimedit_global.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

FakeVimEditOption::FakeVimEditOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::FakeVimEditOption)
{
    ui->setupUi(m_widget);
    QStringList cmds = m_liteApp->settings()->value(FAKEVIMEDIT_INITCOMMANDS,initCommandList()).toStringList();
    ui->textInitCommands->setPlainText(cmds.join("\n"));
    connect(ui->resetCommandsButton,SIGNAL(clicked(bool)),this,SLOT(on_resetCommandsButton_clicked()));
}

FakeVimEditOption::~FakeVimEditOption()
{
    delete m_widget;
    delete ui;
}

QWidget *FakeVimEditOption::widget()
{
    return m_widget;
}

QString FakeVimEditOption::name() const
{
    return "FakeVimEdit";
}

QString FakeVimEditOption::mimeType() const
{
    return OPTION_FAKEVIMEDIT;
}

void FakeVimEditOption::apply()
{
    //bool useFakeVim = ui->enableUseFakeVimCheckBox->isChecked();
    QStringList cmds = ui->textInitCommands->toPlainText().split("\n",QString::SkipEmptyParts);
    m_liteApp->settings()->setValue(FAKEVIMEDIT_INITCOMMANDS,cmds);
}

void FakeVimEditOption::on_resetCommandsButton_clicked()
{
    ui->textInitCommands->setPlainText(initCommandList().join("\n"));
}
