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
#include "quickopeneditor.h"
#include "quickopenmimetype.h"
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

}

bool QuickOpenManager::initWithApp(IApplication *app)
{
    if (!IQuickOpenManager::initWithApp(app)) {
        return false;
    }

    m_liteApp->extension()->addObject("LiteApi.IQuickOpenManager",this);

    m_widget = new QuickOpenWidget(m_liteApp,m_liteApp->mainWindow());
    m_widget->editor()->setPlaceholderText(tr("Type '?' to get help on the actions you can take from here"));

    connect(m_widget->editor(),SIGNAL(textChanged(QString)),this,SLOT(filterChanged(QString)));
    connect(m_widget->editor(),SIGNAL(returnPressed()),this,SLOT(selected()));
    connect(m_widget->view(),SIGNAL(clicked(QModelIndex)),this,SLOT(selected()));
    connect(m_widget->view(),SIGNAL(activated(QModelIndex)),this,SLOT(selected()));
    connect(m_widget,SIGNAL(hidePopup()),this,SLOT(hideQuickOpen()));
    connect(m_widget,SIGNAL(indexChanage(QModelIndex)),this,SLOT(indexChanage(QModelIndex)));

    m_quickOpenFiles = new QuickOpenFiles(app,this);

    //setCurrentFilter(m_quickOpenFiles);
    m_filterMap.insert("",m_quickOpenFiles);
    m_filterMap.insert("~",new QuickOpenEditor(m_liteApp,this));

    this->registerQuickOpenMimeType("@");
    m_quickOpenAct = new QAction(tr("Quick Open"),this);
    m_quickOpenAct->setData("");

    m_quickOpenEditAct = new QAction(tr("Quick Open Editor"),this);
    m_quickOpenEditAct->setData("~");

    m_quickOpenSymbolAct = new QAction(tr("Quick Open Symbol"),this);
    m_quickOpenSymbolAct->setData("@");

    LiteApi::IActionContext *context = m_liteApp->actionManager()->getActionContext(m_liteApp,"App");
    context->regAction(m_quickOpenAct,"QuickOpen","CTRL+P");
    context->regAction(m_quickOpenEditAct,"QuickOpenEditor","CTRL+ALT+P");
    context->regAction(m_quickOpenSymbolAct,"QuickOpenSymbol","CTRL+ALT+O");

    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_quickOpenAct);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_quickOpenEditAct);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_quickOpenSymbolAct);

    connect(m_quickOpenAct,SIGNAL(triggered(bool)),this,SLOT(quickOpen()));
    connect(m_quickOpenEditAct,SIGNAL(triggered(bool)),this,SLOT(quickOpen()));
    connect(m_quickOpenSymbolAct,SIGNAL(triggered(bool)),this,SLOT(quickOpen()));

    return true;
}

void QuickOpenManager::addFilter(const QString &sym, IQuickOpen *filter)
{
    if (filter == 0) {
        return;
    }
    if (sym.isEmpty()) {
        m_liteApp->appendLog("QuickOpen",QString("warning, skip empty symbol, id=%1").arg(filter->id()),true);
        return;
    }
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

QMap<QString, IQuickOpen *> QuickOpenManager::filterMap() const
{
    return m_filterMap;
}

void QuickOpenManager::setCurrentFilter(IQuickOpen *filter)
{
    if (filter) {
        filter->activate();
    }
    if (m_currentFilter == filter) {
        return;
    }
    m_currentFilter = filter;
    if (m_currentFilter) {
        m_sym = m_filterMap.key(filter);
        m_widget->setModel(m_currentFilter->model());
    }
}

IQuickOpen *QuickOpenManager::currentFilter() const
{
    return m_currentFilter;
}

QModelIndex QuickOpenManager::currentIndex() const
{
    return m_widget->view()->currentIndex();
}

void QuickOpenManager::showById(const QString &id)
{
    IQuickOpen *i = findById(id);
    if (i) {
        setCurrentFilter(i);
        showQuickOpen();
    }
}

void QuickOpenManager::showBySymbol(const QString &sym)
{
    IQuickOpen *i = findBySymbol(sym);
    if (i == 0) {
        i = m_quickOpenFiles;
    }
    if (i) {
        setCurrentFilter(i);
        showQuickOpen();
    }
}

IQuickOpen *QuickOpenManager::findById(const QString &id)
{
    QMutableMapIterator<QString,IQuickOpen*> i(m_filterMap);
    while (i.hasNext()) {
        i.next();
        if (i.value()->id() == id) {
            return i.value();
        }
    }
    if (id == m_quickOpenFiles->id()) {
        return m_quickOpenFiles;
    }
    return 0;
}

IQuickOpen *QuickOpenManager::findBySymbol(const QString &sym)
{
    QMutableMapIterator<QString,IQuickOpen*> i(m_filterMap);
    while (i.hasNext()) {
        i.next();
        if (i.key() == sym) {
            return i.value();
        }
    }
    return 0;
}

QTreeView *QuickOpenManager::modelView() const
{
    return m_widget->view();
}

IQuickOpenMimeType *QuickOpenManager::registerQuickOpenMimeType(const QString &sym)
{
    IQuickOpenMimeType *symbol = m_quickOpenSymbolMap[sym];
    if (!symbol) {
        symbol = new QuickOpenMimeType(m_liteApp,this);
        this->addFilter(sym,symbol);
    }
    return symbol;
}

void QuickOpenManager::quickOpen()
{
    m_updateMap.clear();
    QString sym;
    QAction *act = (QAction*)sender();
    if (act) {
        sym = act->data().toString();
    }
    showBySymbol(sym);
}

void QuickOpenManager::quickOpenEditor()
{
    showById("quickopen/editor");
}

void QuickOpenManager::updateModel()
{
    if (!m_currentFilter) {
        return;
    }
    if (m_updateMap.value(m_currentFilter)) {
        return;
    }
    m_updateMap.insert(m_currentFilter,true);
    m_currentFilter->updateModel();
    m_widget->view()->resizeColumnToContents(0);
}

void QuickOpenManager::showQuickOpen()
{
    updateModel();
    m_widget->editor()->setText(m_sym);
    m_widget->showView();
}

void QuickOpenManager::hideQuickOpen()
{
    m_widget->close();
    m_updateMap.clear();
    m_currentFilter = 0;
    m_sym.clear();
}

void QuickOpenManager::filterChanged(const QString &text)
{
    IQuickOpen *quick = 0;
    if (!text.isEmpty()) {
        QMapIterator<QString,IQuickOpen*> i(m_filterMap);
        QString mimeType;
        LiteApi::IEditor *cur = m_liteApp->editorManager()->currentEditor();
        if (cur) {
            mimeType = cur->mimeType();
        }
        while (i.hasNext()) {
            i.next();
            if (i.key().isEmpty()) {
                continue;
            }
            if (text.startsWith(i.key())) {
                quick = i.value();
                break;
            }
        }
    }
    if (quick == 0)  {
        quick = m_quickOpenFiles;
    }
    if (quick != m_currentFilter) {
        this->setCurrentFilter(quick);
        updateModel();
    }
    if (m_currentFilter) {
        QModelIndex index = m_currentFilter->filterChanged(text.mid(m_sym.size()));
        m_widget->view()->setCurrentIndex(index);
        m_widget->view()->scrollTo(index);
    }
}

void QuickOpenManager::indexChanage(const QModelIndex &index)
{
    if (m_currentFilter) {
        m_currentFilter->indexChanged(index);
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

