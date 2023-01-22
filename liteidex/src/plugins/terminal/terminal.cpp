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
#include "terminal_global.h"
#include "../liteapp/liteapp_global.h"
#include "vterm/vtermwidget.h"
#include "vterm/vtermcolor.h"
#include "liteenvapi/liteenvapi.h"
#include <QTabWidget>
#include <QFileInfo>
#include <QDir>
#include <QTime>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QInputDialog>
#include <QActionGroup>
#include <QTemporaryFile>
#include <QApplication>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

#ifdef Q_OS_WIN
static Command makeCommand(const QString &name, const QString &path, const QStringList &args = QStringList(), const QStringList &loginArgs = QStringList())
{
    Command cmd;
    cmd.name = name;
    cmd.path = path;
    cmd.args = args;
    cmd.loginArgs = loginArgs;
    return cmd;
}

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

static QStringList GetUnixShellList()
{
    QFile f("/etc/shells");
    if (!f.open(QFile::ReadOnly)) {
        return QStringList();
    }
    QStringList shells;
    QString ar = QString::fromUtf8(f.readAll());
    foreach (QString line, ar.split("\n")) {
        line = line.trimmed();
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }
        int pos = line.indexOf("#");
        if (pos > 0) {
            line = line.left(pos).trimmed();
        }
        shells << line;
    }
    return  shells;
}

Terminal::Terminal(LiteApi::IApplication *app, QObject *parent) : LiteApi::ITerminal(parent),
    m_liteApp(app), m_indexId(0)
{
    qRegisterMetaType<TabInfoData>("TabInfoData");
    qRegisterMetaTypeStreamOperators<TabInfoData>("TabInfoData");

    m_widget = new QWidget;
    m_tab = new LiteTabWidget(QSize(16,16));
    m_tab->tabBar()->setTabsClosable(true);
    m_tab->tabBar()->setElideMode(Qt::ElideNone);


    QVBoxLayout *layout = new QVBoxLayout(m_widget);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_tab->tabBarWidget());
    layout->addWidget(m_tab->stackedWidget());

    m_newTabAct = new QAction(tr("New"),this);
    m_newTabAct->setToolTip(tr("Open a new terminal"));
    connect(m_newTabAct,SIGNAL(triggered()),this,SLOT(newTerminal()));
    m_closeTabAct = new QAction(tr("Close"),this);
    m_closeTabAct->setToolTip(tr("Close current terminal"));
    connect(m_closeTabAct,SIGNAL(triggered()),this,SLOT(closeCurrenTab()));
    m_closeAllTabAct = new QAction(tr("CloseAll"),this);
    m_closeAllTabAct->setToolTip(tr("Close all terminal"));
    connect(m_closeAllTabAct,SIGNAL(triggered()),this,SLOT(closeAllTab()));
#ifndef Q_OS_WIN
    m_loadEnvAct = new QAction(tr("LoadEnv"),this);
    m_loadEnvAct->setToolTip(tr("Current terminal load environment from LiteIDE"));
    connect(m_loadEnvAct,SIGNAL(triggered()),this,SLOT(tabLoadEnv()));
#endif

    QList<QAction*> actions;
    m_filterMenu = new QMenu(tr("Filter"));
    m_filterMenu->setIcon(QIcon("icon:images/filter.png"));
#ifdef Q_OS_WIN
    QString bash = GetWindowGitBash();
    QString powershell = GetWindowPowerShell();
    QString shell = GetWindowsShell();
    m_cmdList.append(makeCommand("cmd",shell));
    if (!powershell.isEmpty()) {
        m_cmdList.append(makeCommand("powershell",powershell));
    }
    if (!bash.isEmpty()) {
        m_cmdList.append(makeCommand("bash",bash));
    }
#else
    QStringList shellList = GetUnixShellList();
    shellList.prepend("/bin/bash");
    shellList.removeDuplicates();
    foreach (QString shell, shellList) {
        QFileInfo info(shell);
        if (!info.exists()) {
            continue;
        }
        Command cmd;
        cmd.name = info.fileName();
        cmd.path = info.filePath();
        cmd.loginArgs << "-l";
        m_cmdList << cmd;
    }
#endif
    m_curName = m_liteApp->settings()->value(TERMINAL_CURCMD,m_cmdList[0].name).toString();
    m_darkMode = m_liteApp->settings()->value(TERMINAL_DARKMODE,false).toBool();
    m_loginMode = m_liteApp->settings()->value(TERMINAL_LOGINSHELL,false).toBool();
