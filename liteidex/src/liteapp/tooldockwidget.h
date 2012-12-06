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
// Module: tooldockwidget.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-9-12
// $Id: tooldockwidget.h,v 1.0 2012-9-12 visualfc Exp $

#ifndef TOOLDOCKWIDGET_H
#define TOOLDOCKWIDGET_H

#include <QDockWidget>
#include <QToolBar>
#include <QComboBox>
#include <QPointer>
#include <QAction>

class QLabel;

class ToolDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ToolDockWidget(QWidget *parent = 0);
    QAction *addWidget(QWidget *widget);
    void setWindowTitle(const QString &text);
    QList<QAction *> actions() const;
    void addAction(QAction *act, const QString &title);
    void removeAction(QAction *act);
    QAction * checkedAction () const;
    void setToolMenu(QMenu *menu);
    void setWidgetActions(QList<QAction*> actions);
    void createMenu(Qt::DockWidgetArea area, bool split);
signals:
    void moveActionTo(Qt::DockWidgetArea, QAction*, bool);
protected slots:
    void moveAction();
    void splitAction();
    void unsplitAction();
    void moveActionSplit();
    void actionChanged();
    void activeComboBoxIndex(int);
protected:
    Qt::DockWidgetArea area;
    QToolBar *m_toolBar;
    QLabel   *m_titleLabel;
    QComboBox *m_comboBox;
    QAction  *m_spacerAct;
    QAction  *m_closeAct;
    QMap<QString,QAction*> m_idActionMap;
    QList<QAction*> m_widgetActions;
    QList<QAction*> m_actions;
    QPointer<QAction> current;
};

#endif // TOOLDOCKWIDGET_H
