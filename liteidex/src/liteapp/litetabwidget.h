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
// Module: litetabwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITETABWIDGET_H
#define LITETABWIDGET_H

#include <QWidget>
#include <QMap>
#include <QPointer>
#include <QTabBar>

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

class TabBar : public QTabBar
{
public:
    TabBar(QWidget *parent = 0) : QTabBar(parent)
    {
        m_enableWheel = true;
    }
    void wheelEvent(QWheelEvent *event)
    {
        if (!m_enableWheel) {
            return;
        }
        QTabBar::wheelEvent(event);
    }
    void setEnableWheel(bool b) {
        m_enableWheel = b;
    }
    bool enableWheel() const {
        return m_enableWheel;
    }
protected:
    bool m_enableWheel;
};

class LiteTabWidget : public QObject
{
    Q_OBJECT
public:
    explicit LiteTabWidget(QSize iconSize, QObject *parent = 0);
    virtual ~LiteTabWidget();
    int addTab(QWidget *w,const QString & label, const QString &tip);
    int addTab(QWidget *w,const QIcon & icon, const QString & label,const QString &tip);
    void removeTab(int index);
    int indexOf(QWidget *w);
    QWidget *widget(int index);
    QWidget *currentWidget();
    TabBar *tabBar();
    void setTabText(int index, const QString & text);
    QList<QWidget*> widgetList() const;
    QWidget *stackedWidget();
    QWidget *tabBarWidget();
    void setListMenu(QMenu *menu);
signals:
    void currentChanged(int index);
    void tabCloseRequested(int index);
    void tabAddRequest();
public slots:
    void setCurrentWidget(QWidget *w);
    void setCurrentIndex(int index);
    void tabMoved(int,int);
public slots:
    void closeCurrentTab();
    void tabCurrentChanged(int);
protected:
    QToolBar        *m_dumpToolBar;
    QWidget         *m_tabBarWidget;
    TabBar         *m_tabBar;
    QToolButton     *m_listButton;
    QToolButton     *m_closeButton;
    QStackedWidget  *m_stackedWidget;
    QList<QWidget*>  m_widgetList;
    QAction         *m_closeTabAct;
    QAction         *m_addTabAct;
    QPointer<QWidget>  m_currentWidget;
};

#endif // LITETABWIDGET_H
