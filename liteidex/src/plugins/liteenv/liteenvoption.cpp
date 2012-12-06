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
// Module: liteappoption.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: liteappoption.cpp,v 1.0 2011-7-25 visualfc Exp $

#include "liteenvoption.h"
#include "ui_liteenvoption.h"
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteEnvOption::LiteEnvOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteEnvOption)
{
    ui->setupUi(m_widget);

    m_fileModel = new QFileSystemModel(this);
    QString path = m_liteApp->resourcePath()+"/liteenv";
    QModelIndex root = m_fileModel->setRootPath(path);
    ui->fileTreeView->setEditTriggers(0);
    ui->fileTreeView->setModel(m_fileModel);
    ui->fileTreeView->setRootIndex(root);
    ui->fileTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);
    connect(ui->fileTreeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedFile(QModelIndex)));

}

LiteEnvOption::~LiteEnvOption()
{
    delete m_widget;
    delete ui;
}

QWidget *LiteEnvOption::widget()
{
    return m_widget;
}

QString LiteEnvOption::name() const
{
    return "LiteEnv";
}

QString LiteEnvOption::mimeType() const
{
    return "option/liteenv";
}

void LiteEnvOption::apply()
{
}

void LiteEnvOption::doubleClickedFile(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QFileInfo info = m_fileModel->fileInfo(index);
    if (!info.isFile()) {
        return;
    }
    if (info.suffix() == QLatin1String("env")) {
        m_liteApp->fileManager()->openEditor(info.filePath());
    }
}
