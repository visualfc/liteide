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
#include "liteenvapi/liteenvapi.h"
#include <QTabWidget>
#include <QFileInfo>
#include <QDir>
#include <QTime>

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

    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,m_tab,"Terminal",tr("Terminal"),true,
                                                                    QList<QAction*>() << m_newTabAct << m_closeTabAct);
    connect(m_toolWindowAct,SIGNAL(toggled(bool)),this,SLOT(visibilityChanged(bool)));
    connect(m_tab,SIGNAL(tabCloseRequested(int)),this,SLOT(tabCloseRequested(int)));
}

void Terminal::newTerminal()
{
    VTermWidget *term = new VTermWidget(m_tab);
    //term->setDarkMode(true);
    QString dir;
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor && !editor->filePath().isEmpty()) {
        dir = QFileInfo(editor->filePath()).path();
    }
    if (dir.isEmpty()) {
        dir = QDir::homePath();
    }
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    QString info = QString("\033[1m%1: %2\033[0m\r\n").arg(QTime::currentTime().toString("hh:mm:ss")).arg(dir);
    term->inputWrite(info.toUtf8());
#ifdef Q_OS_WIN
   term->start("c:\\windows\\system32\\cmd.exe",QStringList(),dir,env.toStringList());//) << "-i" << "-l",env.toStringList());
//      m_term->start("C:\\Program Files\\Git\\bin\\bash.exe",QStringList(),env.toStringList());//) << "-i" << "-l",env.toStringList());
//    m_term->start("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe",QStringList(),env.toStringList());//) << "-i" << "-l",env.toStringList());
#else
    term->start("/bin/bash",QStringList() << "-i" << "-l",dir,env.toStringList());
#endif
    int index = m_tab->addTab(term,QString("terminal %1").arg(++m_indexId));
    m_tab->setCurrentIndex(index);
    term->setFocus();
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
        m_tab->setTabText(index,title);
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
