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
// Module: quickopenmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenmanager.h"
#include "quickopen_global.h"
#include "quickopenfiles.h"
#include <QTreeView>
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

QuickOpenManager::QuickOpenManager(QObject *parent) : LiteApi::IQuickOpenManager(parent)
{

}

QuickOpenManager::~QuickOpenManager()
{
    qDeleteAll(m_filterMap);
}

bool QuickOpenManager::initWithApp(IApplication *app)
{
    if (!IQuickOpenManager::initWithApp(app)) {
        return false;
    }

    m_liteApp->extension()->addObject("LiteApi.IQuickOpenManager",this);

    m_widget = new QuickOpenWidget(m_liteApp,m_liteApp->mainWindow());
    connect(m_widget,SIGNAL(filterChanged(QString)),this,SLOT(filterChanged(QString)));
    connect(m_widget,SIGNAL(selected()),this,SLOT(selected()));

    m_quickOpenFiles = new QuickOpenFiles(app,this);

    setCurrentFilter(m_quickOpenFiles);

    m_quickOpenAct = new QAction(tr("Quick Open"),this);

    LiteApi::IActionContext *context = m_liteApp->actionManager()->getActionContext(m_liteApp,"App");
    context->regAction(m_quickOpenAct,"QuickOpen","CTRL+P");

    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_quickOpenAct);

    connect(m_quickOpenAct,SIGNAL(triggered(bool)),this,SLOT(showQuickOpen()));
    return true;
}

void QuickOpenManager::addFilter(const QString &sym, IQuickOpen *filter)
{
    m_filterMap.insert(sym,filter);
}

void QuickOpenManager::removeFilter(IQuickOpen *filter)
{
    QMutableMapIterator<QString,IQuickOpen*> i(m_filterMap);
    while (i.hasNext()) {
        i.next();
        if (i.value() == filter) {
            m_filterMap.remove(i.key());
            break;
        }
    }
}

QList<IQuickOpen *> QuickOpenManager::filterList() const
{
    return m_filterMap.values();
}

void QuickOpenManager::setCurrentFilter(IQuickOpen *filter)
{
    if (m_currentFilter == filter) {
        return;
    }
    m_currentFilter = filter;
    m_sym = m_filterMap.key(filter);
}

IQuickOpen *QuickOpenManager::currentFilter() const
{
    return m_currentFilter;
}

QModelIndex QuickOpenManager::currentIndex() const
{
    return m_widget->view()->currentIndex();
}

void QuickOpenManager::showQuickOpen()
{
    if (m_currentFilter) {
        m_currentFilter->updateModel();
        m_widget->setModel(m_currentFilter->model());
        m_widget->view()->resizeColumnToContents(0);
    }
    m_widget->editor()->clear();
    m_widget->showPopup();
}

void QuickOpenManager::hideQuickOpen()
{
    m_widget->close();
}

void QuickOpenManager::filterChanged(const QString &text)
{
    if (text.isEmpty()) {
        setCurrentFilter(m_quickOpenFiles);
    }
    if (!m_sym.isEmpty() && !text.startsWith(m_sym)) {
        if (text.size() == 1 || text.endsWith(" ")) {
            QString key = text.trimmed();
            if (key.isEmpty()) {
                return;
            }
            QMapIterator<QString,IQuickOpen*> i(m_filterMap);
            while(i.hasNext()) {
                i.next();
                if (i.key() == key) {
                    this->setCurrentFilter(i.value());
                    return;
                }
            }
        }
    }
    if (m_currentFilter) {
        QModelIndex index = m_currentFilter->filter(text.mid(m_sym.size()));
        m_widget->view()->setCurrentIndex(index);
    }
}

void QuickOpenManager::selected()
{
    if (!m_currentFilter) {
        return;
    }
    QString text = m_widget->editor()->text();
    QModelIndex index = m_widget->view()->currentIndex();
    if (!m_currentFilter->selected(text.mid(m_sym.size()),index)) {
        return;
    }
    this->hideQuickOpen();
}

