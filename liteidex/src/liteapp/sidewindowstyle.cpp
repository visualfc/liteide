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
// Module: sidewindowstyle.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "sidewindowstyle.h"
#include "tooldockwidget.h"
#include "rotationtoolbutton.h"
#include "liteapp_global.h"
#include "liteapi/liteids.h"
#include <QStatusBar>
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

SideDockWidget::SideDockWidget(QSize iconSize, QWidget *parent) :
    BaseDockWidget(iconSize,parent)
{
}

void SideDockWidget::createMenu(Qt::DockWidgetArea area)
{
    m_area = area;
    m_moveMenu = new QMenu(tr("Move To"),this);
    QAction *act = new QAction(tr("OutputBar"),this);
    act->setData(Qt::BottomDockWidgetArea);
    m_moveMenu->addAction(act);
    connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));

    m_menu = new QMenu(this);

    QToolButton *btn = new QToolButton(m_toolBar);
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setIcon(QIcon("icon:images/movemenu.png"));
    btn->setMenu(m_menu);
    btn->setText(tr("SideBar"));
    btn->setToolTip(tr("Show SideBar"));
    btn->setStyleSheet("QToolButton::menu-indicator {image: none;}");
    m_toolBar->insertWidget(m_closeAct,btn);
}

void SideDockWidget::moveAction()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(m_area,area,current);
}

void SideDockWidget::actionChanged()
{

}

void SideDockWidget::activeComboBoxIndex(int index)
{
    if (index < 0 || index >= m_comboBox->count()) {
        return;
    }
    QString objName = m_comboBox->itemData(index).toString();
    foreach(QAction *act, m_actions) {
        if (act->objectName() == objName) {
            QAction *org = current.data();
            current = act;
            emit currenActionChanged(org,act);
            break;
        }
    }
}

void SideDockWidget::setCheckedAction(QAction *action)
{
    current = action;
    for (int i = 0; i < m_comboBox->count(); i++) {
        if (m_comboBox->itemData(i).toString() == action->objectName()) {
            m_comboBox->setCurrentIndex(i);
            break;
        }
    }
}

void SideDockWidget::setActions(const QMap<QAction *, SideActionState *> &m)
{
    m_actions = m.keys();
    m_comboBox->clear();
    int cur = 0;
    int index = 0;
    m_menu->clear();
    QMapIterator<QAction *, SideActionState *> i(m);
    while(i.hasNext()) {
        i.next();
        QAction *act = i.key();
        m_comboBox->addItem(i.value()->title,act->objectName());
        m_menu->addAction(act);
        if (current && (current->objectName() == act->objectName())) {
            cur = index;
        }
        index++;
    }
    m_menu->addMenu(m_moveMenu);
    m_comboBox->setCurrentIndex(cur);
}

SideActionBar::SideActionBar(QSize _iconSize, QMainWindow *_window, Qt::DockWidgetArea _area)
    : BaseActionBar(_window), m_iconSize(_iconSize), m_window(_window),m_area(_area), m_bHideToolBar(false)
{
    m_toolBar = new QToolBar;
    m_toolBar->hide();
    m_toolBar->setObjectName(QString("side_tool_%1").arg(m_area));
    m_toolBar->setMovable(false);

//    QWidget *spacer = new QWidget;
//    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    spacerAct = toolBar->addWidget(spacer);
//    toolBar->addSeparator();
}

SideActionBar::~SideActionBar()
{
    qDeleteAll(m_actionStateMap);
}

