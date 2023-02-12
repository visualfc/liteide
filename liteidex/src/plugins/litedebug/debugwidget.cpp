/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: debugwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "debugwidget.h"
#include <QTreeView>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QPlainTextEdit>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QItemDelegate>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

class WatchDelegate : public QItemDelegate
{
public:
    WatchDelegate(QObject *parent) : QItemDelegate(parent)
    {
    }
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
    {
        if (index.column() == 0) {
            return QItemDelegate::createEditor(parent,option,index);
        }
        return 0;
    }
};


DebugWidget::DebugWidget(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    m_debugger(0)
{
    m_tabWidget = new QTabWidget;

    m_asyncView = new QTreeView;
    m_varsView = new SymbolTreeView(false);
    m_watchView = new SymbolTreeView(false);
    m_framesView = new QTreeView;
    m_libraryView = new QTreeView;
    m_goroutinesView = new SymbolTreeView(false);
    m_threadsView = new QTreeView;
    m_regsView = new QTreeView;

    m_asyncView->setEditTriggers(0);
    m_varsView->setEditTriggers(0);
    m_varsView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_watchView->setEditTriggers(0);
    m_watchView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_framesView->setEditTriggers(0);
#if QT_VERSION >= 0x050000
    m_framesView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_threadsView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_goroutinesView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_framesView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_threadsView->header()->setResizeMode(QHeaderView::ResizeToContents);
    m_goroutinesView->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    m_libraryView->setEditTriggers(0);
    m_threadsView->setEditTriggers(0);
    m_goroutinesView->setEditTriggers(0);
    m_regsView->setEditTriggers(0);

    m_watchView->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_watchView->setItemDelegate(new WatchDelegate(this));

    m_debugLogEdit = new TextOutput(m_liteApp);
    m_debugLogEdit->setReadOnly(false);
    m_debugLogEdit->setFilterTermColor(true);
    m_debugLogEdit->setMaximumBlockCount(10000);
    m_debugLogEdit->setLineWrapMode(QPlainTextEdit::NoWrap);


    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_tabWidget);

    m_widget->setLayout(layout);

    m_watchMenu = new QMenu(m_widget);
    m_addWatchAct = new QAction(tr("Add Watch"),this);
    //m_addLocalWatchAct = new QAction(tr("Add Local Watch"),this);
    m_removeWatchAct = new QAction(tr("Remove Watch"),this);
    m_removeAllWatchAct = new QAction(tr("Remove All Watches"),this);
    m_watchMenu->addAction(m_addWatchAct);
    //m_watchMenu->addAction(m_addLocalWatchAct);
    m_watchMenu->addSeparator();
    m_watchMenu->addAction(m_removeWatchAct);
    m_watchMenu->addAction(m_removeAllWatchAct);

    connect(m_debugLogEdit,SIGNAL(enterText(QString)),this,SLOT(enterText(QString)));
    connect(m_varsView,SIGNAL(expanded(QModelIndex)),this,SLOT(expandedVarsView(QModelIndex)));
    connect(m_watchView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(watchViewContextMenu(QPoint)));
    connect(m_addWatchAct,SIGNAL(triggered()),this,SLOT(addWatch()));
    //connect(m_addLocalWatchAct,SIGNAL(triggered()),this,SLOT(addLocalWatch()));
    connect(m_removeWatchAct,SIGNAL(triggered()),this,SLOT(removeWatch()));
    connect(m_removeAllWatchAct,SIGNAL(triggered()),this,SLOT(removeAllWatchAct()));
    connect(m_asyncView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclickView(QModelIndex)));
    connect(m_watchView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclickView(QModelIndex)));
    connect(m_varsView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclickView(QModelIndex)));
    connect(m_framesView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclickView(QModelIndex)));
    connect(m_threadsView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclickView(QModelIndex)));
    connect(m_goroutinesView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclickView(QModelIndex)));
    connect(m_regsView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclickView(QModelIndex)));
    connect(m_libraryView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(dbclickView(QModelIndex)));
}

DebugWidget::~DebugWidget()
{
    delete m_asyncView;
    delete m_varsView;
    delete m_watchView;
    delete m_framesView;
    delete m_threadsView;
    delete m_goroutinesView;
    delete m_regsView;
    delete m_libraryView;

    if (m_widget) {
        delete m_widget;
    }
}

QWidget *DebugWidget::widget()
{
    return m_widget;
}