#ifdef Q_OS_WIN
    m_loginMode = false;
#endif

    m_darkModeAct = new QAction(tr("Dark Mode"),this);
    m_darkModeAct->setCheckable(true);
    m_darkModeAct->setChecked(m_darkMode);


    m_newTabAct->setText(tr("New")+" ["+m_curName+"]");

    connect(m_darkModeAct,SIGNAL(toggled(bool)),this,SLOT(toggledDarkMode(bool)));

    m_filterMenu->addAction(m_darkModeAct);
#ifndef Q_OS_WIN
    m_loginModeAct = new QAction(tr("Login Mode (shell --login)"),this);
    m_loginModeAct->setCheckable(true);
    m_loginModeAct->setChecked(m_loginMode);
    m_filterMenu->addAction(m_loginModeAct);
    connect(m_loginModeAct,SIGNAL(toggled(bool)),this,SLOT(toggledLoginMode(bool)));
#endif

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

    actions << m_newTabAct << m_closeTabAct << m_closeAllTabAct;
#ifndef Q_OS_WIN
    actions << m_loadEnvAct;
#endif

    m_listMenu = new QMenu;
    m_listGroup = new QActionGroup(this);
    m_tab->setListMenu(m_listMenu);
    connect(m_listMenu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowListMenu()));
    connect(m_listGroup,SIGNAL(triggered(QAction*)),this,SLOT(triggeredListAction(QAction*)));


    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,m_widget,"Terminal",tr("Terminal"),true,actions);
    connect(m_toolWindowAct,SIGNAL(toggled(bool)),this,SLOT(visibilityChanged(bool)));
    connect(m_tab,SIGNAL(tabCloseRequested(int)),this,SLOT(tabCloseRequested(int)));
    connect(m_tab,SIGNAL(currentChanged(int)),this,SLOT(tabCurrentChanged(int)));

    applyOption(OPTION_TERMIANL);

    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));
#if QT_VERSION >= 0x050000
//    connect(m_tab->tabBar(),SIGNAL(tabBarDoubleClicked(int)),this,SLOT(tabBarDoubleClicked(int)));
#endif
    applyOption(OPTION_LITEAPP);

    m_liteApp->extension()->addObject("LiteApi.ITerminal",this);

    //m_fmctxOpenTerminalAct = new QAction(tr("Open in Integrated Terminal"),this);
    //connect(m_fmctxOpenTerminalAct,SIGNAL(triggered()),this,SLOT(fmctxOpenTerminal()));
    //connect(m_liteApp->fileManager(),SIGNAL(aboutToShowFolderContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo,QString)),this,SLOT(aboutToShowFolderContextMenu(QMenu*,LiteApi::FILESYSTEM_CONTEXT_FLAG,QFileInfo,QString)));

    qApp->installEventFilter(this);

}

#ifdef Q_OS_MAC
static QMap<QString,QString> getProcessWorkDirList(const QStringList &pids)
{
    QString cmd = QString("lsof -a -p %1 -d cwd -Fn").arg(pids.join(","));
    QMap<QString,QString> kv;
    QProcess p;
    p.start(cmd);
    if (!p.waitForStarted(1000)) {
        return kv;
    }
    if (!p.waitForFinished(3000)) {
        p.kill();
        return kv;
    }
    QByteArray ar = p.readAllStandardOutput();
    QStringList lines = QString::fromUtf8(ar).split("\n",qtSkipEmptyParts);
//    p4153
//    fcwd
//    n/Users/vfc
    int index = 0;
    foreach (QString line, lines) {
        if (index %3 == 2) {
            QString pid = lines[index-2];
            if (line.startsWith("n") && pid.startsWith("p")) {
                kv[pid.mid(1)] = line.mid(1);
            }
        }
        index++;
    }
    return kv;
}
#elif defined(Q_OS_WIN)
static QMap<QString,QString> getProcessWorkDirList(const QStringList &pids)
{
    QMap<QString,QString> kv;
    return kv;
}
#else
static QMap<QString,QString> getProcessWorkDirList(const QStringList &pids)
{
    QMap<QString,QString> kv;
    if (QDir("/proc").exists()) {
        foreach (QString pid, pids) {
            QFileInfo info("/proc/"+pid+"/cwd");
            if (info.isSymLink()) {
                kv[pid] = info.symLinkTarget();
            }
        }
    } else {
//        pwdx 9194 9947
//        9194: /home/my
//        9947: /home/my
        QString cmd = QString("pwdx %1").arg(pids.join(" "));
        QProcess p;
        p.start(cmd);
        if (!p.waitForStarted(1000)) {
            return kv;
        }
        if (!p.waitForFinished(3000)) {
            p.kill();
            return kv;
        }
        QByteArray ar = p.readAllStandardOutput();
        QStringList lines = QString::fromUtf8(ar).split("\n",qtSkipEmptyParts);
        foreach (QString line, lines) {
            QStringList ar = line.split(":",qtSkipEmptyParts);
            QString path = ar[1].trimmed();
            if (ar.size() == 2 && path.startsWith("/")) {
                kv[ar[0].trimmed()] = path;
            }
        }
    }
    return kv;
}
#endif


