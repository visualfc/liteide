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
// Module: toolmainwindow.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "toolmainwindow.h"
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QStatusBar>
#include <QLabel>
#include <QDockWidget>
#include <QStatusBar>
#include <QKeySequence>
#include <QMenu>
#include "rotationtoolbutton.h"
#include "tooldockwidget.h"
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

ActionToolBar::ActionToolBar(QWidget *parent, Qt::DockWidgetArea _area)
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

    dock1 = new ToolDockWidget(parent);
    dock1->setObjectName(QString("dock_%1").arg(area));
    dock1->setWindowTitle(QString("dock_%1").arg(area));
    dock1->setFeatures(QDockWidget::DockWidgetClosable);
    dock1->hide();
    dock1->createMenu(area,false);

    dock2 = new ToolDockWidget(parent);
    dock2->setObjectName(QString("dock_%1_split").arg(area));
    dock2->setWindowTitle(QString("dock_%1_split").arg(area));
    dock2->setFeatures(QDockWidget::DockWidgetClosable);
    dock2->hide();
    dock2->createMenu(area,true);

    connect(dock1,SIGNAL(visibilityChanged(bool)),this,SLOT(dock1Visible(bool)));
    connect(dock2,SIGNAL(visibilityChanged(bool)),this,SLOT(dock2Visible(bool)));
    connect(dock1,SIGNAL(moveActionTo(Qt::DockWidgetArea,QAction*,bool)),this,SIGNAL(moveActionTo(Qt::DockWidgetArea,QAction*,bool)));
    connect(dock2,SIGNAL(moveActionTo(Qt::DockWidgetArea,QAction*,bool)),this,SIGNAL(moveActionTo(Qt::DockWidgetArea,QAction*,bool)));
}

ToolDockWidget *ActionToolBar::dock(bool split) const
{
    return split?dock2:dock1;
}

void ActionToolBar::addAction(QAction *action, const QString &title, bool split)
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

void ActionToolBar::removeAction(QAction *action, bool split)
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

void ActionToolBar::setHideToolBar(bool b)
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

void ActionToolBar::dock1Visible(bool b)
{
    QAction *action = dock1->checkedAction();
    if (action) {
        action->setChecked(dock1->isVisible());
    } else if (b && !dock1->actions().isEmpty()) {
        dock1->actions().first()->setChecked(true);
    }
}

void ActionToolBar::dock2Visible(bool b)
{
    QAction *action = dock2->checkedAction();
    if (action) {
        action->setChecked(dock2->isVisible());
    } else if (b && !dock2->actions().isEmpty()) {
        dock2->actions().first()->setChecked(true);
    }
}


