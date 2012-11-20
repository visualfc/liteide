/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: dockmanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: dockmanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "dockmanager.h"
#include "mainwindow.h"

#include <QDockWidget>
#include <QMenu>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


QWidget *DockManager::widget()
{
    return m_liteApp->mainWindow()->window();
}

QDockWidget *DockManager::addDock(QWidget *widget, const QString &title, Qt::DockWidgetArea area, Qt::DockWidgetAreas alowedAreas, QDockWidget::DockWidgetFeatures features)
{
    /*
    QDockWidget *dock = m_widgetDockMap.value(widget);
    if (dock) {
        return dock;
    }
    dock = new QDockWidget(title,m_liteApp->mainWindow());
    dock->setWidget(widget);
    dock->setObjectName(title);
    dock->setAllowedAreas(alowedAreas);
    dock->setFeatures(features);
    m_liteApp->mainWindow()->addDockWidget(area, dock);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuPanePos,dock->toggleViewAction());
    */
    ToolMainWindow *main = static_cast<ToolMainWindow*>(m_liteApp->mainWindow());
    if (!main) {
        return NULL;
    }
    QDockWidget *dock = (QDockWidget*)main->addToolWindow(area,widget,title,title);
    m_widgetDockMap.insert(widget,dock);

    return dock;
}

void DockManager::removeDock(QWidget *widget)
{
    QDockWidget *dock = m_widgetDockMap.value(widget);
    if (dock) {
        QMenu *menu = m_liteApp->actionManager()->loadMenu("menu/view");
        if (menu) {
            menu->removeAction(dock->toggleViewAction());
        }
        m_liteApp->mainWindow()->removeDockWidget(dock);
    }
    m_widgetDockMap.remove(widget);
}

void DockManager::showDock(QWidget *widget)
{
    QDockWidget *dock = m_widgetDockMap.value(widget);
    if (dock) {
        dock->show();
    }
}

void DockManager::hideDock(QWidget *widget)
{
    QDockWidget *dock = m_widgetDockMap.value(widget);
    if (dock) {
        dock->hide();
    }
}

QDockWidget *DockManager::dockWidget(QWidget *widget)
{
    return m_widgetDockMap.value(widget);
}