Terminal::~Terminal()
{
    m_liteApp->settings()->beginGroup("terminal/tabs");
    m_liteApp->settings()->remove("");
    QStringList pids;
    for (int i = 0; i < m_tab->count(); i++) {
        TabInfoData data = m_tab->tabData(i).value<TabInfoData>();
        if (!data.pid.isEmpty()) {
            pids << data.pid;
        }
    }
    // check pid
    QMap<QString,QString> kv = getProcessWorkDirList(pids);
    for (int i = 0; i < m_tab->count(); i++) {
        QString key = QString("%1").arg(i);
        TabInfoData data = m_tab->tabData(i).value<TabInfoData>();
        data.title = m_tab->tabBar()->tabText(i);
        if (!data.pid.isEmpty()) {
            QString dir = kv[data.pid];
            if (!dir.isEmpty()) {
                data.dir = dir;
                data.title = makeTitle(QFileInfo(dir).fileName());
            }
        }
        m_liteApp->settings()->setValue(key,QVariant::fromValue(data));
    }

    m_liteApp->settings()->endGroup();

    closeAllTab();

    m_listMenu->clear();
    delete m_listMenu;
    delete m_filterMenu;
    qDeleteAll(m_listGroup->actions());
    delete m_listGroup;
    delete m_tab;
    delete m_widget;
}

void Terminal::openDefaultTerminal(const QString &workDir)
{
    m_toolWindowAct->setChecked(true);

    QString dir = QDir::cleanPath(QDir::fromNativeSeparators(workDir));

    for (int i = 0; i < m_tab->count(); i++) {
        TabInfoData data = m_tab->tabData(i).value<TabInfoData>();
        QString pwd = getTabCurrentWorkDir(i); // check current work dir is same
        if (data.dir == dir && pwd == dir && data.cmd == m_curName) {
            if (!data.open) {
                m_tab->setCurrentIndex(i);
                return;
            } else {
                VTermWidget *widget = (VTermWidget*)m_tab->widget(i);
                if (widget && widget->process()->processList() == 1) {
                    m_tab->setCurrentIndex(i);
                    widget->setFocus();
                    return;
                }
            }
        }
    }


    QString cmdName = m_curName;
    //QString title = QString("%1 %2").arg(m_curName).arg(++m_indexId);
    dir = QDir::toNativeSeparators(workDir);
    //openNewTerminal(cmdName,m_loginMode,title,dir,env);
    VTermWidget *term = new VTermWidget(m_liteApp, m_terminalFont, m_widget);

    QString title = makeTitle(QFileInfo(dir).fileName());
    int index = m_tab->addTab(term,title,dir);
    m_tab->setCurrentIndex(index);
    openTerminal(index,term,cmdName,m_loginMode,dir);
}

void Terminal::appLoaded()
{
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    m_liteApp->settings()->beginGroup("terminal/tabs");
    foreach(QString key,m_liteApp->settings()->childKeys()) {
        TabInfoData data = m_liteApp->settings()->value(key).value<TabInfoData>();
        if (!data.cmd.isEmpty() && !data.title.isEmpty()) {
            VTermWidget *widget = new VTermWidget(m_liteApp,m_terminalFont, m_widget);
            int index = m_tab->addTab(widget,data.title,QDir::toNativeSeparators(data.dir));
            data.open = false;
            m_tab->setTabData(index,QVariant::fromValue(data));
        }
    }
    m_liteApp->settings()->endGroup();
    m_indexId = m_tab->count();
    if (m_tab->count() >= 1) {
        m_tab->setCurrentIndex(m_tab->count()-1);
    }
}

