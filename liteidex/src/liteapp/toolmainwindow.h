/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: toolmainwindow.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TOOLMAINWINDOW_H
#define TOOLMAINWINDOW_H

#include "liteapi/liteapi.h"
#include "windowstyle.h"

class ToolMainWindow : public QMainWindow
{
    Q_OBJECT    
public:
    ToolMainWindow(QWidget *parent = 0);
    ~ToolMainWindow();
    void setWindowStyle(IWindowStyle *style);
    QAction *addToolWindow(LiteApi::IApplication *app, Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false, QList<QAction*> widgetActions = QList<QAction*>());
    void removeToolWindow(QAction *action);
    QAction *findToolWindow(QWidget *wiget);
    QByteArray saveToolState(int version = 0) const;
    bool loadInitToolState(const QByteArray &state, int version = 0);
    bool restoreState(const QByteArray &state, int version = 0);
public slots:
    void hideToolWindow(Qt::DockWidgetArea area = Qt::BottomDockWidgetArea);
    void showOrHideToolWindow();
    void hideAllToolWindows();
    void moveToolWindow(Qt::DockWidgetArea area, QAction *action,bool split = false);    
protected:
    IWindowStyle *m_windowStyle;
};

#endif // TOOLMAINWINDOW_H
