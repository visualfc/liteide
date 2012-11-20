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
// Module: dockmanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: dockmanager.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef DOCKMANAGER_H
#define DOCKMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class QDockWidget;
class DockManager : public IDockManager
{
public:
    virtual QWidget *widget();
    virtual QDockWidget *addDock(QWidget *widget,
                         const QString &title,
                         Qt::DockWidgetArea ares = Qt::LeftDockWidgetArea,
                         Qt::DockWidgetAreas alowedAreas = Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea,
                         QDockWidget::DockWidgetFeatures  features = QDockWidget::AllDockWidgetFeatures);
    virtual void removeDock(QWidget *widget);
    virtual void showDock(QWidget *widget);
    virtual void hideDock(QWidget *widget);
    virtual QDockWidget *dockWidget(QWidget *widget);
protected:
     QMap<QWidget*,QDockWidget*>  m_widgetDockMap;
 };

#endif // DOCKMANAGER_H