void SideActionBar::addAction(QAction *action, QWidget *widget, const QString &id, const QString &title, QList<QAction *> widgetActions)
{
    RotationToolButton *btn = new RotationToolButton;
    btn->setDefaultAction(action);    
    if (m_area == Qt::LeftDockWidgetArea) {
        btn->setRotation(RotationToolButton::CounterClockwise);
    } else if (m_area == Qt::RightDockWidgetArea) {
        btn->setRotation(RotationToolButton::Clockwise);
    }

    SideDockWidget *dock = new SideDockWidget(m_iconSize, m_window);
    dock->setObjectName(QString("side_dock_%1").arg(id));
    dock->setWindowTitle(title);
    dock->setFeatures(QDockWidget::DockWidgetClosable);
    dock->hide();
    dock->createMenu(m_area);

    m_window->addDockWidget(m_area,dock);

    connect(dock,SIGNAL(visibilityChanged(bool)),this,SLOT(dockVisible(bool)));
    connect(dock,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*)),this,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*)));
    connect(dock,SIGNAL(currenActionChanged(QAction*,QAction*)),this,SLOT(currenActionChanged(QAction*,QAction*)));

    SideActionState *state = new SideActionState;
    state->toolBtn = btn;
    state->widget = widget;
    state->id = id;
    state->title = title;
    state->widgetActions = widgetActions;
    m_actionStateMap.insert(action,state);
    dock->setCheckedAction(action);
    //toolBar->insertWidget(spacerAct,btn);
    m_toolBar->addWidget(btn);
    if (m_toolBar->isHidden() && !m_bHideToolBar) {
        m_toolBar->show();
    }
    m_dockList.append(dock);
    connect(action,SIGNAL(toggled(bool)),this,SLOT(toggledAction(bool)));
    //update actions
    foreach(SideDockWidget *dock, m_dockList) {
        dock->setActions(m_actionStateMap);
    }
}

void SideActionBar::removeAction(QAction *action)
{
    for (int i = 0; i < m_dockList.size(); i++) {
        SideDockWidget *dock = m_dockList[i];
        if (dock->checkedAction() == action) {
            dock->hide();
            m_window->removeDockWidget(dock);
            m_dockList.removeAt(i);
            m_toolBar->removeAction(action);
            break;
        }
    }

    SideActionState *state = m_actionStateMap.value(action);
    if (state) {
        delete state->toolBtn;
    }
    m_actionStateMap.remove(action);
    delete state;

    foreach(SideDockWidget *dock, m_dockList) {
        dock->setActions(m_actionStateMap);
    }
}

void SideActionBar::setHideToolBar(bool b)
{
    m_bHideToolBar = b;
    if (m_bHideToolBar) {
        m_toolBar->hide();
    } else {
        m_toolBar->show();
    }
}

QAction *SideActionBar::findToolAction(QWidget *widget) const
{
    QMapIterator<QAction*,SideActionState*> i(m_actionStateMap);
    while (i.hasNext()) {
        i.next();
        if (i.value()->widget == widget) {
            return i.key();
        }
    }
    return 0;
}

void SideActionBar::dockVisible(bool b)
{
    SideDockWidget *dock = (SideDockWidget*)sender();
    QAction *action = dock->checkedAction();
    if (action) {
        action->setChecked(dock->isVisible());
    } else if (b && !dock->actions().isEmpty()) {
        dock->actions().first()->setChecked(true);
    }
}

void SideActionBar::updateAction(QAction *action)
{
    SideActionState *state = m_actionStateMap.value(action);
    foreach (SideDockWidget *dock, m_dockList) {
        if (dock->checkedAction() == action) {
            if (action->isChecked()) {
                if (dock->isHidden()) {
                    dock->show();
                }
                dock->setWidget(state->widget);
                dock->setWidgetActions(state->widgetActions);
                dock->setObjectName(QString("side_dock_%1").arg(state->id));
                dock->setWindowTitle(state->title);
            } else {
                dock->hide();
            }
            break;
        }
    }
}


void SideActionBar::toggledAction(bool)
{
    QAction *action = (QAction*)sender();
    updateAction(action);
}

void SideActionBar::currenActionChanged(QAction *org, QAction *act)
{
    if (org == act) {
        return;
    }
    SideDockWidget *curDock = (SideDockWidget*)sender();
    foreach (SideDockWidget *dock, m_dockList) {
        if ((dock->checkedAction() == act) && (dock != curDock)) {
            dock->setCheckedAction(org);
            org->setChecked(act->isChecked());
            updateAction(org);
            break;
        }
    }
    curDock->setCheckedAction(act);
    act->setChecked(true);
    updateAction(act);
}


