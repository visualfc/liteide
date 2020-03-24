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
// Module: mainwindow.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "liteapi/liteapi.h"
#include "toolmainwindow.h"
#include "windowstyle.h"

class IMainWindow : public QObject
{
    Q_OBJECT
public:
    IMainWindow(QObject *parent) : QObject(parent) {}
    virtual QMainWindow *mainWindow() = 0;
    virtual LiteApi::IToolWindowManager *toolWindowManager() = 0;
signals:
    void fullScreenStateChanged(bool b);
public slots:
    virtual void setFullScreen(bool b) = 0;
    virtual void editorModifyChanged(bool b) = 0;
    virtual void currentEditorChanged(LiteApi::IEditor *editor) = 0;
};

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
    void setWindowStyle(IWindowStyle *style);
protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void changeEvent(QEvent *e);
    virtual bool event(QEvent *event);
signals:
    void fullScreenStateChanged(bool b);
public slots:
    void setFullScreen(bool b);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void editorModifyChanged(LiteApi::IEditor *editor, bool b);
    void about();
    void triggeredWindowsAct();
protected:
    IApplication     *m_liteApp;
    QAction          *m_aboutAct;
    QSplitter *m_mainSplitter;
    bool              m_windowClosedCheck;
protected:
    void setActiveWindowAction(QWidget *window);
    void setWindowTitle(const QString &name, const QString &filePath, bool isModify);
    static QMap<QWidget*, QAction *> s_windowActions;
    static QMenu       *s_macDocMenu;
};


#endif // MAINWINDOW_H