ToolMainWindow::ToolMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_areaToolBar.insert(Qt::TopDockWidgetArea,new ActionToolBar(this,Qt::TopDockWidgetArea));
    m_areaToolBar.insert(Qt::BottomDockWidgetArea,new ActionToolBar(this,Qt::BottomDockWidgetArea));
    m_areaToolBar.insert(Qt::LeftDockWidgetArea,new ActionToolBar(this,Qt::LeftDockWidgetArea));
    m_areaToolBar.insert(Qt::RightDockWidgetArea,new ActionToolBar(this,Qt::RightDockWidgetArea));

    QMapIterator<Qt::DockWidgetArea,ActionToolBar*> it(m_areaToolBar);
    while(it.hasNext()) {
        it.next();
        Qt::DockWidgetArea area = it.key();
        ActionToolBar *actionToolBar = it.value();
        addToolBar((Qt::ToolBarArea)area,actionToolBar->toolBar);
        addDockWidget(area,actionToolBar->dock1);
        addDockWidget(area,actionToolBar->dock2);
        if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea)
            splitDockWidget(actionToolBar->dock1,actionToolBar->dock2,Qt::Horizontal);
        else
            splitDockWidget(actionToolBar->dock1,actionToolBar->dock2,Qt::Vertical);
        connect(actionToolBar,SIGNAL(moveActionTo(Qt::DockWidgetArea,QAction*,bool)),this,SLOT(moveToolWindow(Qt::DockWidgetArea,QAction*,bool)));
    }

    this->setDockNestingEnabled(true);
    this->setDockOptions(QMainWindow::AllowNestedDocks);

    m_statusBar = new QStatusBar;

    m_hideSideAct = new QAction(tr("Hide Sidebars"),this);
    m_hideSideAct->setIcon(QIcon("icon:images/hidesidebar.png"));
    m_hideSideAct->setCheckable(true);

    QToolButton *btn = new QToolButton;
    btn->setDefaultAction(m_hideSideAct);
    btn->setStyleSheet("QToolButton {border:0}"
                       "QToolButton:checked {background : qlineargradient(spread:pad, x1:0, y1:1, x2:1, y2:0, stop:0 rgba(55, 57, 59, 255), stop:1 rgba(255, 255, 255, 255));}");
    m_statusBar->addWidget(btn);

    ActionToolBar *bar = m_areaToolBar.value(Qt::BottomDockWidgetArea);
    //bar->toolBar->setStyleSheet("QToolBar {border:0}");
    m_statusBar->setContentsMargins(0,0,0,0);
    m_statusBar->addWidget(bar->toolBar,1);

    this->setStatusBar(m_statusBar);

    //this->setStyleSheet("QMainWindow::separator{width:1; background-color: gray ;}");
    //m_statusBar->setStyleSheet("QStatusBar {border-top: 1px solid gray}");
    //m_statusBar->setStyleSheet("QStatusBar {border:0}");
    /*
    this->setStyleSheet("QToolBar {border:1 ; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #EEEEEE, stop: 1 #ababab); color : #EEEEEE}"
                        "QStatusBar {border:1 ; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #EEEEEE, stop: 1 #ababab); color : #EEEEEE}"
                        "QMainWindow::separator{width:1; background-color: #ababab ;}");
    */

    QAction *hideBottomAct = new QAction("HideBottom",this);
    hideBottomAct->setShortcut(QKeySequence(Qt::Key_Escape));

    this->addAction(hideBottomAct);
    connect(hideBottomAct,SIGNAL(triggered()),bar->dock1,SLOT(close()));
    connect(hideBottomAct,SIGNAL(triggered()),bar->dock2,SLOT(close()));

    connect(m_hideSideAct,SIGNAL(toggled(bool)),this,SLOT(hideSideBar(bool)));
}

ToolMainWindow::~ToolMainWindow()
{
    qDeleteAll(m_actStateMap);
}

void ToolMainWindow::toggledAction(bool)
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    ActionState *state = m_actStateMap.value(action);
    if (!state) {
        return;
    }
    ToolDockWidget *dock = m_areaToolBar.value(state->area)->dock(state->split);
    if (action->isChecked()) {
        if (dock->isHidden()) {
            dock->show();
        }
        dock->setWidget(state->widget);
        dock->setWidgetActions(state->widgetActions);
        dock->setWindowTitle(state->title);
    } else {
        if (!dock->checkedAction()) {
            dock->hide();
        }
    }
}

QAction *ToolMainWindow::findToolWindow(QWidget *widget)
{
    QMapIterator<QAction*,ActionState*> it(m_actStateMap);
    while (it.hasNext()) {
        it.next();
        if (it.value()->widget == widget) {
            return it.key();
        }
    }
    return NULL;
}

void ToolMainWindow::removeToolWindow(QAction *action)
{
    ActionState *state = m_actStateMap.value(action);
    if (!state) {
        return;
    }
    if (action->isChecked()) {
        action->setChecked(false);
    }
    ActionToolBar *actToolBar = m_areaToolBar.value(state->area);
    if (actToolBar) {
        actToolBar->removeAction(action,state->split);
    }
}

