/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: litetabwidget.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: litetabwidget.h,v 1.0 2012-11-30 visualfc Exp $

#ifndef LITETABWIDGET_H
#define LITETABWIDGET_H

#include <QWidget>
#include <QMap>
#include <QPointer>

class QMenu;
class QTabBar;
class QHBoxLayout;
class QStackedLayout;
class QStackedWidget;
class QToolButton;
class QToolBar;
class QAction;
class QActionGroup;

//header : headerWidget
//body: stackedWidget
class LiteTabWidget : public QObject
{
    Q_OBJECT
public:
    explicit LiteTabWidget(QObject *parent = 0);

    int addTab(QWidget *w,const QString & label, const QString &tip);
    int addTab(QWidget *w,const QIcon & icon, const QString & label,const QString &tip);
    void removeTab(int index);
    int indexOf(QWidget *w);
    QWidget *widget(int index);
    QWidget *currentWidget();
    QTabBar *tabBar();
    void setTabText(int index, const QString & text);
    QList<QWidget*> widgetList() const;
    QToolBar *headerToolBar();
    QWidget *stackedWidget();
signals:
    void currentChanged(int index);
    void tabCloseRequested(int index);
    void tabAddRequest();
public slots:
    void setCurrentWidget(QWidget *w);
    void setCurrentIndex(int index);
public slots:
    void closeCurrentTab();
    void selectListActGroup(QAction*);
    void tabCurrentChanged(int);
protected:
    QToolBar        *m_headerToolBar;
    QTabBar         *m_tabBar;
    QToolButton     *m_listButton;
    QStackedWidget  *m_stackedWidget;
    QList<QWidget*>  m_widgetList;
    QAction         *m_closeTabAct;
    QAction         *m_addTabAct;
    QMenu           *m_listActMenu;
    QActionGroup    *m_listActGroup;
    QPointer<QWidget>  m_currentWidget;
};

#endif // LITETABWIDGET_H
