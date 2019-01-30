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
// Module: windowstyle.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef WINDOWSTYLE_H
#define WINDOWSTYLE_H

#include <liteapi/liteapi.h>

class IWindowStyle : public QObject
{
    Q_OBJECT
public:
    IWindowStyle(QObject *parent) : QObject(parent) {}
    virtual void createToolWindowMenu() = 0;
    virtual QAction *addToolWindow(LiteApi::IApplication *app, Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false, QList<QAction*> widgetActions = QList<QAction*>()) = 0;
    virtual void removeToolWindow(QAction *action) = 0;
    virtual QAction *findToolWindow(QWidget *wiget) = 0;
    virtual void moveToolWindow(Qt::DockWidgetArea from, Qt::DockWidgetArea to,QAction *action,bool split) = 0;
    virtual void saveToolState() const = 0;
    virtual void restoreToolsState() = 0;
    virtual void updateConer() = 0;
public slots:
    virtual void hideOutputWindow() = 0;
    virtual void showOrHideToolWindow() = 0;
    virtual void hideAllToolWindows() = 0;
};

#endif // WINDOWSTYLE_H