void DebugWidget::enterText(const QString &text)
{
    QString cmd = text.simplified();
    if (!cmd.isEmpty() && m_debugger && m_debugger->isRunning()) {
        emit debugCmdInput();
        m_debugger->enterDebugText(cmd);
    }
}

void DebugWidget::clearLog()
{
    m_debugLogEdit->clear();
}

void DebugWidget::appendLog(const QString &log)
{
    m_debugLogEdit->append(log);
}

static void setResizeView(QTreeView *view)
{
    QAbstractItemModel *model = view->model();
    if (!model) {
        return;
    }
    if (model->columnCount() <= 1) {
        view->setHeaderHidden(true);
        return;
    } else {
        view->setHeaderHidden(false);
    }
#if QT_VERSION >= 0x050000
    view->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
#else
    view->header()->setResizeMode(0,QHeaderView::ResizeToContents);
#endif
}

void DebugWidget::updateView(QTreeView *view, LiteApi::IDebugger *debug, LiteApi::DEBUG_MODEL_TYPE type, const QString &title)
{
    QAbstractItemModel *model = debug->debugModel(type);
    view->setModel(model);
    setResizeView(view);
    if (model != 0) {
        m_tabWidget->addTab(view,title);
    }
}

void DebugWidget::setDebugger(LiteApi::IDebugger *debug)
{
    if (m_debugger == debug) {
        return;
    }
    m_debugger = debug;
    if (!m_debugger) {
        return;
    }
    m_tabWidget->clear();
    updateView(m_asyncView,debug,LiteApi::ASYNC_MODEL,tr("Async Record"));
    updateView(m_varsView,debug,LiteApi::VARS_MODEL,tr("Variables"));
    updateView(m_watchView,debug,LiteApi::WATCHES_MODEL,tr("Watch"));
    updateView(m_framesView,debug,LiteApi::FRAMES_MODEL,tr("Call Stack"));
    updateView(m_threadsView,debug,LiteApi::THREADS_MODEL,tr("Threads"));
    updateView(m_goroutinesView,debug,LiteApi::GOROUTINES_MODEL,tr("Goroutines"));
    updateView(m_regsView,debug,LiteApi::REGS_MODEL,tr("Registers"));
    updateView(m_libraryView,debug,LiteApi::LIBRARY_MODEL,tr("Libraries"));
    m_tabWidget->addTab(m_debugLogEdit,tr("Console"));

    connect(m_debugger,SIGNAL(setExpand(LiteApi::DEBUG_MODEL_TYPE,QModelIndex,bool)),this,SLOT(setExpand(LiteApi::DEBUG_MODEL_TYPE,QModelIndex,bool)));
    connect(m_debugger,SIGNAL(watchCreated(QString,QString)),this,SLOT(watchCreated(QString,QString)));
    connect(m_debugger,SIGNAL(watchRemoved(QString)),this,SLOT(watchRemoved(QString)));
    connect(m_debugger,SIGNAL(beginUpdateModel(LiteApi::DEBUG_MODEL_TYPE)),this,SLOT(beginUpdateModel(LiteApi::DEBUG_MODEL_TYPE)));
    connect(m_debugger,SIGNAL(endUpdateModel(LiteApi::DEBUG_MODEL_TYPE)),this,SLOT(endUpdateModel(LiteApi::DEBUG_MODEL_TYPE)));
}

void DebugWidget::expandedVarsView(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    if (!m_debugger) {
        return;
    }
    m_debugger->expandItem(index,LiteApi::VARS_MODEL);
}
void DebugWidget::setExpand(LiteApi::DEBUG_MODEL_TYPE type, const QModelIndex &index, bool expanded)
{
    if (!index.isValid()) {
        return;
    }
    if (!m_debugger) {
        return;
    }
    QTreeView *view = 0;
    switch (type) {
    case LiteApi::VARS_MODEL:
        view = m_varsView;
        break;
    case LiteApi::ASYNC_MODEL:
        view = m_asyncView;
        break;
    case LiteApi::FRAMES_MODEL:
        view = m_framesView;
        break;
    case LiteApi::THREADS_MODEL:
        view = m_threadsView;
        break;
    case LiteApi::GOROUTINES_MODEL:
        view = m_goroutinesView;
        break;
    case LiteApi::REGS_MODEL:
        view = m_regsView;
        break;
    case LiteApi::LIBRARY_MODEL:
        view = m_libraryView;
        break;
    default:
        view = 0;
    }

    if (view) {
        view->setExpanded(index,expanded);
    }
}

