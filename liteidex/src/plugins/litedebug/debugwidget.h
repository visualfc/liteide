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
// Module: debugwidget.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: debugwidget.h,v 1.0 2011-8-12 visualfc Exp $

#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include "liteapi/liteapi.h"
#include "litedebugapi/litedebugapi.h"
#include "textoutput/terminaledit.h"

class QTabWidget;
class QTreeView;
class QLineEdit;
class QPlainTextEdit;

class DebugWidget : public QObject
{
    Q_OBJECT
public:
    explicit DebugWidget(LiteApi::IApplication *app,QObject *parent = 0);
    virtual ~DebugWidget();
    virtual QWidget *widget();
public slots:
    void loadDebugInfo(const QString &id);
    void saveDebugInfo(const QString &id);
    void addWatch();
    void addLocalWatch();
    void removeWatch();
    void removeAllWatchAct();
    void watchViewContextMenu(QPoint);
    void setDebugger(LiteApi::IDebugger *debug);
    void enterText(const QString &cmd);
    void appendLog(const QString &log);
    void clearLog();
    void expandedVarsView(QModelIndex);
    void setExpand(LiteApi::DEBUG_MODEL_TYPE type, const QModelIndex &index, bool expanded);
    void watchCreated(QString,QString);
    void watchRemoved(QString);
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    LiteApi::IDebugger *m_debugger;
    QTabWidget *m_tabWidget;
    QTreeView *m_asyncView;
    QTreeView *m_varsView;
    QTreeView *m_watchView;
    QTreeView *m_statckView;
    QTreeView *m_libraryView;
    TerminalEdit *m_debugLogEdit;
    QMenu      *m_watchMenu;
    QAction    *m_addWatchAct;
    QAction    *m_addLocalWatchAct;
    QAction    *m_removeWatchAct;
    QAction     *m_removeAllWatchAct;
    QMap<QString,QString> m_watchMap;
};

#endif // DEBUGWIDGET_H
