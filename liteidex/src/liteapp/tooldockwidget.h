/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
#include <QLabel>
#include <QPointer>
#include <QAction>

class QVBoxLayout;
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
    virtual void setWidgetList(QList<QWidget*> widgets);
    virtual void addAction(QAction *act, const QString &title);
    virtual void removeAction(QAction *act);
    virtual void setWidget(QWidget *widget);
    virtual QWidget *widget() const;
    int currentIndex() const {
        return m_comboBox->currentIndex();
    }
protected slots:
    virtual void actionChanged();
    virtual void activeComboBoxIndex(int);
    virtual void topLevelChanged(bool);
    void  setFloatingWindow(bool b);
protected:
    QToolBar *m_toolBar;
    QWidget  *m_widget;
    QWidget  *m_mainWidget;
    QVBoxLayout *m_mainLayout;
    //QLabel   *m_titleLabel;
    QComboBox *m_comboBox;
    QAction *m_comboBoxAct;
    //QAction *m_titleLabelAct;
    QWidget  *m_spacer;
    QAction  *m_spacerAct;
    QAction  *m_closeAct;
    QAction  *m_floatAct;
    QMap<QString,QAction*> m_idActionMap;
    QList<QAction*> m_widgetActions;
    QList<QWidget*> m_widgetList;
    QList<QAction*> m_actions;
    QPointer<QAction> current;
};

class SplitDockWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit SplitDockWidget(QSize iconSize, QWidget *parent = 0);
    void createMenu(Qt::DockWidgetArea area, bool split);
    void setWindowTitle(const QString &text);
signals:
    void moveActionTo(Qt::DockWidgetArea, Qt::DockWidgetArea, QAction*, bool);
protected slots:
    void moveAction();
    void splitAction();
    void unsplitAction();
    void moveActionSplit();
protected:
    Qt::DockWidgetArea m_area;
    QString m_areaInfo;
};

class OutputDockWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit OutputDockWidget(QSize iconSize, QWidget *parent = 0);
    void createMenu(Qt::DockWidgetArea area);
    virtual void setWindowTitle(const QString &text);
    virtual void setWidget(QWidget *widget);
signals:
    void moveActionTo(Qt::DockWidgetArea,Qt::DockWidgetArea, QAction*);
protected slots:
    void moveAction();
};

#endif // TOOLDOCKWIDGET_H
