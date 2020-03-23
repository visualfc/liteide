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
// Module: splitwindowstyle.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "splitwindowstyle.h"
#include "rotationtoolbutton.h"
#include "tooldockwidget.h"
#include "liteapp_global.h"
#include "liteapi/liteids.h"
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QStatusBar>
#include <QLabel>
#include <QDockWidget>
#include <QStatusBar>
#include <QKeySequence>
#include <QMenu>
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

ActionGroup::ActionGroup(QObject *parent) : QObject(parent)
{
    current = 0;
}

QList<QAction *> ActionGroup::actions() const
{
    return m_actions;
}

void ActionGroup::addAction(QAction *action)
{
    if(!m_actions.contains(action)) {
        m_actions.append(action);
        QObject::connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    }
    if (current && current->isChecked()) {
        current->setChecked(false);
    }
    if (action->isChecked()) {
        current = action;
    }
}

void ActionGroup::removeAction(QAction *action)
{
    if (m_actions.removeAll(action)) {
        if (action == current)
            current = 0;
        QObject::disconnect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    }
}

QAction * ActionGroup::checkedAction () const
{
    return current;
}

void ActionGroup::actionChanged()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Q_ASSERT_X(action != 0, "ActionGroup::actionChanged", "internal error");
    if (action->isChecked()) {
        if (action != current) {
            if(current)
                current->setChecked(false);
            current = action;
        }
    } else if (action == current) {
        current = 0;
    }
}

SplitActionToolBar::SplitActionToolBar(QSize iconSize, QWidget *parent, Qt::DockWidgetArea _area)
    : QObject(parent), area(_area), bHideToolBar(false)
{
    toolBar = new QToolBar;
    toolBar->hide();
    toolBar->setObjectName(QString("tool_%1").arg(area));
    toolBar->setMovable(false);

    QWidget *spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    spacerAct = toolBar->addWidget(spacer);
    toolBar->addSeparator();
    QWidget *spacer2 = new QWidget;
    spacer2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    toolBar->addWidget(spacer2);

    dock1 = new SplitDockWidget(iconSize, parent);
    dock1->setObjectName(QString("dock_%1").arg(area));
    dock1->setWindowTitle(QString("dock_%1").arg(area));
    dock1->setFeatures(QDockWidget::DockWidgetClosable);
    dock1->hide();
    dock1->createMenu(area,false);

    dock2 = new SplitDockWidget(iconSize,parent);
    dock2->setObjectName(QString("dock_%1_split").arg(area));
    dock2->setWindowTitle(QString("dock_%1_split").arg(area));
    dock2->setFeatures(QDockWidget::DockWidgetClosable);
    dock2->hide();
    dock2->createMenu(area,true);

    connect(dock1,SIGNAL(visibilityChanged(bool)),this,SLOT(dock1Visible(bool)));
    connect(dock2,SIGNAL(visibilityChanged(bool)),this,SLOT(dock2Visible(bool)));
    connect(dock1,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*,bool)),this,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*,bool)));
    connect(dock2,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*,bool)),this,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*,bool)));
}

SplitDockWidget *SplitActionToolBar::dock(bool split) const
{
    return split?dock2:dock1;
}

void SplitActionToolBar::addAction(QAction *action, const QString &title, bool split)
{
    RotationToolButton *btn = new RotationToolButton;
    btn->setDefaultAction(action);
    if (area == Qt::LeftDockWidgetArea) {
        btn->setRotation(RotationToolButton::CounterClockwise);
    } else if (area == Qt::RightDockWidgetArea) {
        btn->setRotation(RotationToolButton::Clockwise);
    }
    m_actionWidgetMap.insert(action,btn);
    if (split) {
        dock2->addAction(action,title);
        toolBar->addWidget(btn);
    } else {
        dock1->addAction(action,title);
        toolBar->insertWidget(spacerAct,btn);
    }
    if (toolBar->isHidden() && !bHideToolBar) {
        toolBar->show();
    }
}

