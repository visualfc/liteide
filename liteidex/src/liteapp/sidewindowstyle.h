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
// Module: sidewindowstyle.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SIDEWINDOWSTYLE_H
#define SIDEWINDOWSTYLE_H

#include "windowstyle.h"
#include "tooldockwidget.h"

class SideDockWidget;
class OutputDockWidget;
class QToolButton;

struct SideActionState
{
    QWidget *toolBtn;
    QWidget *widget;
    QList<QAction*> widgetActions;
    QString id;
    QString title;
};

class SideDockWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit SideDockWidget(QSize iconSize, QWidget *parent = 0);
    void createMenu(Qt::DockWidgetArea area);
    void setCheckedAction(QAction *action);
    void setActions(const QMap<QAction*,SideActionState*> &m);
signals:
    void moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea, QAction*);
    void currenActionChanged(QAction *org, QAction *act);
protected slots:
    void moveAction();
    void actionChanged();
    virtual void activeComboBoxIndex(int);
protected:
    QMenu *m_menu;
    QMenu *m_moveMenu;
    Qt::DockWidgetArea m_area;
};

class BaseActionBar : public QObject
{
public:
    BaseActionBar(QObject *parent) : QObject(parent)
    {}
    virtual void addAction(QAction *action, QWidget *widget, const QString &id, const QString &title, QList<QAction*> widgetActions) = 0;
    virtual void removeAction(QAction *action) = 0;
    virtual QAction *findToolAction(QWidget *widget) const = 0;
    virtual QToolBar *toolBar() const = 0;
    virtual QMap<QAction*,SideActionState*> actionMap() const = 0;
};

class SideActionBar : public BaseActionBar
{
    Q_OBJECT
public:
    SideActionBar(QSize m_iconSize, QMainWindow *m_window, Qt::DockWidgetArea m_area = Qt::BottomDockWidgetArea);
    virtual ~SideActionBar();
    void addAction(QAction *action, QWidget *widget, const QString &id, const QString &title, QList<QAction*> widgetActions);
    void removeAction(QAction *action);
    QAction *findToolAction(QWidget *widget) const;
    virtual QToolBar *toolBar() const { return m_toolBar; }
    virtual QMap<QAction*,SideActionState*> actionMap() const { return m_actionStateMap; }
    void updateAction(QAction *action);
signals:
    void moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*);
protected slots:
    void setHideToolBar(bool b);
    void dockVisible(bool);
    void toggledAction(bool b);
    void currenActionChanged(QAction *org, QAction *act);
protected:
    QSize m_iconSize;
    QMainWindow *m_window;
    Qt::DockWidgetArea m_area;
    QToolBar *m_toolBar;
    //QAction  *spacerAct;
    QList<SideDockWidget*> m_dockList;
    QMap<QAction*,SideActionState*> m_actionStateMap;
    bool m_bHideToolBar;
};


class OutputActionBar : public BaseActionBar
{
    Q_OBJECT
public:
    OutputActionBar(QSize iconSize, QMainWindow *window, Qt::DockWidgetArea m_area = Qt::BottomDockWidgetArea);
    virtual ~OutputActionBar();
    OutputDockWidget *dockWidget() const;
    void addAction(QAction *action, QWidget *widget, const QString &id, const QString &title, QList<QAction*> widgetActions);
    void removeAction(QAction *action);
    void setHideToolBar(bool b);
    virtual QAction *findToolAction(QWidget *widget) const;
    virtual QToolBar *toolBar() const { return m_toolBar; }
    virtual QMap<QAction*,SideActionState*> actionMap() const { return m_actionStateMap; }
signals:
    void moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea,QAction*);
protected slots:
    void dockVisible(bool);
    void toggledAction(bool b);
protected:
    Qt::DockWidgetArea m_area;
    QToolBar *m_toolBar;
   // QAction  *spacerAct;
    OutputDockWidget *m_dock;
    QMap<QAction*,SideActionState*> m_actionStateMap;
    bool m_bHideToolBar;
};

class SideWindowStyle : public IWindowStyle
{
    Q_OBJECT
public:
    SideWindowStyle(LiteApi::IApplication *app, QMainWindow *window, QObject *parent = 0);
    ~SideWindowStyle();
    virtual void createToolWindowMenu();
    virtual QAction *addToolWindow(LiteApi::IApplication *app, Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false, QList<QAction*> widgetActions = QList<QAction*>());
    virtual void removeToolWindow(QAction *action);
    virtual QAction *findToolWindow(QWidget *widget);
    virtual void moveToolWindow(Qt::DockWidgetArea from, Qt::DockWidgetArea to, QAction *action, bool split);
    virtual void saveToolState() const;
    virtual void restoreToolsState();
    virtual void updateConer();
    void restoreHideToolWindows();
    void restoreHideSideToolWindows();
    void hideSideToolWindows();
public slots:
    void moveActionTo(Qt::DockWidgetArea from, Qt::DockWidgetArea to, QAction*action);
    virtual void hideOutputWindow();
    virtual void showOrHideToolWindow();
    virtual void hideAllToolWindows();
    void hideSideBar(bool);
    void toggledSideBar(bool);
protected:
    LiteApi::IApplication *m_liteApp;
    QMainWindow *m_mainWindow;
    SideActionBar *m_sideBar;
    OutputActionBar *m_outputBar;
    QMap<Qt::DockWidgetArea, BaseActionBar*> m_actionBarMap;
    QStatusBar  *m_statusBar;
    QAction     *m_hideSideAct;
    QList<QAction*> m_hideActionList;
    QList<QAction*> m_hideSideActionList;
    QMenu       *m_sideMenu;
    QMenu       *m_outputMenu;
    bool         m_useShortcuts;
};

#endif // SIDEWINDOWSTYLE_H