OutputActionBar::OutputActionBar(QSize iconSize, QMainWindow *window, Qt::DockWidgetArea _area)
    : BaseActionBar(window), m_area(_area), m_bHideToolBar(false)
{
    m_toolBar = new QToolBar;
    m_toolBar->hide();
    m_toolBar->setObjectName(QString("side_tool_%1").arg(m_area));
    m_toolBar->setMovable(false);

//    QWidget *spacer = new QWidget;
//    spacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    spacerAct = toolBar->addWidget(spacer);
//    toolBar->addSeparator();

    m_dock = new OutputDockWidget(iconSize, window);
    m_dock->setObjectName(QString("side_dock_%1").arg(m_area));
    m_dock->setWindowTitle(QString("side_dock_%1").arg(m_area));
    m_dock->setFeatures(QDockWidget::DockWidgetClosable);
    m_dock->hide();
    m_dock->createMenu(m_area);

    window->addDockWidget(m_area,m_dock);

    connect(m_dock,SIGNAL(visibilityChanged(bool)),this,SLOT(dockVisible(bool)));
    connect(m_dock,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*)),this,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*)));
}

OutputActionBar::~OutputActionBar()
{
    qDeleteAll(m_actionStateMap);
}

OutputDockWidget *OutputActionBar::dockWidget() const
{
    return m_dock;
}

void OutputActionBar::addAction(QAction *action, QWidget *widget, const QString &id, const QString &title, QList<QAction *> widgetActions)
{
    RotationToolButton *btn = new RotationToolButton;
    btn->setDefaultAction(action);
    if (m_area == Qt::LeftDockWidgetArea) {
        btn->setRotation(RotationToolButton::CounterClockwise);
    } else if (m_area == Qt::RightDockWidgetArea) {
        btn->setRotation(RotationToolButton::Clockwise);
    }
    SideActionState *state = new SideActionState;
    state->toolBtn = btn;
    state->widget = widget;
    state->id = id;
    state->title = title;
    state->widgetActions = widgetActions;
    m_actionStateMap.insert(action,state);
    m_dock->addAction(action,title);
    //toolBar->insertWidget(spacerAct,btn);
    m_toolBar->addWidget(btn);
    if (m_toolBar->isHidden() && !m_bHideToolBar) {
        m_toolBar->show();
    }
    connect(action,SIGNAL(toggled(bool)),this,SLOT(toggledAction(bool)));
}

void OutputActionBar::removeAction(QAction *action)
{
    if (action->isChecked()) {
        action->setChecked(false);
    }
    SideActionState *state = m_actionStateMap.value(action);
    if (state) {
        delete state->toolBtn;
    }
    m_dock->removeAction(action);
    m_actionStateMap.remove(action);
    delete state;
    if (m_dock->actions().isEmpty()) {
        m_toolBar->hide();
    }
}

void OutputActionBar::setHideToolBar(bool b)
{
    m_bHideToolBar = b;
    if (m_bHideToolBar) {
        m_toolBar->hide();
    } else {
        if (!m_dock->actions().isEmpty()){
            m_toolBar->show();
        }
    }
}

QAction *OutputActionBar::findToolAction(QWidget *widget) const
{
    QMapIterator<QAction*,SideActionState*> i(m_actionStateMap);
    while (i.hasNext()) {
        i.next();
        if (i.value()->widget == widget) {
            return i.key();
        }
    }
    return 0;
}

void OutputActionBar::dockVisible(bool b)
{
    QAction *action = m_dock->checkedAction();
    if (action) {
        action->setChecked(m_dock->isVisible());
    } else if (b && !m_dock->actions().isEmpty()) {
        m_dock->actions().first()->setChecked(true);
    }
}

void OutputActionBar::toggledAction(bool)
{
    QAction *action = (QAction*)sender();
    SideActionState *state = m_actionStateMap.value(action);
    if (!state) {
        return;
    }
    if (action->isChecked()) {
        if (m_dock->isHidden()) {
            m_dock->show();
        }
        m_dock->setWidget(state->widget);
        m_dock->setWidgetActions(state->widgetActions);
        m_dock->setWindowTitle(state->title);
    } else {
        if (!m_dock->checkedAction()) {
            m_dock->hide();
        }
    }
}

