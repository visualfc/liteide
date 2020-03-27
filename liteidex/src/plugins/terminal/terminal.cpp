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
// Module: terminal.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "terminal.h"
#include "vterm/vtermwidget.h"
#include "vterm/vtermcolor.h"
#include "liteenvapi/liteenvapi.h"
#include <QTabWidget>
#include <QFileInfo>
#include <QDir>
#include <QTime>
#include <QFontMetrics>

#define TERMINAL_CURCMD "terminal/curcmd"
#define TERMINAL_DARKMODE "terminal/darkmode"

#ifdef Q_OS_WIN
static QString checkFile(const QStringList &dirList, const QString &filePath)
{
    foreach (QString root, dirList) {
        QFileInfo info(root,filePath);
        if (info.exists()) {
            return QDir::toNativeSeparators(info.filePath());
        }
    }
    return QString();
}

static QString GetWindowGitBash()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList dirList;
    dirList << env.value("ProgramW6432","C:\\Program Files");
    dirList << env.value("ProgramFiles","C:\\Program Files");
    return checkFile(dirList,"Git\\bin\\bash.exe");
}


static QString GetWindowPowerShell()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList dirList;
    dirList << env.value("windir","C:\\Windows");
    return checkFile(dirList,"System32\\WindowsPowerShell\\v1.0\\powershell.exe");
}

static QString GetWindowsShell()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString windir = env.value("windir","C:\\Windows");
    QFileInfo info(windir,"System32\\cmd.exe");
    return QDir::toNativeSeparators(info.filePath());
}
#endif

Terminal::Terminal(LiteApi::IApplication *app, QObject *parent) : QObject(parent),
    m_liteApp(app), m_indexId(0)
{
    m_tab = new QTabWidget;
    m_tab->setDocumentMode(true);
    m_tab->setTabsClosable(true);
    m_tab->setUsesScrollButtons(true);

    m_newTabAct = new QAction("New",this);
    connect(m_newTabAct,SIGNAL(triggered()),this,SLOT(newTerminal()));
    m_closeTabAct = new QAction("Terminate",this);
    connect(m_closeTabAct,SIGNAL(triggered()),this,SLOT(closeCurrenTab()));


    QList<QAction*> actions;
    m_filterMenu = new QMenu(tr("Filter"));
    m_filterMenu->setIcon(QIcon("icon:images/filter.png"));
#ifdef Q_OS_WIN
    QString bash = GetWindowGitBash();
    QString powershell = GetWindowPowerShell();
    QString cmd = GetWindowsShell();
    m_cmdList.append(Command("cmd",cmd));
    if (!powershell.isEmpty()) {
        m_cmdList.append(Command("powershell",powershell));
    }
    if (!bash.isEmpty()) {
        m_cmdList.append(Command("bash",bash));
    }
#else
    m_cmdList.append(Command("bash","/bin/bash",QStringList() << "-i" << "-l"));
    m_cmdList.append(Command("bash(2)","/bin/bash"));
#endif
    m_curName = m_liteApp->settings()->value(TERMINAL_CURCMD,m_cmdList[0].name).toString();
    m_darkMode = m_liteApp->settings()->value(TERMINAL_DARKMODE,false).toBool();

    m_darkModeAct = new QAction(tr("Dark Mode"),this);
    m_darkModeAct->setCheckable(true);
    m_darkModeAct->setChecked(m_darkMode);

    connect(m_darkModeAct,SIGNAL(toggled(bool)),this,SLOT(toggledDarkMode(bool)));
    m_filterMenu->addAction(m_darkModeAct);

    if (m_cmdList.size() > 1) {
        QActionGroup *group = new QActionGroup(this);
        foreach (Command cmd, m_cmdList) {
            QString info = cmd.name+"\t"+cmd.path;
            if (!cmd.args.isEmpty()) {
                info += " "+cmd.args.join(" ");
            }
            QAction *act = new QAction(info,this);
            act->setData(cmd.name);
            act->setCheckable(true);
            act->setToolTip(cmd.path);
            if (m_curName == cmd.name) {
                act->setChecked(true);
            }
            group->addAction(act);
        }
        connect(group,SIGNAL(triggered(QAction*)),this,SLOT(triggeredCmd(QAction*)));
        m_filterMenu->addSeparator();
        m_filterMenu->addActions(group->actions());
        actions << m_filterMenu->menuAction();
    }

    actions << m_newTabAct << m_closeTabAct;
    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,m_tab,"Terminal",tr("Terminal"),true,actions);
    connect(m_toolWindowAct,SIGNAL(toggled(bool)),this,SLOT(visibilityChanged(bool)));
    connect(m_tab,SIGNAL(tabCloseRequested(int)),this,SLOT(tabCloseRequested(int)));
}


void Terminal::newTerminal()
{
    VTermWidget *term = new VTermWidget(m_tab);
    int index = m_tab->addTab(term,QString("%1 %2").arg(m_curName).arg(++m_indexId));
    m_tab->setCurrentIndex(index);
    term->setFocus();
    term->updateGeometry();
    term->setDarkMode(m_darkMode);

    QString dir;
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor && !editor->filePath().isEmpty()) {
        dir = QFileInfo(editor->filePath()).path();
    }
    if (dir.isEmpty()) {
        dir = QDir::homePath();
    }
    dir = QDir::toNativeSeparators(dir);
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    QString info = QString("%1: %2").arg(QTime::currentTime().toString("hh:mm:ss")).arg(dir);
    term->inputWrite(colored(info,TERM_COLOR_DEFAULT,TERM_COLOR_DEFAULT,TERM_ATTR_BOLD).toUtf8());
    term->inputWrite("\r\n");

    Command cmd = m_cmdList[0];
    if (m_cmdList.size() > 1) {
        foreach (Command c, m_cmdList) {
            if (m_curName == c.name) {
                cmd = c;
                break;
            }
        }
    }
    term->start(cmd.path,cmd.args,dir,env.toStringList());

    connect(term,SIGNAL(titleChanged(QString)),this,SLOT(termTitleChanged(QString)));
    connect(term,SIGNAL(exited()),this,SLOT(termExited()));
}

void Terminal::visibilityChanged(bool b)
{
    if (b && m_tab->count() == 0) {
        newTerminal();
    }
    QWidget *widget = m_tab->currentWidget();
    if (widget) {
        widget->setFocus();
    }
}

void Terminal::termExited()
{
    VTermWidget *widget = static_cast<VTermWidget*>(sender());
    int index = m_tab->indexOf(widget);
    if (index >= 0) {
        m_tab->removeTab(index);
    }
    widget->deleteLater();
}

void Terminal::termTitleChanged(QString title)
{
    VTermWidget *widget = static_cast<VTermWidget*>(sender());
    int index = m_tab->indexOf(widget);
    if (index >= 0) {
        m_tab->setTabToolTip(index,title);
    }
}

void Terminal::tabCloseRequested(int index)
{
    VTermWidget *widget = static_cast<VTermWidget*>(m_tab->widget(index));
    widget->deleteLater();
}

void Terminal::closeCurrenTab()
{
    int index = m_tab->currentIndex();
    if (index >= 0) {
        tabCloseRequested(index);
    }
}

void Terminal::triggeredCmd(QAction *act)
{
    m_curName = act->data().toString();
    m_liteApp->settings()->setValue(TERMINAL_CURCMD,m_curName);
}

void Terminal::toggledDarkMode(bool checked)
{
    m_darkMode = checked;
    m_liteApp->settings()->setValue(TERMINAL_DARKMODE,m_darkMode);
}
