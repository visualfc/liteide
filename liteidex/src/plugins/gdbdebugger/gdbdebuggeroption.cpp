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
// Module: gdbdebuggeroption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "gdbdebuggeroption.h"
#include "ui_gdbdebuggeroption.h"
#include "gdbdebugger_global.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GdbDebuggerOption::GdbDebuggerOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GdbDebuggerOption)
{
    ui->setupUi(m_widget);
}

GdbDebuggerOption::~GdbDebuggerOption()
{
    delete m_widget;
    delete ui;
}

QWidget *GdbDebuggerOption::widget()
{
    return m_widget;
}

QString GdbDebuggerOption::name() const
{
    return "GdbDebugger";
}

QString GdbDebuggerOption::mimeType() const
{
    return OPTION_GDBDEBUGGER;
}

void GdbDebuggerOption::load()
{
    ui->useTtyCheckBox->setChecked(isGdbDebuggerUseTty(m_liteApp));
}

void GdbDebuggerOption::save()
{
    m_liteApp->settings()->setValue(GDBDEBUGGER_USETTY,ui->useTtyCheckBox->isChecked());
}

bool isGdbDebuggerUseTty(LiteApi::IApplication *app)
{
#ifdef Q_OS_WIN
    return app->settings()->value(GDBDEBUGGER_USETTY,true).toBool();
#else
    return app->settings()->value(GDBDEBUGGER_USETTY,false).toBool();
#endif
}