SideWindowStyle::SideWindowStyle(LiteApi::IApplication *app, QMainWindow *window, QObject *parent)
    : IWindowStyle(parent),m_liteApp(app),m_mainWindow(window)
{
    QSize iconSize = LiteApi::getToolBarIconSize(app);
    m_sideBar = new SideActionBar(iconSize,window,Qt::LeftDockWidgetArea);
    m_outputBar = new OutputActionBar(iconSize,window,Qt::BottomDockWidgetArea);
    connect(m_sideBar,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*)),this,SLOT(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*)));
    connect(m_outputBar,SIGNAL(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*)),this,SLOT(moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*)));

    m_actionBarMap[Qt::LeftDockWidgetArea] = m_sideBar;
    m_actionBarMap[Qt::BottomDockWidgetArea] = m_outputBar;

    m_mainWindow->addToolBar(Qt::LeftToolBarArea,m_sideBar->toolBar());
    //m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea,m_sideBar->dock);

    m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea,m_outputBar->dockWidget());

    m_mainWindow->setDockNestingEnabled(true);
    m_mainWindow->setDockOptions(QMainWindow::AllowNestedDocks);

    m_statusBar = new QStatusBar;

    m_hideSideAct = new QAction(tr("Hide SideBar"),this);
    m_hideSideAct->setIcon(QIcon("icon:images/hidesidebar.png"));
    m_hideSideAct->setCheckable(true);

    QToolButton *btn = new QToolButton;
    btn->setDefaultAction(m_hideSideAct);
    btn->setStyleSheet("QToolButton {border:0}"
                       "QToolButton:checked {background : qlineargradient(spread:pad, x1:0, y1:1, x2:1, y2:0, stop:0 rgba(55, 57, 59, 255), stop:1 rgba(255, 255, 255, 255));}");
    m_statusBar->addWidget(btn);

    m_statusBar->setContentsMargins(0,0,0,0);

    m_statusBar->addWidget(m_outputBar->toolBar(),1);

    m_mainWindow->setStatusBar(m_statusBar);
    //m_mainWindow->addToolBar(Qt::BottomToolBarArea,m_outputBar->toolBar);

    m_sideMenu = 0;
    m_outputMenu = 0;

    connect(m_hideSideAct,SIGNAL(toggled(bool)),this,SLOT(hideSideBar(bool)));

    m_useShortcuts = m_liteApp->settings()->value(LITEAPP_TOOLWINDOW_SHORTCUTS,true).toBool();
}

SideWindowStyle::~SideWindowStyle()
{
    delete m_sideMenu;
    delete m_outputMenu;
    delete m_sideBar;
}

void SideWindowStyle::createToolWindowMenu()
{
    QMenu *menu = m_liteApp->actionManager()->loadMenu(ID_MENU_VIEW);
    if (menu) {
        menu->addAction(m_hideSideAct);
        m_sideMenu = menu->addMenu(tr("SideBar Windows"));
        m_outputMenu = menu->addMenu(tr("Output Windows"));
    }
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(m_liteApp,"App");
    actionContext->regAction(m_hideSideAct,"HideSideBar",LiteApi::UseMacShortcuts?"Ctrl+Alt+0":"Alt+0");
}

void SideWindowStyle::restoreHideToolWindows()
{
    foreach(QAction *action,m_hideActionList) {
        action->setChecked(true);
    }
    m_hideActionList.clear();
}

void SideWindowStyle::restoreHideSideToolWindows()
{
    foreach(QAction *action,m_hideSideActionList) {
        action->setChecked(true);
    }
    m_hideSideActionList.clear();
    m_sideBar->toolBar()->show();
}

void SideWindowStyle::hideSideToolWindows()
{
    m_hideSideActionList.clear();

    foreach(QAction *action, m_sideBar->actionMap().keys()) {
        if (action->isChecked()) {
            m_hideSideActionList.append(action);
            action->setChecked(false);
        }
    }
    m_sideBar->toolBar()->hide();
}

void SideWindowStyle::hideAllToolWindows()
{
    m_hideActionList.clear();

    foreach(QAction *action, m_sideBar->actionMap().keys()) {
        if (action->isChecked()) {
            m_hideActionList.append(action);
            action->setChecked(false);
        }
    }
    foreach(QAction *action, m_outputBar->actionMap().keys()) {
        if (action->isChecked()) {
            m_hideActionList.append(action);
            action->setChecked(false);
        }
    }
}

void SideWindowStyle::hideSideBar(bool b)
{
    if (b) {
        hideSideToolWindows();
    } else {
        restoreHideSideToolWindows();
    }
}

