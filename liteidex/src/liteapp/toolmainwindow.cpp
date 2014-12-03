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
// Module: toolmainwindow.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "toolmainwindow.h"
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

ToolMainWindow::ToolMainWindow(QWidget *parent)
    : QMainWindow(parent), m_windowStyle(0)
{

}

ToolMainWindow::~ToolMainWindow()
{
    if (m_windowStyle) {
        delete m_windowStyle;
    }
}

void ToolMainWindow::setWindowStyle(IWindowStyle *style)
{
    m_windowStyle = style;
}

QAction *ToolMainWindow::findToolWindow(QWidget *widget)
{
    return m_windowStyle->findToolWindow(widget);
}

void ToolMainWindow::removeToolWindow(QAction *action)
{
    m_windowStyle->removeToolWindow(action);
}

QAction *ToolMainWindow::addToolWindow(LiteApi::IApplication *app,Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split, QList<QAction*> widgetActions)
{
    return m_windowStyle->addToolWindow(app,area,widget,id,title,split,widgetActions);
}

void ToolMainWindow::moveToolWindow(Qt::DockWidgetArea area,QAction *action,bool split)
{
    m_windowStyle->moveToolWindow(area,action,split);
}

void ToolMainWindow::showOrHideToolWindow()
{
    m_windowStyle->showOrHideToolWindow();
}

void ToolMainWindow::hideAllToolWindows()
{
    m_windowStyle->hideAllToolWindows();
}

QByteArray ToolMainWindow::saveToolState(int version) const
{
    return m_windowStyle->saveToolState(version);
}

bool ToolMainWindow::restoreToolState(const QByteArray &state, int version)
{
    return m_windowStyle->restoreToolsState(state,version);
}

void ToolMainWindow::hideToolWindow(Qt::DockWidgetArea area)
{
    m_windowStyle->hideToolWindow(area);
}