void Terminal::aboutToShowListMenu()
{
    m_listMenu->clear();
    QList<QAction*> actions = m_listGroup->actions();
    qDeleteAll(actions);

    for (int i = 0; i < m_tab->count(); i++) {
        TabInfoData data = m_tab->tabData(i).value<TabInfoData>();
        QString info;
        if (data.login) {
            info = QString("%1\t[%2 --login]").arg(m_tab->tabText(i),data.cmd);
        } else {
            info = QString("%1\t[%2]").arg(m_tab->tabText(i),data.cmd);
        }
        QAction *act = new QAction(info,m_listGroup);
        act->setData(i);
        act->setCheckable(true);
        m_listGroup->addAction(act);
        if (m_tab->currentIndex() == i) {
            act->setChecked(true);
        }
    }
    m_listMenu->addActions(m_listGroup->actions());
}

void Terminal::triggeredListAction(QAction *act)
{
    int index = act->data().toInt();
    m_tab->setCurrentIndex(index);
}

void Terminal::aboutToShowFolderContextMenu(QMenu *menu, LiteApi::FILESYSTEM_CONTEXT_FLAG flag, const QFileInfo &info,const QString &context)
{
//    menu->addSeparator();
//    menu->addAction(m_fmctxOpenTerminalAct);
//    m_fmctxFileInfo = info;
}

void Terminal::fmctxOpenTerminal()
{
    QString dir;
    if (m_fmctxFileInfo.isDir()) {
        dir = m_fmctxFileInfo.filePath();
    } else {
        dir = m_fmctxFileInfo.path();
    }
    openDefaultTerminal(dir);
}

void Terminal::updateFont()
{
    QString fontFamily = m_liteApp->settings()->value(TERMINAL_FAMILY,TERMINAL_FAMILY_DEFAULT).toString();
    int fontSize = m_liteApp->settings()->value(TERMINAL_FONTSIZE,12).toInt();
    int fontZoom = m_liteApp->settings()->value(TERMINAL_FONTZOOM,100).toInt();
    bool antialias = m_liteApp->settings()->value(TERMINAL_ANTIALIAS,true).toBool();


    if (!QFontDatabase().hasFamily(fontFamily)) {
#if QT_VERSION >= 0x050200
        fontFamily = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
#else
        fontFamily = QApplication::font().family();
#endif
    }
    m_terminalFont.setFamily(fontFamily);
    m_terminalFont.setPointSize(fontSize*fontZoom/100.0);
    if (antialias) {
        m_terminalFont.setStyleStrategy(QFont::PreferAntialias);
    } else {
        m_terminalFont.setStyleStrategy(QFont::NoAntialias);
    }
}


Command Terminal::lookupCommand(const QString &name)
{
    foreach(Command cmd, m_cmdList) {
        if (cmd.name == name) {
            return  cmd;
        }
    }
    return m_cmdList[0];
}

void Terminal::openTerminal(int index, VTermWidget *term, const QString &cmdName, bool login, const QString &workdir)
{
    Command cmd = lookupCommand(cmdName);

    term->setFocus();
    term->updateGeometry();
    term->setDarkMode(m_darkMode);

    connect(term,SIGNAL(titleChanged(QString)),this,SLOT(termTitleChanged(QString)));
    connect(term,SIGNAL(exited()),this,SLOT(termExited()));
    connect(term,SIGNAL(started()),this,SLOT(termStarted()));

    // check valid or home
    QString dir;
    if (QDir(workdir).exists()) {
        dir = workdir;
    } else {
        dir = QDir::homePath();
    }
    dir = QDir::cleanPath(dir);

    QString info;
    QString attr;
    if (!cmd.loginArgs.isEmpty()) {
        if (login) {
            attr = "login shell";
        } else {
            attr = "non-login shell";
        }
    } else {
        login = false;
        attr = "open shell";
    }
    info = QString("%1: %2 [%3] in %4").arg(QTime::currentTime().toString("hh:mm:ss")).arg(attr).arg(cmd.path).arg(dir);

    term->inputWrite(term_color(info,TERM_COLOR_DEFAULT,TERM_COLOR_DEFAULT,TERM_ATTR_BOLD).toUtf8());
    term->inputWrite("\r\n");
    if (login) {
        term->inputWrite(term_color("Warning, the Login Shell Go environment may be different from LiteIDE.",TERM_COLOR_RED,TERM_COLOR_DEFAULT,TERM_ATTR_BOLD).toUtf8());
        term->inputWrite("\r\n");
        term->inputWrite(term_color("Please use LoadEnv action to load environment from LiteIDE if need.",TERM_COLOR_RED,TERM_COLOR_DEFAULT,TERM_ATTR_BOLD).toUtf8());
        term->inputWrite("\r\n");
    }

    QStringList args = cmd.args;
    QProcessEnvironment env;
    if (login) {
        args.append(cmd.loginArgs);
        env = QProcessEnvironment::systemEnvironment();
    } else {
        env = LiteApi::getGoEnvironment(m_liteApp);
    }

    TabInfoData data;
    data.cmd = cmdName;
    data.dir = QDir::fromNativeSeparators(dir);
    data.login = login;

    m_tab->setTabData(index,QVariant::fromValue(data));

    term->start(cmd.path,args,dir,env.toStringList());

    data.open = true;
    data.pid = QString("%1").arg(term->process()->pid());

    m_tab->setTabData(index,QVariant::fromValue(data));
}

