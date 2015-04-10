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
// Module: tooldockwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "tooldockwidget.h"
#include "liteapi/liteapi.h"
#include <QAction>
#include <QIcon>
#include <QLabel>
#include <QHBoxLayout>
#include <QVariant>
#include <QMenu>
#include <QToolButton>
#include <QDebug>
#include "memory.h"

BaseDockWidget::BaseDockWidget(QSize iconSize, QWidget *parent) :
    QDockWidget(parent), current(0)
{
    m_comboBox = new QComboBox;
    m_comboBox->setMinimumContentsLength(4);
    //m_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_toolBar = new QToolBar(this);
    m_toolBar->setContentsMargins(0, 0, 0, 0);
    m_toolBar->setIconSize(iconSize);
    //m_toolBar->setFixedHeight(24);
    m_toolBar->addWidget(m_comboBox);

    QWidget *spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_toolBar->addSeparator();
    m_spacerAct = m_toolBar->addWidget(spacer);

    m_closeAct = new QAction(tr("Hide"), m_toolBar);
    m_closeAct->setToolTip(tr("Hide Tool Window"));
    m_closeAct->setIcon(QIcon("icon:images/closetool.png"));
    m_toolBar->addAction(m_closeAct);
    connect(m_closeAct,SIGNAL(triggered()),this,SLOT(close()));
    connect(m_comboBox,SIGNAL(activated(int)),this,SLOT(activeComboBoxIndex(int)));

    this->setTitleBarWidget(m_toolBar);
/*
    m_toolBar->setStyleSheet("QToolBar {border: 1px ; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #eeeeee, stop: 1 #ababab); }"\
                             "QToolBar QToolButton { border:1px ; border-radius: 1px; }"\
                             "QToolBar QToolButton::hover { background-color: #ababab;}"\
                             "QToolBar::separator {width:2px; margin-left:2px; margin-right:2px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #dedede, stop: 1 #a0a0a0);}");
*/
}

void BaseDockWidget::setWindowTitle(const QString &text)
{
    QDockWidget::setWindowTitle(text);
}

QAction *BaseDockWidget::addWidget(QWidget *widget)
{
   return m_toolBar->insertWidget(m_closeAct,widget);
}

void BaseDockWidget::setToolMenu(QMenu *menu)
{
    QToolButton *btn = new QToolButton;
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setMenu(menu);
    btn->setStyleSheet("QToolButton::menu-indicator{image:none;}");

    m_toolBar->insertWidget(m_closeAct,btn);
}

void BaseDockWidget::setWidgetActions(QList<QAction*> actions)
{
    foreach(QAction *action, m_widgetActions) {
        m_toolBar->removeAction(action);
    }
    m_widgetActions = actions;
    m_spacerAct->setVisible(!m_widgetActions.isEmpty());
    foreach(QAction *action, m_widgetActions) {
        m_toolBar->insertAction(m_spacerAct,action);
        if (action->menu() != 0) {
            QWidget *w = m_toolBar->widgetForAction(action);
            QToolButton *btn = qobject_cast<QToolButton*>(w);
            if (btn) {
                btn->setPopupMode(QToolButton::InstantPopup);
                btn->setStyleSheet("QToolButton::menu-indicator{image:none;}");
            }
        }
    }
}

QList<QAction *> BaseDockWidget::actions() const
{
    return m_actions;
}

void BaseDockWidget::removeAction(QAction *action)
{
    if (m_actions.removeAll(action)) {
        if (action == current)
            current = 0;
        int index = m_comboBox->findData(action->objectName());
        if (index >= 0) {
            m_comboBox->removeItem(index);
        }
        QObject::disconnect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    }
}

QAction * BaseDockWidget::checkedAction () const
{
    return current;
}

void BaseDockWidget::actionChanged()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Q_ASSERT_X(action != 0, "ActionGroup::actionChanged", "internal error");
    if (action->isChecked()) {
        if (action != current) {
            if(current)
                current->setChecked(false);
            current = action;
            int index = m_comboBox->findData(action->objectName());
            if (index >= 0) {
                m_comboBox->setCurrentIndex(index);
            }
        }
    } else if (action == current) {
        current = 0;
    }
}

void BaseDockWidget::activeComboBoxIndex(int index)
{
    if (index < 0 || index >= m_comboBox->count()) {
        return;
    }
    QString objName = m_comboBox->itemData(index).toString();
    foreach(QAction *act, m_actions) {
        if (act->objectName() == objName) {
            if (!act->isChecked()) {
                act->setChecked(true);
            }
            break;
        }
    }
}

void BaseDockWidget::addAction(QAction *action, const QString &title)
{
    if(!m_actions.contains(action)) {
        m_actions.append(action);
        m_comboBox->addItem(title,action->objectName());
        QObject::connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    }
    if (current && current->isChecked()) {
        current->setChecked(false);
    }
    if (action->isChecked()) {
        current = action;
    }
}

SplitDockWidget::SplitDockWidget(QSize iconSize, QWidget *parent) :
    BaseDockWidget(iconSize,parent)
{
}

