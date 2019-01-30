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
// Module: tooldockwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TOOLDOCKWIDGET_H
#define TOOLDOCKWIDGET_H

#include <QDockWidget>
#include <QToolBar>
#include <QComboBox>
#include <QPointer>
#include <QAction>

class QLabel;

class BaseDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit BaseDockWidget(QSize iconSize, QWidget *parent = 0);
    virtual QAction *addWidget(QWidget *widget);
    virtual void setWindowTitle(const QString &text);
    virtual QList<QAction *> actions() const;
    virtual QAction * checkedAction() const;
    virtual void setToolMenu(QMenu *menu);
    virtual void setWidgetActions(QList<QAction*> actions);
    virtual void addAction(QAction *act, const QString &title);
    virtual void removeAction(QAction *act);
protected slots:
    virtual void actionChanged();
    virtual void activeComboBoxIndex(int);
protected:
    QToolBar *m_toolBar;
    //QLabel   *m_titleLabel;
    QComboBox *m_comboBox;
    QAction  *m_spacerAct;
    QAction  *m_closeAct;
    QMap<QString,QAction*> m_idActionMap;
    QList<QAction*> m_widgetActions;
    QList<QAction*> m_actions;
    QPointer<QAction> current;
};

class SplitDockWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit SplitDockWidget(QSize iconSize, QWidget *parent = 0);
    void createMenu(Qt::DockWidgetArea area, bool split);
signals:
    void moveActionTo(Qt::DockWidgetArea, Qt::DockWidgetArea, QAction*, bool);
protected slots:
    void moveAction();
    void splitAction();
    void unsplitAction();
    void moveActionSplit();
protected:
    Qt::DockWidgetArea m_area;
};

class OutputDockWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit OutputDockWidget(QSize iconSize, QWidget *parent = 0);
    void createMenu(Qt::DockWidgetArea area);
signals:
    void moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea, QAction*);
protected slots:
    void moveAction();
};

#endif // TOOLDOCKWIDGET_H