QString Terminal::makeTitle(const QString &baseName) const
{
    bool rename = false;
    int index = 0;
    for (int i = 0; i < m_tab->count(); i++) {
        QString name = m_tab->tabText(i);
        if (name == baseName) {
            rename = true;
            index++;
        }
        if (name.startsWith(baseName+" (")) {
            index++;
        }
    }
    if (!rename) {
        return baseName;
    }
    return QString("%1 (%2)").arg(baseName).arg(index);
}

QString Terminal::getTabCurrentWorkDir(int index) const
{
    TabInfoData data = m_tab->tabData(index).value<TabInfoData>();
    if (!data.open) {
        return data.dir;
    }
    VTermWidget *widget = static_cast<VTermWidget*>(m_tab->widget(index));
    QString pid = QString("%1").arg(widget->process()->pid());
    QMap<QString,QString> kv = getProcessWorkDirList(QStringList() << pid);
    QString dir = kv[pid];
    if (!dir.isEmpty()) {
        return dir;
    }
    return data.dir;
}

bool Terminal::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type()) {
    case QEvent::ShortcutOverride:
        if (qApp->focusWidget() == m_tab->currentWidget() && static_cast<QKeyEvent*>(e)->key() == Qt::Key_Escape) {
            e->accept();
        }
        break;
    }
    return QObject::eventFilter(obj,e);
}

void Terminal::newTerminal()
{
    QString cmdName = m_curName;
    //QString title = QString("%1 %2").arg(m_curName).arg(++m_indexId);
    QString dir;
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor && !editor->filePath().isEmpty()) {
        dir = QFileInfo(editor->filePath()).path();
    }
    if (dir.isEmpty()) {
        dir = QDir::homePath();
    }
    dir = QDir::toNativeSeparators(dir);
    QString title = makeTitle(QFileInfo(dir).fileName());
    //openNewTerminal(cmdName,m_loginMode,title,dir,env);
    VTermWidget *term = new VTermWidget(m_liteApp, m_terminalFont, m_widget);
    int index = m_tab->addTab(term,title,QDir::toNativeSeparators(dir));

    m_tab->setCurrentIndex(index);
    openTerminal(index,term,cmdName,m_loginMode,dir);
}

void Terminal::tabLoadEnv()
{
    int index = m_tab->currentIndex();
    if (index >= 0) {
        this->loadEnv(index);
    }
}