void SplitDockWidget::createMenu(Qt::DockWidgetArea area, bool split)
{
    QMenu *moveMenu = new QMenu(tr("Move To"),this);
    if (area != Qt::TopDockWidgetArea) {
        QAction *act = new QAction(tr("Top"),this);
        act->setData(Qt::TopDockWidgetArea);
        moveMenu->addAction(act);
        connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));
        QAction *act1 = new QAction(tr("Top (Split)"),this);
        act1->setData(Qt::TopDockWidgetArea);
        moveMenu->addAction(act1);
        connect(act1,SIGNAL(triggered()),this,SLOT(moveActionSplit()));
    }
    if (area != Qt::BottomDockWidgetArea) {
        QAction *act = new QAction(tr("Bottom"),this);
        act->setData(Qt::BottomDockWidgetArea);
        moveMenu->addAction(act);
        connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));
        QAction *act1 = new QAction(tr("Bottom (Split)"),this);
        act1->setData(Qt::BottomDockWidgetArea);
        moveMenu->addAction(act1);
        connect(act1,SIGNAL(triggered()),this,SLOT(moveActionSplit()));
    }
    if (area != Qt::LeftDockWidgetArea) {
        QAction *act = new QAction(tr("Left"),this);
        act->setData(Qt::LeftDockWidgetArea);
        moveMenu->addAction(act);
        connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));
        QAction *act1 = new QAction(tr("Left (Split)"),this);
        act1->setData(Qt::LeftDockWidgetArea);
        moveMenu->addAction(act1);
        connect(act1,SIGNAL(triggered()),this,SLOT(moveActionSplit()));
    }
    if (area != Qt::RightDockWidgetArea) {
        QAction *act = new QAction(tr("Right"),this);
        act->setData(Qt::RightDockWidgetArea);
        moveMenu->addAction(act);
        connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));
        QAction *act1 = new QAction(tr("Right (Split)"),this);
        act1->setData(Qt::RightDockWidgetArea);
        moveMenu->addAction(act1);
        connect(act1,SIGNAL(triggered()),this,SLOT(moveActionSplit()));
    }

    QMenu *menu = new QMenu(this);
    if (split) {
        QAction *unsplitAct = new QAction(tr("Unsplit"),this);
        unsplitAct->setData(area);
        connect(unsplitAct,SIGNAL(triggered()),this,SLOT(unsplitAction()));
        menu->addAction(unsplitAct);
    } else {
        QAction *splitAct = new QAction(tr("Split"),this);
        splitAct->setData(area);
        connect(splitAct,SIGNAL(triggered()),this,SLOT(splitAction()));
        menu->addAction(splitAct);
    }
    menu->addAction(moveMenu->menuAction());

    if (area == Qt::BottomDockWidgetArea || area == Qt::TopDockWidgetArea) {
        m_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        m_comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        QWidget *spacer = new QWidget;
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        m_toolBar->insertWidget(m_closeAct,spacer);
    }

    QToolButton *btn = new QToolButton(m_toolBar);
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setIcon(QIcon("icon:images/movemenu.png"));
    btn->setMenu(menu);
    btn->setText(tr("Move To"));
    btn->setToolTip(tr("Move To"));
    btn->setStyleSheet("QToolButton::menu-indicator {image: none;}");
    m_toolBar->insertWidget(m_closeAct,btn);
}

void SplitDockWidget::moveAction()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current,false);
}

void SplitDockWidget::moveActionSplit()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current,true);
}

void SplitDockWidget::splitAction()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current,true);
}

void SplitDockWidget::unsplitAction()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current,false);
}


OutputDockWidget::OutputDockWidget(QSize iconSize, QWidget *parent) :
    BaseDockWidget(iconSize,parent)
{
    m_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    QWidget *spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_toolBar->insertWidget(m_closeAct,spacer);
}

void OutputDockWidget::createMenu(Qt::DockWidgetArea /*area*/)
{
//    QMenu *moveMenu = new QMenu(tr("Move To"),this);
//    QAction *act = new QAction(tr("SideBar"),this);
//    act->setData(area);
//    moveMenu->addAction(act);
//    connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));

//    QMenu *menu = new QMenu(this);
//    menu->addAction(moveMenu->menuAction());

//    m_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
//    m_comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
//    QWidget *spacer = new QWidget;
//    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
//    m_toolBar->insertWidget(m_closeAct,spacer);

//    QToolButton *btn = new QToolButton(m_toolBar);
//    btn->setPopupMode(QToolButton::InstantPopup);
//    btn->setIcon(QIcon("icon:images/movemenu.png"));
//    btn->setMenu(menu);
//    btn->setText(tr("Move To"));
//    btn->setToolTip(tr("Move To"));
//    //btn->setStyleSheet("QToolButton::menu-indicator {image: none;}");
//    m_toolBar->insertWidget(m_closeAct,btn);
}

void OutputDockWidget::moveAction()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current);
}
