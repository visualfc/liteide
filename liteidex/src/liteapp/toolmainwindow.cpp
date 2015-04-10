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
// Module: toolmainwindow.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "toolmainwindow.h"
#include <QDebug>
#include "memory.h"

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

void ToolMainWindow::createToolWindowMenu()
{
    m_windowStyle->createToolWindowMenu();
}

QAction *ToolMainWindow::findToolWindow(QWidget *widget)
{
    return m_windowStyle->findToolWindow(widget);
}

QByteArray ToolMainWindow::saveState(int version) const
{
    m_windowStyle->saveToolState();
    return QMainWindow::saveState(version);
}

bool ToolMainWindow::restoreState(const QByteArray &state, int version)
{
    bool b = QMainWindow::restoreState(state,version);
    m_windowStyle->restoreToolsState();
    return b;
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

void ToolMainWindow::hideOutputWindow()
{
    m_windowStyle->hideOutputWindow();
}

void ToolMainWindow::hideAllToolWindows()
{
    m_windowStyle->hideAllToolWindows();
}
