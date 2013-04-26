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
// Module: toolmainwindow.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TOOLMAINWINDOW_H
#define TOOLMAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QAction>

class ActionGroup;
class QSplitter;
class RotationToolButton;
class ToolDockWidget;

class ActionGroup : public QObject
{
    Q_OBJECT
public:
    ActionGroup(QObject *parent);
    QList<QAction *> actions() const;
    void addAction(QAction *act);
    void removeAction(QAction *act);
    QAction * checkedAction () const;
protected slots:
    void actionChanged();
protected:
    QList<QAction *> m_actions;
    QPointer<QAction> current;
};

class ActionToolBar : public QObject
{
    Q_OBJECT
public:
    ActionToolBar(QWidget *parent, Qt::DockWidgetArea area);
    ToolDockWidget *dock(bool split) const;
    void addAction(QAction *action, const QString &title, bool split);
    void removeAction(QAction *action, bool split);
    void setHideToolBar(bool b);
signals:
    void moveActionTo(Qt::DockWidgetArea,QAction*,bool);
protected slots:
    void dock1Visible(bool);
    void dock2Visible(bool);
public:
    Qt::DockWidgetArea area;
    QToolBar *toolBar;
    QAction  *spacerAct;
    ToolDockWidget *dock1;
    ToolDockWidget *dock2;
    QMap<QAction*,QWidget*> m_actionWidgetMap;
    bool bHideToolBar;
};

struct ActionState
{
    QWidget *widget;
    QList<QAction*> widgetActions;
    Qt::DockWidgetArea area;
    bool    split;
    QString id;
    QString  title;
};

struct InitToolSate
{
    Qt::DockWidgetArea area;
    bool               split;
    bool               checked;
};

class ToolWindowBar : public QObject
{

};

class ToolMainWindow : public QMainWindow
{
    Q_OBJECT    
public:
    ToolMainWindow(QWidget *parent = 0);
    ~ToolMainWindow();
    QAction *addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false, QList<QAction*> widgetActions = QList<QAction*>());
    void removeToolWindow(QAction *action);
    QAction *findToolWindow(QWidget *wiget);
    QByteArray saveToolState(int version = 0) const;
    bool loadInitToolState(const QByteArray &state, int version = 0);
    bool restoreState(const QByteArray &state, int version = 0);
public slots:
    void hideToolWindow(Qt::DockWidgetArea area = Qt::BottomDockWidgetArea);
    void showOrHideToolWindow();
    void hideAllToolWindows();
    void restoreToolWindows();
    void hideSideBar(bool b);
    void moveToolWindow(Qt::DockWidgetArea area, QAction *action,bool split = false);    
protected slots:
    void toggledAction(bool);
protected:
    QMap<Qt::DockWidgetArea,ActionToolBar*> m_areaToolBar;
    QMap<QAction*,ActionState*> m_actStateMap;
    QMap<QString,InitToolSate> m_initIdStateMap;
    QList<QString> m_initCheckedList;
    QStatusBar  *m_statusBar;
    QAction     *m_hideSideAct;
    QList<QAction*> m_hideActionList;
};

#endif // TOOLMAINWINDOW_H
