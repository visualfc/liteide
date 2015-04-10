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
// Module: toolwindowmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "toolwindowmanager.h"
#include "toolmainwindow.h"
#include "liteapp_global.h"
#include "memory.h"

QAction *ToolWindowManager::addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split, QList<QAction*> widgetActions)
{
    return ((ToolMainWindow*)m_liteApp->mainWindow())->addToolWindow(m_liteApp,area,widget,id,title,split,widgetActions);
}

void ToolWindowManager::moveToolWindow(Qt::DockWidgetArea area,QAction *action, bool split)
{
    ((ToolMainWindow*)m_liteApp->mainWindow())->moveToolWindow(area,action,split);
}

QAction *ToolWindowManager::findToolWindow(QWidget *widget)
{
    return ((ToolMainWindow*)m_liteApp->mainWindow())->findToolWindow(widget);
}

void ToolWindowManager::removeToolWindow(QAction *action)
{
    ((ToolMainWindow*)m_liteApp->mainWindow())->removeToolWindow(action);
}

void ToolWindowManager::removeToolWindow(QWidget *widget)
{
    QAction *act = this->findToolWindow(widget);
    if (act) {
        this->removeToolWindow(act);
    }
}
