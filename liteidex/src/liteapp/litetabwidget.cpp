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
// Module: litetabwidget.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: litetabwidget.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "litetabwidget.h"

#include <QTabBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QToolButton>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QKeyEvent>
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


LiteTabWidget::LiteTabWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_headLayout = new QHBoxLayout;
    m_headLayout->setMargin(0);

    m_tabBar = new QTabBar(this);
    m_tabBar->setExpanding(false);
    m_tabBar->setDocumentMode(true);
    m_tabBar->setDrawBase(false);
    m_tabBar->setUsesScrollButtons(true);

    m_headerToolBar = new QToolBar(this);
    m_headerToolBar->setStyleSheet("QToolBar {border:0}");
    m_headerToolBar->setIconSize(QSize(15,16));

    m_closeTabAct = new QAction(QIcon("icon:images/closepage.png"),tr("Close Page"),this);
    m_listButton = new QToolButton(m_headerToolBar);
    m_listButton->setToolTip(tr("List Page"));
    m_listButton->setIcon(QIcon("icon:images/listpage.png"));
    m_addTabAct = new QAction(QIcon("icon:images/addpage.png"),tr("Add Page"),this);

    m_listActMenu = new QMenu(this);
    m_listActGroup = new QActionGroup(this);
    m_listButton->setMenu(m_listActMenu);
    m_listButton->setPopupMode(QToolButton::InstantPopup);

    m_headerToolBar->addWidget(m_listButton);
    m_headerToolBar->addSeparator();
    m_headerToolBar->addAction(m_addTabAct);
    m_headerToolBar->addSeparator();
    m_headerToolBar->addAction(m_closeTabAct);

    m_stackLayout = new QStackedLayout;
    m_stackLayout->setMargin(0);
    m_stackLayout->setSpacing(0);

    m_headLayout->addWidget(m_tabBar,1);
    m_headLayout->addWidget(m_headerToolBar);

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(m_headLayout);
    mainLayout->addLayout(m_stackLayout);

    this->setLayout(mainLayout);

    connect(m_tabBar,SIGNAL(currentChanged(int)),this,SLOT(tabCurrentChanged(int)));
    connect(m_tabBar,SIGNAL(tabCloseRequested(int)),this,SIGNAL(tabCloseRequested(int)));
    connect(m_closeTabAct,SIGNAL(triggered()),this,SLOT(closeCurrentTab()));
    connect(m_addTabAct,SIGNAL(triggered()),this,SIGNAL(tabAddRequest()));
    connect(m_listActGroup,SIGNAL(triggered(QAction*)),this,SLOT(selectListActGroup(QAction*)));

    m_listButton->setEnabled(false);
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
    m_stackLayout->addWidget(w);
    m_widgetList.append(w);

    return index;
}

void LiteTabWidget::removeTab(int index)
{
    if (index < 0)
        return;
    QWidget *w = widget(index);
    if (w) {
        m_stackLayout->removeWidget(w);
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
    return m_stackLayout->currentWidget();
}

QTabBar *LiteTabWidget::tabBar()
{
    return m_tabBar;
}

QList<QWidget*> LiteTabWidget::widgetList() const
{
    return m_widgetList;
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
        m_stackLayout->setCurrentWidget(w);
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