QAction *ToolMainWindow::addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split, QList<QAction*> widgetActions)
{
    QMap<QString,InitToolSate>::iterator it = m_initIdStateMap.find(id);
    bool checked = true;
    if (it != m_initIdStateMap.end()) {
        area = it.value().area;
        split = it.value().split;
        checked = it.value().checked;
    }

    ActionToolBar *actToolBar = m_areaToolBar.value(area);
    QAction *action = new QAction(this);
    action->setText(title);
    action->setCheckable(true);
    action->setObjectName(id);

    ActionState *state = new ActionState;
    state->area = area;
    state->split = split;
    state->widget = widget;
    state->widgetActions = widgetActions;
    state->id = id;
    state->title = title;

    actToolBar->addAction(action,title,split);

    int index = m_actStateMap.size();
    if (index <= 9) {
        action->setText(QString("&%1: %2").arg(index).arg(title));
        action->setToolTip(tr("\"%1\" Tool Window\tALT+%2").arg(title).arg(index));
        action->setShortcut(QKeySequence(QString("ALT+%1").arg(index)));
    }
    m_actStateMap.insert(action,state);

    connect(action,SIGNAL(toggled(bool)),this,SLOT(toggledAction(bool)));
    return action;
}

void ToolMainWindow::moveToolWindow(Qt::DockWidgetArea area,QAction *action,bool split)
{
    ActionState *state = m_actStateMap.value(action);
    if (!state) {
        return;
    }
    if (state->area == area && state->split == split) {
        return;
    }
    ActionToolBar *actionToolBar = m_areaToolBar.value(area);
    ActionToolBar *oldActToolBar = m_areaToolBar.value(state->area);

    if (action->isChecked()) {
        action->setChecked(false);
    }

    oldActToolBar->removeAction(action,state->split);
    actionToolBar->addAction(action,state->title,split);

    state->area = area;
    state->split = split;
    action->setChecked(true);
}

void ToolMainWindow::restoreToolWindows(){
    foreach(QAction *action,m_hideActionList) {
        action->setChecked(true);
    }
    m_hideActionList.clear();
}

void ToolMainWindow::showOrHideToolWindow()
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

void ToolMainWindow::hideAllToolWindows()
{
    m_hideActionList.clear();
    foreach(QAction *action, m_actStateMap.keys()) {
        if (action->isChecked()) {
            m_hideActionList.append(action);
            action->setChecked(false);
        }
    }
}

void ToolMainWindow::hideSideBar(bool b)
{
    QMapIterator<Qt::DockWidgetArea,ActionToolBar*> it(m_areaToolBar);
    while (it.hasNext()) {
        it.next();
        if (it.key() != Qt::BottomDockWidgetArea) {
            it.value()->setHideToolBar(b);
        }
    }
}

static int VersionMarker = 0xffe0;

QByteArray ToolMainWindow::saveToolState(int version) const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << VersionMarker;
    stream << version;
    QMapIterator<QAction*,ActionState*> it(m_actStateMap);
    while (it.hasNext()) {
        it.next();
        ActionState *state = it.value();
        stream << state->id;
        stream << (int)state->area;
        stream << state->split;
        stream << it.key()->isChecked();
    }
    return data;
}

bool ToolMainWindow::restoreState(const QByteArray &state, int version)
{
    bool b = QMainWindow::restoreState(state,version);
    QMapIterator<QAction*,ActionState*> it(m_actStateMap);
    while(it.hasNext()) {
        it.next();
        QMap<QString,InitToolSate>::iterator find = m_initIdStateMap.find(it.value()->id);
        if (find != m_initIdStateMap.end()) {
            it.key()->setChecked(find.value().checked);
        }
    }
    m_initIdStateMap.clear();
    m_hideSideAct->setChecked(m_areaToolBar.value(Qt::LeftDockWidgetArea)->toolBar->isHidden());
    return b;
}

bool ToolMainWindow::loadInitToolState(const QByteArray &state, int version)
{
    if (state.isEmpty())
        return false;

    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    int marker, v;
    stream >> marker;
    stream >> v;
    if (stream.status() != QDataStream::Ok || marker != VersionMarker || v != version)
        return false;

    QString id;
    InitToolSate value;
    int area;
    while(!stream.atEnd()) {
        stream >> id;
        stream >> area;
        value.area = (Qt::DockWidgetArea)area;
        stream >> value.split;
        stream >> value.checked;
        m_initIdStateMap.insert(id,value);
    }
    if (stream.status() != QDataStream::Ok) {
        m_initIdStateMap.clear();
        return false;
    }
    return true;

}
