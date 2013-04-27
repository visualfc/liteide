/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


DebugWidget::DebugWidget(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    m_debugger(0)
{
    m_tabWidget = new QTabWidget;

    m_asyncView = new QTreeView;
    m_varsView = new QTreeView;
    m_watchView = new QTreeView;
    m_statckView = new QTreeView;
    m_libraryView = new QTreeView;

    m_asyncView->setEditTriggers(0);

    m_varsView->setEditTriggers(0);
    m_varsView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_watchView->setEditTriggers(0);
    m_watchView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_statckView->setEditTriggers(0);
    m_libraryView->setEditTriggers(0);

    m_debugLogEdit = new TerminalEdit;
    m_debugLogEdit->setReadOnly(false);
    m_debugLogEdit->setMaximumBlockCount(1024);

    m_tabWidget->addTab(m_asyncView,tr("Async Record"));
    m_tabWidget->addTab(m_varsView,tr("Variables"));
    m_tabWidget->addTab(m_watchView,tr("Watch"));
    m_tabWidget->addTab(m_statckView,tr("Call Stack"));
    m_tabWidget->addTab(m_libraryView,tr("Libraries"));
    m_tabWidget->addTab(m_debugLogEdit,tr("Console"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_tabWidget);

    m_widget->setLayout(layout);

    m_watchMenu = new QMenu(m_widget);
    m_addWatchAct = new QAction(tr("Add Global Watch"),this);
    m_addLocalWatchAct = new QAction(tr("Add Local Watch"),this);
    m_removeWatchAct = new QAction(tr("Remove Watch"),this);
    m_removeAllWatchAct = new QAction(tr("Remove All Watches"),this);
    m_watchMenu->addAction(m_addWatchAct);
    m_watchMenu->addAction(m_addLocalWatchAct);
    m_watchMenu->addSeparator();
    m_watchMenu->addAction(m_removeWatchAct);
    m_watchMenu->addAction(m_removeAllWatchAct);

    connect(m_debugLogEdit,SIGNAL(enterText(QString)),this,SLOT(enterText(QString)));
    connect(m_varsView,SIGNAL(expanded(QModelIndex)),this,SLOT(expandedVarsView(QModelIndex)));
    connect(m_watchView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(watchViewContextMenu(QPoint)));
    connect(m_addWatchAct,SIGNAL(triggered()),this,SLOT(addWatch()));
    connect(m_addLocalWatchAct,SIGNAL(triggered()),this,SLOT(addLocalWatch()));
    connect(m_removeWatchAct,SIGNAL(triggered()),this,SLOT(removeWatch()));
    connect(m_removeAllWatchAct,SIGNAL(triggered()),this,SLOT(removeAllWatchAct()));
}

DebugWidget::~DebugWidget()
{
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
        m_debugger->command(cmd.toUtf8());
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
    }
    view->header()->setResizeMode(0,QHeaderView::ResizeToContents);
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
    m_asyncView->setModel(debug->debugModel(LiteApi::ASYNC_MODEL));
    m_varsView->setModel(debug->debugModel(LiteApi::VARS_MODEL));
    m_watchView->setModel(debug->debugModel(LiteApi::WATCHES_MODEL));
    m_statckView->setModel(debug->debugModel(LiteApi::CALLSTACK_MODEL));
    m_libraryView->setModel(debug->debugModel(LiteApi::LIBRARY_MODEL));
    setResizeView(m_asyncView);
    setResizeView(m_varsView);
    setResizeView(m_watchView);
    setResizeView(m_statckView);
    setResizeView(m_libraryView);
    connect(m_debugger,SIGNAL(setExpand(LiteApi::DEBUG_MODEL_TYPE,QModelIndex,bool)),this,SLOT(setExpand(LiteApi::DEBUG_MODEL_TYPE,QModelIndex,bool)));
    connect(m_debugger,SIGNAL(watchCreated(QString,QString)),this,SLOT(watchCreated(QString,QString)));
    connect(m_debugger,SIGNAL(watchRemoved(QString)),this,SLOT(watchRemoved(QString)));
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
    case LiteApi::CALLSTACK_MODEL:
        view = m_statckView;
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
    foreach(QString var, m_liteApp->settings()->value(id).toStringList()) {
        if (var.indexOf(".") < 0) {
            //local var
            m_debugger->createWatch(var,true,false);
        }
        m_debugger->createWatch(var,false,true);
    }
}

void DebugWidget::saveDebugInfo(const QString &id)
{
    QStringList vars;
    foreach(QString var, m_watchMap.values()) {
        vars.append(var);
    }
    m_liteApp->settings()->setValue(id,vars);
}

void DebugWidget::addWatch()
{
    QString text = QInputDialog::getText(this->m_widget,tr("Add Global Watch"),tr("Watch expression (e.g. main.var os.Stdout):"));
    if (text.isEmpty()) {
        return;
    }
    if (text.indexOf(".") >= 0) {
        text = QString("\'%1'").arg(text);
    }
    m_debugger->createWatch(text,false,true);
}

void DebugWidget::addLocalWatch()
{
    QString text = QInputDialog::getText(this->m_widget,tr("Add Local Watch"),tr("Watch expression (e.g. s1.str):"));
    if (text.isEmpty()) {
        return;
    }
    m_debugger->createWatch(text,false,true);
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
    m_debugger->removeWatchByName(name,true);
}

void DebugWidget::removeAllWatchAct()
{
    QStringList watchList;
    for (int i = 0; i < m_watchView->model()->rowCount(); i++) {
        QModelIndex index = m_watchView->model()->index(i,0);
        if (index.isValid()) {
            watchList << index.data(Qt::DisplayRole).toString();
        }
    }
    foreach(QString var, watchList) {
        m_debugger->removeWatch(var,true);
    }
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

