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
// Module: quickopenmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenmanager.h"
#include "quickopen_global.h"
#include "quickopenfiles.h"
#include "quickopenfolder.h"
#include "quickopeneditor.h"
#include "quickopenmimetype.h"
#include "quickopenaction.h"
#include "quickopenhelp.h"
#include "quickopenfilesystem.h"
#include "liteapi/liteids.h"
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

bool QuickOpenManager::initWithApp(IApplication *app)
{
    if (!IQuickOpenManager::initWithApp(app)) {
        return false;
    }

    m_liteApp->extension()->addObject("LiteApi.IQuickOpenManager",this);

    m_widget = new QuickOpenWidget(m_liteApp,m_liteApp->mainWindow());

    connect(m_widget,SIGNAL(filterChanged(QString)),this,SLOT(filterChanged(QString)));
    connect(m_widget->editor(),SIGNAL(returnPressed()),this,SLOT(selected()));
    connect(m_widget->view(),SIGNAL(clicked(QModelIndex)),this,SLOT(selected()));
    connect(m_widget->view(),SIGNAL(activated(QModelIndex)),this,SLOT(selected()));
    connect(m_widget,SIGNAL(hideWidget()),this,SLOT(hideWidget()));
    connect(m_widget,SIGNAL(indexChanage(QModelIndex)),this,SLOT(indexChanage(QModelIndex)));

    m_quickOpenFiles = new QuickOpenFiles(app,this);
    m_quickOpenFolder = new QuickOpenFolder(app,this);
    m_quickOpenFileSystem = new QuickOpenFileSystem(app,this);

    this->addFilter("",m_quickOpenFiles);
    this->addFilter("",m_quickOpenFolder);
    this->addFilter("",m_quickOpenFileSystem);
    this->addFilter("~",new QuickOpenEditor(m_liteApp,this));
    this->addFilter(">",new QuickOpenAction(m_liteApp,this));
    this->addFilter("?",new QuickOpenHelp(m_liteApp,this));

    this->registerQuickOpenMimeType("@");

    m_quickOpenAct = new QAction(tr("Quick Open File"),this);
    m_quickOpenEditAct = new QAction(tr("Quick Open Editor"),this);
    m_quickOpenSymbolAct = new QAction(tr("Quick Open Symbol"),this);
    m_quickOpenActionAct = new QAction(tr("Quick Open Command"),this);
    m_quickOpenHelpAct = new QAction(tr("Show All Quick Open Actions"),this);

    m_liteApp->actionManager()->setViewMenuSeparator("sep/quickopen",true);

    LiteApi::IActionContext *context = m_liteApp->actionManager()->getActionContext(m_liteApp,"App");
    context->regAction(m_quickOpenAct,"QuickOpen","CTRL+P");
    context->regAction(m_quickOpenEditAct,"QuickOpenEditor","CTRL+ALT+P");
    context->regAction(m_quickOpenActionAct,"QuickOpenCommand","CTRL+SHIFT+P");
    context->regAction(m_quickOpenSymbolAct,"QuickOpenSymbol","CTRL+SHIFT+O");
    context->regAction(m_quickOpenHelpAct,"QuickOpenHelp","");

    m_liteApp->actionManager()->insertMenuActions(ID_MENU_TOOLS,"sep/quickopen",true, QList<QAction*>()
                                                  << m_quickOpenAct
                                                  << m_quickOpenActionAct
                                                  << m_quickOpenEditAct
                                                  << m_quickOpenSymbolAct
                                                  << m_quickOpenHelpAct );

    connect(m_quickOpenAct,SIGNAL(triggered(bool)),this,SLOT(quickOpen()));
    connect(m_quickOpenEditAct,SIGNAL(triggered(bool)),this,SLOT(quickOpenEditor()));
    connect(m_quickOpenSymbolAct,SIGNAL(triggered(bool)),this,SLOT(quickOpenSymbol()));
    connect(m_quickOpenActionAct,SIGNAL(triggered(bool)),this,SLOT(quickOpenCommand()));
    connect(m_quickOpenHelpAct,SIGNAL(triggered(bool)),this,SLOT(quickOpenHelp()));

    connect(m_liteApp,SIGNAL(aboutToQuit()),this,SLOT(appAboutToQuit()));

    return true;
}