void SplitActionToolBar::removeAction(QAction *action, bool split)
{
    QWidget *widget = m_actionWidgetMap.value(action);
    if (widget) {
        delete widget;
    }
    m_actionWidgetMap.remove(action);
    if (split) {
        dock2->removeAction(action);
    } else {
        dock1->removeAction(action);
    }
    if (dock1->actions().isEmpty() && dock2->actions().isEmpty()) {
        toolBar->hide();
    }
}

void SplitActionToolBar::setHideToolBar(bool b)
{
    bHideToolBar = b;
    if (bHideToolBar) {
        toolBar->hide();
    } else {
        if (!dock1->actions().isEmpty() || !dock2->actions().isEmpty()){
            toolBar->show();
        }
    }
}

void SplitActionToolBar::dock1Visible(bool b)
{
    QAction *action = dock1->checkedAction();
    if (action) {
        action->setChecked(dock1->isVisible());
    } else if (b && !dock1->actions().isEmpty()) {
        dock1->actions().first()->setChecked(true);
    }
}

void SplitActionToolBar::dock2Visible(bool b)
{
    QAction *action = dock2->checkedAction();
    if (action) {
        action->setChecked(dock2->isVisible());
    } else if (b && !dock2->actions().isEmpty()) {
        dock2->actions().first()->setChecked(true);
    }
}


SplitWindowStyle::SplitWindowStyle(LiteApi::IApplication *app, QMainWindow *window, QObject *parent)
    : IWindowStyle(parent), m_liteApp(app), m_mainWindow(window)
{
    QSize iconSize = LiteApi::getToolBarIconSize(app);
    m_areaToolBar.insert(Qt::TopDockWidgetArea,new SplitActionToolBar(iconSize, m_mainWindow,Qt::TopDockWidgetArea));
    m_areaToolBar.insert(Qt::BottomDockWidgetArea,new SplitActionToolBar(iconSize, m_mainWindow,Qt::BottomDockWidgetArea));
    m_areaToolBar.insert(Qt::LeftDockWidgetArea,new SplitActionToolBar(iconSize, m_mainWindow,Qt::LeftDockWidgetArea));
    m_areaToolBar.insert(Qt::RightDockWidgetArea,new SplitActionToolBar(iconSize, m_mainWindow,Qt::RightDockWidgetArea));

    QMapIterator<Qt::DockWidgetArea,SplitActionToolBar*> it(m_areaToolBar);
    while(it.hasNext()) {
        it.next();
        Qt::DockWidgetArea area = it.key();
        SplitActionToolBar *actionToolBar = it.value();
        m_mainWindow->addToolBar((Qt::ToolBarArea)area,actionToolBar->toolBar);
        m_mainWindow->addDockWidget(area,actionToolBar->dock1);
        m_mainWindow->addDockWidget(area,actionToolBar->dock2);
        if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea)
            m_mainWindow->splitDockWidget(actionToolBar->dock1,actionToolBar->dock2,Qt::Horizontal);
        else
            m_mainWindow->splitDockWidget(actionToolBar->dock1,actionToolBar->dock2,Qt::Vertical);
        connect(actionToolBar,SIGNAL(moveActionTo(Qt::DockWidgetArea, Qt::DockWidgetArea,QAction*,bool)),this,SLOT(moveToolWindow(Qt::DockWidgetArea, Qt::DockWidgetArea,QAction*,bool)));
    }

    m_mainWindow->setDockNestingEnabled(true);
    m_mainWindow->setDockOptions(QMainWindow::AllowNestedDocks);

    m_statusBar = new QStatusBar;

    m_hideSideAct = new QAction(tr("Hide Sidebars"),this);
    m_hideSideAct->setIcon(QIcon("icon:images/hidesidebar.png"));
    m_hideSideAct->setCheckable(true);

    QToolButton *btn = new QToolButton;
    btn->setDefaultAction(m_hideSideAct);
    btn->setStyleSheet("QToolButton {border:0}"
                       "QToolButton:checked {background : qlineargradient(spread:pad, x1:0, y1:1, x2:1, y2:0, stop:0 rgba(55, 57, 59, 255), stop:1 rgba(255, 255, 255, 255));}");
    m_statusBar->addWidget(btn);

    m_statusBar->setContentsMargins(0,0,0,0);

    SplitActionToolBar *bar = m_areaToolBar.value(Qt::BottomDockWidgetArea);
    if (bar) {
        m_statusBar->addWidget(bar->toolBar,1);
    }

    m_mainWindow->setStatusBar(m_statusBar);

    m_windowMenu = 0;

    connect(m_hideSideAct,SIGNAL(toggled(bool)),this,SLOT(hideSideBar(bool)));
    m_useShortcuts = m_liteApp->settings()->value(LITEAPP_TOOLWINDOW_SHORTCUTS,true).toBool();
}