void SideWindowStyle::toggledSideBar(bool b)
{
    if (b) {
        m_hideSideAct->setChecked(false);
    }
}

void SideWindowStyle::showOrHideToolWindow()
{
    bool hide = false;
    foreach(QAction *action, m_sideBar->actionMap().keys()) {
        if (action->isChecked()) {
            hide = true;
            break;
        }
    }
    if (hide) {
        hideAllToolWindows();
    } else {
        restoreHideToolWindows();
    }
}

void SideWindowStyle::hideOutputWindow()
{
    foreach(QAction *act, m_outputBar->actionMap().keys()) {
        if (act->isChecked()) {
            act->setChecked(false);
        }
    }
}

void SideWindowStyle::saveToolState() const
{
    m_liteApp->settings()->setValue("side_side_hide",m_hideSideAct->isChecked());
}

void SideWindowStyle::restoreToolsState()
{
    m_hideSideAct->setChecked(m_liteApp->settings()->value("side_side_hide").toBool());
}

void SideWindowStyle::updateConer()
{
    m_mainWindow->setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
    m_mainWindow->setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);
}

void SideWindowStyle::moveToolWindow(Qt::DockWidgetArea from, Qt::DockWidgetArea to, QAction *action, bool /*split*/)
 {
    qDebug() << "move tool window" << from << to;
    if (from == to) {
        return;
    }
    BaseActionBar *fromBar = m_actionBarMap[from];
    BaseActionBar *toBar = m_actionBarMap[to];
    if (!fromBar || !toBar) {
        return;
    }
    SideActionState *state = fromBar->actionMap().value(action);
    toBar->addAction(action,state->widget,state->id,state->title,state->widgetActions);
    fromBar->removeAction(action);
}

void SideWindowStyle::moveActionTo(Qt::DockWidgetArea from, Qt::DockWidgetArea to, QAction *action)
{
    this->moveToolWindow(from,to,action,false);
}

QAction *SideWindowStyle::findToolWindow(QWidget *widget)
{
    QAction *act = m_sideBar->findToolAction(widget);
    if (act) {
        return act;
    }
    return m_outputBar->findToolAction(widget);
}

void SideWindowStyle::removeToolWindow(QAction */*action*/)
{

}

QAction *SideWindowStyle::addToolWindow(LiteApi::IApplication *app, Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool /*split*/, QList<QAction *> widgetActions)
{
    QAction *action = new QAction(this);
    action->setText(title);
    action->setCheckable(true);
    action->setObjectName(id);
    if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        m_outputBar->addAction(action,widget,id,title,widgetActions);
        int index = m_outputBar->actionMap().size();
        action->setText(title);
        if ((index <= 9) && m_useShortcuts) {
            action->setText(QString("%1: %2").arg(index).arg(title));
            //QKeySequence ks(LiteApi::UseMacShortcuts?QString("Ctrl+Alt+%1").arg(index):QString("Alt+%1").arg(index));
            QKeySequence ks(QString("Alt+%1").arg(index));
            LiteApi::IActionContext *actionContext = app->actionManager()->getActionContext(app,"App");
            actionContext->regAction(action,"ToolWindow_"+id,ks.toString());
        }
        if (m_outputMenu) {
            m_outputMenu->addAction(action);
        }
    } else {
        m_sideBar->addAction(action,widget,id,title,widgetActions);
        int index = m_sideBar->actionMap().size();
        action->setText(title);
        if ((index <= 9) && m_useShortcuts) {
            action->setText(QString("%1: %2").arg(index).arg(title));
            //QKeySequence ks(LiteApi::UseMacShortcuts?QString("Ctrl+Alt+%1").arg(index):QString("Ctrl+Alt+%1").arg(index));
            QKeySequence ks(QString("Ctrl+Alt+%1").arg(index));
            LiteApi::IActionContext *actionContext = app->actionManager()->getActionContext(app,"App");
            actionContext->regAction(action,"ToolWindow_"+id,ks.toString());
        }
        connect(action,SIGNAL(toggled(bool)),this,SLOT(toggledSideBar(bool)));
        if (m_sideMenu) {
            m_sideMenu->addAction(action);
        }
    }
    return action;
}