void DebugWidget::watchViewContextMenu(QPoint pos)
{
    QMenu *contextMenu = m_watchMenu;

    if (contextMenu && contextMenu->actions().count() > 0) {
        contextMenu->popup(m_watchView->mapToGlobal(pos));
    }
}

void DebugWidget::loadDebugInfo(const QString &id)
{
    m_watchMap.clear();
    QString key = QString("litedebug_watch/%1").arg(id);
    QStringList all = m_liteApp->settings()->value(key).toStringList();
    m_debugger->setInitWatchList(all);
}

void DebugWidget::saveDebugInfo(const QString &id)
{
    QStringList vars;
    foreach(QString var, m_watchMap.values()) {
        vars.append(var);
    }
    QString key = QString("litedebug_watch/%1").arg(id);
    if (vars.isEmpty()) {
        m_liteApp->settings()->remove(key);
    } else {
        m_liteApp->settings()->setValue(key,vars);
    }
}

void DebugWidget::addWatch()
{
    QString text = QInputDialog::getText(this->m_widget,tr("Add Watch"),tr("Watch expression (e.g. buf main.var os.Stdout):"));
    if (text.isEmpty()) {
        return;
    }
    m_debugger->createWatch(text);
}

void DebugWidget::removeWatch()
{
    QModelIndex index = m_watchView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QModelIndex head = m_watchView->model()->index(index.row(),0);
    if (!head.isValid()) {
        return;
    }
    QString name = head.data(Qt::UserRole + 1).toString();
    m_debugger->removeWatch(name);
}

void DebugWidget::removeAllWatchAct()
{
    m_debugger->removeAllWatch();
    m_watchMap.clear();
}

void DebugWidget::watchCreated(QString var,QString name)
{
    if (!m_watchMap.keys().contains(var)) {
        m_watchMap.insert(var,name);
    }
}

void DebugWidget::watchRemoved(QString var)
{
    m_watchMap.remove(var);
}

void DebugWidget::setInputFocus()
{
    m_debugLogEdit->setFocus();
}

void DebugWidget::dbclickView(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    if (!m_debugger) {
        return;
    }
    QTreeView *view = (QTreeView*)sender();
    if (view == m_asyncView) {
        m_debugger->dbclickItem(index,LiteApi::ASYNC_MODEL);
    } else if (view == m_varsView) {
        m_debugger->dbclickItem(index,LiteApi::VARS_MODEL);
    } else if (view == m_watchView) {
        m_debugger->dbclickItem(index,LiteApi::WATCHES_MODEL);
    } else if (view == m_framesView) {
        m_debugger->dbclickItem(index,LiteApi::FRAMES_MODEL);
    } else if (view == m_threadsView) {
        m_debugger->dbclickItem(index,LiteApi::THREADS_MODEL);
    } else if (view == m_libraryView) {
        m_debugger->dbclickItem(index,LiteApi::LIBRARY_MODEL);
    } else if (view == m_regsView) {
        m_debugger->dbclickItem(index,LiteApi::REGS_MODEL);
    } else if (view == m_goroutinesView) {
        m_debugger->dbclickItem(index,LiteApi::GOROUTINES_MODEL);
    }
}

void DebugWidget::beginUpdateModel(LiteApi::DEBUG_MODEL_TYPE type)
{
    if (type == LiteApi::VARS_MODEL) {
        m_varsView->saveState(&m_varsState);
    } else if (type == LiteApi::WATCHES_MODEL) {
        m_watchView->saveState(&m_watchState);
    } else if (type == LiteApi::GOROUTINES_MODEL) {
        m_goroutinesView->saveState(&m_goroutinesState);
    }
}

void DebugWidget::endUpdateModel(LiteApi::DEBUG_MODEL_TYPE type)
{
    if (type == LiteApi::VARS_MODEL) {
        m_varsView->loadState(m_varsView->model(),&m_varsState);
    } else if (type == LiteApi::WATCHES_MODEL) {
        m_watchView->loadState(m_watchView->model(),&m_watchState);
    } else if (type == LiteApi::GOROUTINES_MODEL) {
        m_goroutinesView->loadState(m_goroutinesView->model(),&m_goroutinesState);
    }
}
