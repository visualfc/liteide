/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2020 LiteIDE. All rights reserved.
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
// Module: terminal.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TERMINAL_H
#define TERMINAL_H

#include "liteapi/liteapi.h"
#include "tabwidget/litetabwidget.h"

class QTabWidget;
class VTermWidget;
class QAction;

struct Command
{
    QString     name;
    QString     path;
    QStringList args;
    QStringList loginArgs;
    QString     info;
};

class Terminal : public QObject
{
    Q_OBJECT
public:
    explicit Terminal(LiteApi::IApplication *app, QObject *parent);

signals:

public slots:
    void newTerminal();
    void visibilityChanged(bool b);
    void termExited();
    void termTitleChanged(QString title);
    void tabCloseRequested(int index);
    void closeCurrenTab();
    void triggeredCmd(QAction* act);
    void toggledDarkMode(bool checked);
    void toggledLoginMode(bool checked);
    void applyOption(const QString &opt);
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    LiteTabWidget *m_tab;
    QMenu *m_filterMenu;
    QAction *m_toolWindowAct;
    QAction *m_newTabAct;
    QAction *m_closeTabAct;
    QAction *m_darkModeAct;
    QAction *m_loginModeAct;
    QList<Command> m_cmdList;
    QString  m_curName;
    bool m_darkMode;
    bool m_loginMode;
    int m_indexId;
};

#endif // TERMINAL_H
