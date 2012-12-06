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
// Module: mainwindow.h
// Creator: visualfc <visualfc@gmail.com>



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "liteapi/liteapi.h"
#include "toolmainwindow.h"

using namespace LiteApi;
class QSplitter;
class MainWindow : public ToolMainWindow
{
    Q_OBJECT
public:
    MainWindow(IApplication *app, QWidget *parent = 0);
    ~MainWindow();
public:
    QSplitter *splitter();
protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
public slots:
    void editorModifyChanged(bool b);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void about();
protected:
    IApplication     *m_liteApp;
    QAction          *m_aboutAct;
    QSplitter *m_mainSplitter;
};


#endif // MAINWINDOW_H
