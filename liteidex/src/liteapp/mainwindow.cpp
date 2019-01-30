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
// Module: mainwindow.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "mainwindow.h"
#include "aboutdialog.h"

#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDockWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QUrl>
#include <QMimeData>
#include <QTextBlock>
#include <QRegExp>
#include <QFileInfo>
#include <QSplitter>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QMenu* MainWindow::s_macDocMenu = 0;
QMap<QWidget*, QAction*> MainWindow::s_windowActions;

MainWindow::MainWindow(IApplication *app, QWidget *parent)
    : ToolMainWindow(parent),
      m_liteApp(app),
      m_windowClosedCheck(false)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    QIcon icon;
    icon.addFile("icon:images/liteide16.png");
    icon.addFile("icon:images/liteide24.png");
    icon.addFile("icon:images/liteide32.png");
    icon.addFile("icon:images/liteide48.png");
    icon.addFile("icon:images/liteide64.png");
    icon.addFile("icon:images/liteide128.png");
    this->setWindowIcon(icon);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setAcceptDrops(true);

    m_mainSplitter = new QSplitter(Qt::Vertical,this);
    setCentralWidget(m_mainSplitter);

#if QT_VERSION >= 0x050200
#ifdef Q_OS_OSX
    if (!s_macDocMenu) {
        s_macDocMenu = new QMenu;
        s_macDocMenu->setAsDockMenu();
    }
    if (s_macDocMenu) {
        QAction *act = new QAction("macOS",this);
        act->setCheckable(true);
        s_macDocMenu->addAction(act);
        connect(act,SIGNAL(triggered(bool)),this,SLOT(triggeredWindowsAct()));
        s_windowActions.insert(this,act);
    }
#endif
#endif
}

QSplitter *MainWindow::splitter()
{
    return m_mainSplitter;
}

void MainWindow::setWindowStyle(IWindowStyle *style)
{
    ToolMainWindow::setWindowStyle(style);
    //loadInitToolState(m_liteApp->settings()->value("liteapp/toolState").toByteArray());
}

MainWindow::~MainWindow()
{
    s_windowActions.remove(this);
    delete m_liteApp;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (this->m_windowClosedCheck) {
        return;
    }
    m_liteApp->saveSession(m_liteApp->currentSession());
    m_liteApp->saveState();
    m_liteApp->projectManager()->closeProject();
    if (m_liteApp->editorManager()->closeAllEditors()) {
        this->m_windowClosedCheck = true;
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange) {
        bool b = (windowState() & Qt::WindowFullScreen) != 0;
        emit fullScreenStateChanged(b);
    }
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        setActiveWindowAction(this);
    }
    return ToolMainWindow::event(event);
}

void MainWindow::setFullScreen(bool b)
{
    if (bool(windowState() & Qt::WindowFullScreen) == b)
        return;
    if (b) {
        setWindowState(windowState() | Qt::WindowFullScreen);
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    QStringList folders;
    foreach (QUrl url, urls) {
        QString fileName = url.toLocalFile();
        if (fileName.isEmpty()) {
            return;
        }
        QFileInfo info(fileName);
        if (info.isFile()) {
            m_liteApp->fileManager()->openFile(fileName);
        } else if(info.isDir()) {
            folders.append(info.filePath());
        }
    }
    if (!folders.isEmpty()) {
        foreach (QString folder, folders) {
            m_liteApp->fileManager()->addFolderList(folder);
        }
    }
}

void MainWindow::setWindowTitle(const QString &name, const QString &filePath, bool isModify)
{
    QAction *act = s_windowActions.value(this);
    if (act) {
        QString title = QString("(%1)").arg(m_liteApp->currentSession());
        if (!name.isEmpty()) {
            title = name + " - "+title;
        }
        act->setText(title);
    }

    QString title = QString("LiteIDE (%1)").arg(m_liteApp->currentSession());
    if (!filePath.isEmpty()) {
        QString path = QDir::toNativeSeparators(filePath);
        if (isModify) {
            path += "*";
        }
        title = path + " - "+title;
    } else if (!name.isEmpty()) {
        title = name + " - "+title;
    }
    ToolMainWindow::setWindowTitle(title);
}

void MainWindow::currentEditorChanged(IEditor *editor)
{
    QString name;
    QString filePath;
    bool isModified = false;
    if (editor) {
        name = editor->name();
        filePath = editor->filePath();
        isModified = editor->isModified();
    }
    this->setWindowTitle(name,filePath,isModified);
}

void MainWindow::editorModifyChanged(IEditor *editor, bool b)
{
    QString name;
    QString filePath;
    if (editor) {
        name = editor->name();
        filePath = editor->filePath();
    }
    this->setWindowTitle(name,filePath,b);
}

void MainWindow::about()
{
    AboutDialog *dlg = new AboutDialog(m_liteApp,m_liteApp->mainWindow());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

void MainWindow::triggeredWindowsAct()
{
    QAction *act = (QAction*)sender();
    if (!act) {
        return;
    }
    QWidget *widget = s_windowActions.key(act,0);
    if (!widget) {
        return;
    }
    QWidget *window = widget->window();
    if (window == this) {
        act->setChecked(true);
        this->setWindowState(windowState() & ~Qt::WindowMinimized);
    }
    window->raise();
    window->activateWindow();
}

void MainWindow::setActiveWindowAction(QWidget *window)
{
    QMapIterator<QWidget*,QAction*> it(s_windowActions);
    while (it.hasNext()) {
        it.next();
        it.value()->setChecked(it.key() == window);
    }
}