void Terminal::visibilityChanged(bool b)
{
    if (!b) {
        return;
    }
    if (m_tab->count() == 0) {
        newTerminal();
    }
    QWidget *widget = m_tab->currentWidget();
    if (widget) {
        widget->setFocus();
    }
    //deploy load
    tabCurrentChanged(m_tab->currentIndex());
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

void Terminal::termStarted()
{
//    VTermWidget *widget = static_cast<VTermWidget*>(sender());
//    int index = m_tab->indexOf(widget);
//    if (index >= 0) {
//        this->loadEnv(index);
//    }
}

void Terminal::loadEnv(int index)
{
    VTermWidget *widget = static_cast<VTermWidget*>(m_tab->widget(index));
    if (widget->process()->processList() > 1) {
        return;
    }
    TabInfoData data = m_tab->tabData(index).value<TabInfoData>();
    QTemporaryFile file;
    file.setAutoRemove(false);
    if (file.open()) {
        QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
        QStringList list;
        foreach (QString key, env.keys()) {
            if (key.contains("(")) {
                continue;
            }
            list << QString("export %1=\"%2\"").arg(key).arg(env.value(key));
        }
        file.write("#!/bin/sh\n");
        file.write("echo "+term_bold("Load environment form LiteIDE.").toUtf8());
        file.write("\n");
        file.write(list.join("\n").toUtf8());
        file.write("\n");
        file.write("rm "+file.fileName().toUtf8());
        file.write("\n");
        file.close();
    }
    file.setPermissions(file.permissions() | QFile::ExeUser | QFile::ExeOwner);
    widget->process()->write("source "+file.fileName().toUtf8()+"\n");
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
    m_tab->removeTab(index);
    if (widget) {
        this->disconnect(widget,0,this,0);
        widget->deleteLater();
    }
}

void Terminal::tabCurrentChanged(int index)
{
    //deploy load
    if (!m_widget->isVisible()) {
        return;
    }
    TabInfoData data = m_tab->tabData(index).value<TabInfoData>();
    if (data.cmd.isEmpty()) {
        return;
    }
    if (data.open) {
        VTermWidget *term = static_cast<VTermWidget*>(m_tab->widget(index));
        term->setFocus();
        return;
    }
    data.open = true;
    m_tab->setTabData(index,QVariant::fromValue(data));
    VTermWidget *term = static_cast<VTermWidget*>(m_tab->widget(index));
    openTerminal(index,term,data.cmd,data.login,data.dir);
}

void Terminal::tabBarDoubleClicked(int index)
{
    QString text = m_tab->tabText(index);
    bool ok = false;
    text = QInputDialog::getText(m_widget,tr("Terminal"),tr("Rename Tab Title"),QLineEdit::Normal,text,&ok);
    if (ok && !text.isEmpty()) {
        m_tab->setTabText(index,text);
        TabInfoData data = m_tab->tabData(index).value<TabInfoData>();
        data.title = text;
        m_tab->setTabData(index,QVariant::fromValue(data));
    }
}

void Terminal::closeCurrenTab()
{
    int index = m_tab->currentIndex();
    if (index >= 0) {
        tabCloseRequested(index);
    }
}

void Terminal::closeAllTab()
{
    for (int i = m_tab->count()-1; i >= 0; i--) {
        VTermWidget *widget = static_cast<VTermWidget*>(m_tab->widget(i));
        m_tab->removeTab(i);
        if (widget) {
            disconnect(widget,0,this,0);
            widget->deleteLater();
        }
    }
}

void Terminal::triggeredCmd(QAction *act)
{
    m_curName = act->data().toString();
    m_liteApp->settings()->setValue(TERMINAL_CURCMD,m_curName);
    m_newTabAct->setText(tr("New")+ " ["+m_curName+"]");
}

void Terminal::toggledDarkMode(bool checked)
{
    m_darkMode = checked;
    m_liteApp->settings()->setValue(TERMINAL_DARKMODE,m_darkMode);
}

void Terminal::toggledLoginMode(bool checked)
{
    m_loginMode = checked;
    m_liteApp->settings()->setValue(TERMINAL_LOGINSHELL,m_loginMode);
}

void Terminal::applyOption(const QString &opt)
{
    if (opt == OPTION_TERMIANL) {
        updateFont();
    } else if (opt == OPTION_LITEAPP) {
#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050900
    QString qss = m_liteApp->settings()->value(LITEAPP_QSS,"default.qss").toString();
    if (qss == "default.qss") {
        m_tab->tabBar()->setStyleSheet(
                    "QTabBar::tab {"
                    "border: 1px solid #C4C4C3;"
                    "border-bottom-color: #C2C7CB; /* same as the pane color */"
                    "min-width: 8ex;"
                    "padding: 4px 2px 4px 2px;"
                    "}"
                    "QTabBar::close-button:hover,QTabBar::close-button:selected {"
                    "margin: 0px;"
                    "image: url(:/images/close.png);"
                    "subcontrol-position: left;"
                    "padding: 1px;"
                    "}"
                    "QTabBar::tab:selected, QTabBar::tab:hover {"
                    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 0 #fafafa, stop: 0.4 #f4f4f4,"
                    "stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
                    "}"
                    "QTabBar::tab:selected {"
                    "border-color: #9B9B9B;"
                    "border-bottom-color: #C2C7CB; /* same as pane color */"
                    "}"
                    "QTabBar::tab:!selected {"
                    "margin-top: 2px; /* make non-selected tabs look smaller */"
                    "}");
    } else {
      m_tab->tabBar()->setStyleSheet("QTabBar::close-button:hover,QTabBar::close-button:selected {margin: 0px; image: url(:/images/close.png); subcontrol-position: left; }");
    }
#endif
#endif
    }
}