SplitWindowStyle::~SplitWindowStyle()
{
    qDeleteAll(m_actStateMap);
}

void SplitWindowStyle::createToolWindowMenu()
{
    QMenu *menu = m_liteApp->actionManager()->loadMenu(ID_MENU_VIEW);
    if (menu) {
        m_windowMenu = menu->addMenu(tr("Tool Windows"));
    }
}

void SplitWindowStyle::toggledAction(bool)
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    SplitActionState *state = m_actStateMap.value(action);
    if (!state) {
        return;
    }
    SplitDockWidget *dock = m_areaToolBar.value(state->area)->dock(state->split);
    if (action->isChecked()) {
        if (dock->isHidden()) {
            dock->show();
        }
        dock->setWidget(state->widget);
        if (!state->widgetList.isEmpty()) {
            dock->setWidgetList(state->widgetList);
        } else {
            dock->setWidgetActions(state->widgetActions);
        }
        dock->setWindowTitle(state->title);
        state->widget->setVisible(true);
    } else {
        if (!dock->checkedAction()) {
            dock->hide();
            state->widget->setVisible(false);
        }
    }
}

QAction *SplitWindowStyle::findToolWindow(QWidget *widget)
{
    QMapIterator<QAction*,SplitActionState*> it(m_actStateMap);
    while (it.hasNext()) {
        it.next();
        if (it.value()->widget == widget) {
            return it.key();
        }
    }
    return NULL;
}

void SplitWindowStyle::removeToolWindow(QAction *action)
{
    SplitActionState *state = m_actStateMap.value(action);
    if (!state) {
        return;
    }
    if (action->isChecked()) {
        action->setChecked(false);
    }
    SplitActionToolBar *actToolBar = m_areaToolBar.value(state->area);
    if (actToolBar) {
        actToolBar->removeAction(action,state->split);
    }
}

QAction *SplitWindowStyle::addToolWindow(LiteApi::IApplication *app,Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split, QList<QAction*> widgetActions, QList<QWidget*> widgetList)
{
//    QMap<QString,SplitInitToolSate>::iterator it = m_initIdStateMap.find(id);
//    if (it != m_initIdStateMap.end()) {
//        area = it.value().area;
//        split = it.value().split;
//    }
    area = (Qt::DockWidgetArea)m_liteApp->settings()->value("split_area/"+id,area).toInt();
    split = m_liteApp->settings()->value("split_split/"+id,split).toBool();


    SplitActionToolBar *actToolBar = m_areaToolBar.value(area);
    QAction *action = new QAction(this);
    action->setText(title);
    action->setCheckable(true);
    action->setObjectName(id);

    SplitActionState *state = new SplitActionState;
    state->area = area;
    state->split = split;
    state->widget = widget;
    state->widgetList = widgetList;
    state->widgetActions = widgetActions;
    state->id = id;
    state->title = title;

    actToolBar->addAction(action,title,split);

    action->setText(title);
    int index = m_actStateMap.size();
    if ((index <= 9) && m_useShortcuts) {
        action->setText(QString("%1: %2").arg(index).arg(title));
        QKeySequence ks(LiteApi::UseMacShortcuts?QString("Ctrl+Alt+%1").arg(index):QString("Alt+%1").arg(index));
        LiteApi::IActionContext *actionContext = app->actionManager()->getActionContext(app,"App");
        actionContext->regAction(action,"ToolWindow_"+id,ks.toString());
    }
    m_actStateMap.insert(action,state);    

    connect(action,SIGNAL(toggled(bool)),this,SLOT(toggledAction(bool)));
    if (m_windowMenu) {
        m_windowMenu->addAction(action);
    }
    return action;
}

