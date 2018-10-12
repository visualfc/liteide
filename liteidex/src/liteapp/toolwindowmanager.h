/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: toolwindowmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TOOLWINDOWMANAGER_H
#define TOOLWINDOWMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class ToolWindowManager : public IToolWindowManager
{
public:
    virtual QAction *addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split, QList<QAction*> widgetActions = QList<QAction*>());
    virtual void moveToolWindow(Qt::DockWidgetArea from, Qt::DockWidgetArea to,QAction *action, bool split);
    virtual QAction *findToolWindow(QWidget *widget);
    virtual void removeToolWindow(QAction *action);
    virtual void removeToolWindow(QWidget *widget);
};

#endif // TOOLWINDOWMANAGER_H
