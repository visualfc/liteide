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
#include "terminalapi/terminalapi.h"
#include "tabwidget/litetabwidget.h"
#include <QProcessEnvironment>

class QTabWidget;
class VTermWidget;
class QAction;
class QActionGroup;

struct Command
{
    QString     name;
    QString     path;
    QStringList args;
    QStringList loginArgs;
    QString     info;
};

struct TabInfoData
{
    QString cmd;   //command name
    QString dir;   //work dir
    QString title; // tab title
    QString pid; //process id
    bool    login; // --login
    bool    open;  // open terminal
};

inline QDataStream &operator<<(QDataStream &s, const TabInfoData &data)
{
    s << data.cmd;
    s << data.dir;
    s << data.title;
    s << data.login;
    return s;
}

inline QDataStream &operator>>(QDataStream &s, TabInfoData &data)
{
    s >> data.cmd;
    s >> data.dir;
    s >> data.title;
    s >> data.login;
    return s;
}

Q_DECLARE_METATYPE(TabInfoData)

class Terminal : public LiteApi::ITerminal
{
    Q_OBJECT
public:
    explicit Terminal(LiteApi::IApplication *app, QObject *parent);
    virtual ~Terminal();
    virtual void openDefaultTerminal(const QString &workDir);
    void openTerminal(int index, VTermWidget *term, const QString &cmdName, bool login, const QString &workdir);
    Command lookupCommand(const QString &name);
    QString makeTitle(const QString &baseName) const;
    QString getTabCurrentWorkDir(int index) const;
    bool eventFilter(QObject *obj, QEvent *e);
    void loadEnv(int index);
signals:

public slots:
    void newTerminal();
    void tabLoadEnv();
    void visibilityChanged(bool b);
    void termExited();
    void termStarted();
    void termTitleChanged(QString title);
    void tabCloseRequested(int index);
    void tabCurrentChanged(int index);
    void tabBarDoubleClicked(int index);
    void closeCurrenTab();
    void closeAllTab();
    void triggeredCmd(QAction* act);
    void toggledDarkMode(bool checked);
    void toggledLoginMode(bool checked);
    void applyOption(const QString &opt);
    void appLoaded();
    void aboutToShowListMenu();
    void triggeredListAction(QAction* act);
    void aboutToShowFolderContextMenu(QMenu* menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info, const QString &context);
    void fmctxOpenTerminal();
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    LiteTabWidget *m_tab;
    QMenu *m_filterMenu;
    QAction *m_toolWindowAct;
    QAction *m_newTabAct;
    QAction *m_closeTabAct;
    QAction *m_closeAllTabAct;
    QAction *m_loadEnvAct;
    QAction *m_darkModeAct;
    QAction *m_loginModeAct;
    QMenu   *m_listMenu;
    QActionGroup *m_listGroup;
    QList<Command> m_cmdList;
    QString  m_curName;
    QFileInfo m_fmctxFileInfo;
    //QAction *m_fmctxOpenTerminalAct;
    bool m_darkMode;
    bool m_loginMode;
    int m_indexId;
};

#endif // TERMINAL_H
