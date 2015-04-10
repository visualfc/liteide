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
// Module: filebrowseroption.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filebrowseroption.h"
#include "ui_filebrowseroption.h"
#include "filebrowser.h"
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include "memory.h"

FileBrowserOption::FileBrowserOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::FileBrowserOption)
{
    ui->setupUi(m_widget);
}

FileBrowserOption::~FileBrowserOption()
{
    delete m_widget;
    delete ui;
}

QWidget *FileBrowserOption::widget()
{
    return m_widget;
}

QString FileBrowserOption::name() const
{
    return "FileBrowser";
}

QString FileBrowserOption::mimeType() const
{
    return "option/filebrowser";
}

void FileBrowserOption::apply()
{
    QString cmd = ui->cmdLineEdit->text().trimmed();
    QString args = ui->argsLineEdit->text().trimmed();
    m_liteApp->settings()->setValue("filebrowser/shell_cmd",cmd);
    m_liteApp->settings()->setValue("filebrowser/shell_args",args.split(" ",QString::SkipEmptyParts));
}
