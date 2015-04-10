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
// Module: litetabwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litetabwidget.h"
#include "liteapi/liteapi.h"
#include <QTabBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QToolButton>
#include <QPushButton>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QKeyEvent>
#include <QDebug>
#include "memory.h"


LiteTabWidget::LiteTabWidget(QSize iconSize, QObject *parent) :
    QObject(parent)
{
    m_tabBar = new QTabBar;
    m_tabBar->setExpanding(false);
    m_tabBar->setDocumentMode(true);
    m_tabBar->setDrawBase(false);
    m_tabBar->setUsesScrollButtons(true);
    m_tabBar->setMovable(true);
    m_tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);

    m_dumpToolBar = new QToolBar;
    m_dumpToolBar->setObjectName("toolbar.tabs");
    m_dumpToolBar->setIconSize(iconSize);

    m_tabBarWidget = new QWidget;

    m_addTabAct = new QAction(QIcon("icon:images/addpage.png"),tr("Open a new tab"),this);
    m_listActMenu = new QMenu;
    m_listActGroup = new QActionGroup(this);

    m_listButton = new QToolButton(m_dumpToolBar);
    m_listButton->setToolTip(tr("List All Tabs"));
    m_listButton->setIcon(QIcon("icon:images/listpage.png"));
    m_listButton->setMenu(m_listActMenu);
    m_listButton->setPopupMode(QToolButton::InstantPopup);
    m_listButton->setStyleSheet("QToolButton::menu-indicator{image:none;}");

    m_closeTabAct = new QAction(QIcon("icon:images/closetool.png"),tr("Close Tab"),this);
    m_closeButton = new QToolButton(m_dumpToolBar);
    m_closeButton->setDefaultAction(m_closeTabAct);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_tabBar,1);
    layout->addWidget(m_listButton);
    layout->addWidget(m_closeButton);

    m_tabBarWidget->setLayout(layout);

    m_stackedWidget = new QStackedWidget;

    connect(m_tabBar,SIGNAL(currentChanged(int)),this,SLOT(tabCurrentChanged(int)));
    connect(m_tabBar,SIGNAL(tabCloseRequested(int)),this,SIGNAL(tabCloseRequested(int)));
    connect(m_tabBar,SIGNAL(tabMoved(int,int)),this,SLOT(tabMoved(int,int)));
    connect(m_closeTabAct,SIGNAL(triggered()),this,SLOT(closeCurrentTab()));
    connect(m_addTabAct,SIGNAL(triggered()),this,SIGNAL(tabAddRequest()));
    connect(m_listActGroup,SIGNAL(triggered(QAction*)),this,SLOT(selectListActGroup(QAction*)));

    m_listButton->setEnabled(false);
}

LiteTabWidget::~LiteTabWidget()
{
    delete m_listActMenu;
    delete m_tabBarWidget;
    delete m_dumpToolBar;
}

void LiteTabWidget::closeCurrentTab()
{
    int index = m_tabBar->currentIndex();
    if (index < 0) {
        return;
    }
    emit tabCloseRequested(index);
}

void LiteTabWidget::selectListActGroup(QAction *act)
{
    int index = m_listActGroup->actions().indexOf(act);
    if (index < 0) {
        return;
    }
    setCurrentIndex(index);
}

int LiteTabWidget::addTab(QWidget *w,const QString & label, const QString &tip)
{
    return addTab(w,QIcon(),label,tip);
}

int LiteTabWidget::addTab(QWidget *w,const QIcon & icon, const QString & label, const QString &tip)
{
    if (!w) {
        return -1;
    }

    if (m_widgetList.size() == 0) {
        m_listButton->setEnabled(true);
    }

    QAction *act = m_listActGroup->addAction(icon,label);
    act->setCheckable(true);
    m_listActMenu->addAction(act);

    int index = m_tabBar->addTab(icon,label);
    if (!tip.isEmpty()) {
        m_tabBar->setTabToolTip(index,tip);
    }
    m_stackedWidget->addWidget(w);
    m_widgetList.append(w);

    return index;
}

void LiteTabWidget::removeTab(int index)
{
    if (index < 0)
        return;
    QWidget *w = widget(index);
    if (w) {
        m_stackedWidget->removeWidget(w);
        m_widgetList.removeAt(index);
    }

    QAction *act = m_listActGroup->actions().value(index);
    if (act) {
        m_listActMenu->removeAction(act);
        m_listActGroup->removeAction(act);
    }
    if (m_widgetList.size() == 0) {
        m_listButton->setEnabled(false);
    }

    m_tabBar->removeTab(index);
}

QWidget *LiteTabWidget::currentWidget()
{
    return m_stackedWidget->currentWidget();
}

QTabBar *LiteTabWidget::tabBar()
{
    return m_tabBar;
}

QList<QWidget*> LiteTabWidget::widgetList() const
{
    return m_widgetList;
}

QWidget *LiteTabWidget::stackedWidget()
{
    return m_stackedWidget;
}

QWidget *LiteTabWidget::tabBarWidget()
{
    return m_tabBarWidget;
}

void LiteTabWidget::setTabText(int index, const QString & text)
{
    QAction *act = m_listActGroup->actions().value(index);
    if (act) {
        act->setText(text);
    }
    m_tabBar->setTabText(index,text);
}

int LiteTabWidget::indexOf(QWidget *w)
{
    return m_widgetList.indexOf(w);
}

QWidget *LiteTabWidget::widget(int index)
{
    return m_widgetList.value(index);
}

void LiteTabWidget::setCurrentWidget(QWidget *w)
{
    int index = indexOf(w);
    if (index < 0)
        return;
    setCurrentIndex(index);
}

void LiteTabWidget::tabCurrentChanged(int index)
{
    QWidget *w = m_widgetList.value(index);
    if (w) {
        m_stackedWidget->setCurrentWidget(w);
    }

    QAction *act = m_listActGroup->actions().value(index);
    if (act) {
        act->setChecked(true);
    }

    emit currentChanged(index);
}

void LiteTabWidget::setCurrentIndex(int index)
{
    m_tabBar->setCurrentIndex(index);
}

void LiteTabWidget::tabMoved(int from, int to)
{
    m_widgetList.swap(from,to);
}