void QuickOpenManager::addFilter(const QString &sym, IQuickOpen *filter)
{
    if (filter == 0) {
        return;
    }
    if (m_filterList.contains(filter)) {
        return;
    }
    m_filterList.push_back(filter);
    if (!sym.isEmpty()) {
        m_symFilterMap.insert(sym,filter);
    }
}

void QuickOpenManager::removeFilter(IQuickOpen *filter)
{
    m_filterList.removeAll(filter);
    QMutableMapIterator<QString,IQuickOpen*> i(m_symFilterMap);
    while (i.hasNext()) {
        i.next();
        if (i.value() == filter) {
            m_symFilterMap.remove(i.key());
            break;
        }
    }
}

QList<IQuickOpen *> QuickOpenManager::filterList() const
{
    return m_filterList;
}

QMap<QString, IQuickOpen *> QuickOpenManager::symFilterMap() const
{
    return m_symFilterMap;
}

void QuickOpenManager::setCurrentFilter(IQuickOpen *filter)
{
    if (filter) {
        filter->activate();
        m_widget->editor()->setPlaceholderText(filter->placeholderText());
    }
    if (m_currentFilter == filter) {
        return;
    }
    m_currentFilter = filter;
    if (m_currentFilter) {
        m_sym = m_symFilterMap.key(filter);
        m_widget->setModel(m_currentFilter->model(),m_currentFilter->rootIndex());
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
        showPopup();
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
        showPopup();
    }
}

IQuickOpen *QuickOpenManager::findById(const QString &id)
{
    foreach (LiteApi::IQuickOpen *filter, m_filterList) {
        if (filter->id() == id) {
            return filter;
        }
    }
    return 0;
}

IQuickOpen *QuickOpenManager::findBySymbol(const QString &sym)
{
    QMutableMapIterator<QString,IQuickOpen*> i(m_symFilterMap);
    while (i.hasNext()) {
        i.next();
        if (i.key() == sym) {
            return i.value();
        }
    }
    return 0;
}

QWidget *QuickOpenManager::widget() const
{
    return m_widget;
}

QTreeView *QuickOpenManager::modelView() const
{
    return m_widget->view();
}

QLineEdit *QuickOpenManager::lineEdit() const
{
    return m_widget->editor();
}

bool QuickOpenManager::showOpenFolder(const QString &folder, QPoint *pos)
{
    m_quickOpenFolder->setFolder(folder);

    this->setCurrentFilter(m_quickOpenFolder);

    this->showPopup(pos);

    return true;
}

IQuickOpenMimeType *QuickOpenManager::registerQuickOpenMimeType(const QString &sym)
{
    IQuickOpenMimeType *symbol = m_quickOpenSymbolMap[sym];
    if (!symbol) {
        symbol = new QuickOpenMimeType(m_liteApp,this);
        this->addFilter(sym,symbol);
        m_quickOpenSymbolMap[sym] = symbol;
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

void QuickOpenManager::quickOpenSymbol()
{
    showById("quickopen/symbol");
}

void QuickOpenManager::quickOpenCommand()
{
    showById("quickopen/command");
}

void QuickOpenManager::quickOpenHelp()
{
    showById("quickopen/help");
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

void QuickOpenManager::showPopup(QPoint *pos)
{
    updateModel();
    m_widget->editor()->setText(m_sym);
    m_widget->showView(pos);
}

void QuickOpenManager::hideWidget()
{
    if (m_currentFilter) {
        m_currentFilter->cancel();
    }
    m_currentFilter = 0;
    m_updateMap.clear();
    m_sym.clear();
}

void QuickOpenManager::hidePopup()
{
    m_widget->close();
}

void QuickOpenManager::filterChanged(const QString &text)
{
    bool checkSym = false;
    if (m_currentFilter == m_quickOpenFiles) {
        checkSym = true;
    } else if (!m_sym.isEmpty()) {
        checkSym = true;
    }
    // check register symbol qucik open
    if (checkSym) {
        IQuickOpen *quick = 0;
        if (!text.isEmpty()) {
            QMapIterator<QString,IQuickOpen*> i(m_symFilterMap);
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
    hidePopup();
}

void QuickOpenManager::appAboutToQuit()
{
    hidePopup();
}

