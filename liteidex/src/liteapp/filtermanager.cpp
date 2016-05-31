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
// Module: filtermanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filtermanager.h"
#include <QToolBar>
#include <QComboBox>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

FilterManager::FilterManager()
{

}

FilterManager::~FilterManager()
{
    qDeleteAll(m_filterMap);
}

bool FilterManager::initWithApp(IApplication *app)
{
    if (!IFilterManager::initWithApp(app)) {
        return false;
    }

    return true;
}

void FilterManager::createActions()
{
    m_widget = new QuickOpenWidget(m_liteApp,m_liteApp->mainWindow());

    m_quickOpenAct = new QAction(tr("Quick Open"),this);

    LiteApi::IActionContext *context = m_liteApp->actionManager()->getActionContext(m_liteApp,"App");
    context->regAction(m_quickOpenAct,"QuickOpen","CTRL+P");

    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_quickOpenAct);

    connect(m_quickOpenAct,SIGNAL(triggered(bool)),this,SLOT(showQuickOpen()));
}

void FilterManager::addFilter(const QString &sym, IFilter *filter)
{
    m_filterMap.insert(sym,filter);
}

void FilterManager::removeFilter(IFilter *filter)
{
    QMutableMapIterator<QString,IFilter*> i(m_filterMap);
    while (i.hasNext()) {
        i.next();
        if (i.value() == filter) {
            m_filterMap.remove(i.key());
            break;
        }
    }
}

QList<IFilter *> FilterManager::filterList() const
{
    return m_filterMap.values();
}

void FilterManager::setCurrentFilter(IFilter *filter)
{
    m_currentFilter = filter;
}

IFilter *FilterManager::currentFilter() const
{
    return m_currentFilter;
}

void FilterManager::showQuickOpen()
{
    m_widget->showPopup();
}

void FilterManager::hideQuickOpen()
{
    m_widget->close();
}