void SplitWindowStyle::moveToolWindow(Qt::DockWidgetArea from, Qt::DockWidgetArea to, QAction *action, bool split)
{
    SplitActionState *state = m_actStateMap.value(action);
    if (!state) {
        return;
    }
    if (state->area == to && state->split == split) {
        return;
    }
    SplitActionToolBar *actionToolBar = m_areaToolBar.value(to);
    SplitActionToolBar *oldActToolBar = m_areaToolBar.value(from);

    if (action->isChecked()) {
        action->setChecked(false);
    }

    oldActToolBar->removeAction(action,state->split);
    actionToolBar->addAction(action,state->title,split);

    state->area = to;
    state->split = split;
    action->setChecked(true);
}

void SplitWindowStyle::restoreToolWindows()
{
    foreach(QAction *action,m_hideActionList) {
        action->setChecked(true);
    }
    m_hideActionList.clear();
}

void SplitWindowStyle::showOrHideToolWindow()
{
    bool hide = false;
    foreach(QAction *action, m_actStateMap.keys()) {
        if (action->isChecked()) {
            hide = true;
            break;
        }
    }
    if (hide) {
        hideAllToolWindows();
    } else {
        restoreToolWindows();
    }
}

void SplitWindowStyle::hideAllToolWindows()
{
    m_hideActionList.clear();
    foreach(QAction *action, m_actStateMap.keys()) {
        if (action->isChecked()) {
            m_hideActionList.append(action);
            action->setChecked(false);
        }
    }
}

void SplitWindowStyle::hideOutputWindow()
{
    this->hideToolWindow(Qt::TopDockWidgetArea);
    this->hideToolWindow(Qt::BottomDockWidgetArea);
}

void SplitWindowStyle::hideSideBar(bool b)
{
    QMapIterator<Qt::DockWidgetArea,SplitActionToolBar*> it(m_areaToolBar);
    while (it.hasNext()) {
        it.next();
        if (it.key() != Qt::BottomDockWidgetArea) {
            it.value()->setHideToolBar(b);
        }
    }
}

void SplitWindowStyle::saveToolState() const
{
    QMapIterator<QAction*,SplitActionState*> i(m_actStateMap);
    while (i.hasNext()) {
        i.next();
        SplitActionState *state = i.value();
        m_liteApp->settings()->setValue("split_area/"+state->id,state->area);
        m_liteApp->settings()->setValue("split_split/"+state->id,state->split);
        m_liteApp->settings()->setValue("split_check/"+state->id,i.key()->isChecked());
    }
    m_liteApp->settings()->setValue("split_side_hide",m_hideSideAct->isChecked());
}

void SplitWindowStyle::restoreToolsState()
{
    m_hideSideAct->setChecked(m_liteApp->settings()->value("split_side_hide").toBool());
}

void SplitWindowStyle::updateConer()
{
    m_mainWindow->setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
    m_mainWindow->setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);
}

void SplitWindowStyle::hideToolWindow(Qt::DockWidgetArea area)
{
    SplitActionToolBar *bar = m_areaToolBar.value(area);
    if (bar) {
        bar->dock1->close();
        bar->dock2->close();
    }
}
