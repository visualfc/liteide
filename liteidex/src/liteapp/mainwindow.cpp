/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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


MainWindow::MainWindow(IApplication *app,QWidget *parent)
    : ToolMainWindow(parent),
      m_liteApp(app)
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

    loadInitToolState(m_liteApp->settings()->value("liteapp/toolState").toByteArray());
}

QSplitter *MainWindow::splitter()
{
    return m_mainSplitter;
}

MainWindow::~MainWindow()
{
    delete m_liteApp;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_liteApp->saveSession("default");
    m_liteApp->saveState();
    m_liteApp->projectManager()->closeProject();
    if (m_liteApp->editorManager()->closeAllEditors(false)) {
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
    foreach (QUrl url, urls) {
        QString fileName = url.toLocalFile();
        if (fileName.isEmpty()) {
            return;
        }
        QFileInfo info(fileName);
        if (info.isFile()) {
            m_liteApp->fileManager()->openFile(fileName);
        } else if(info.isDir()) {
            m_liteApp->fileManager()->openFolderProject(info.filePath());
        }
    }
}

void MainWindow::currentEditorChanged(IEditor *editor)
{
    QString title = "LiteIDE";
    if (editor && !editor->filePath().isEmpty()) {
        title += " - " + editor->filePath();
        if (editor->isModified()) {
            title += " * ";
        }
        connect(editor,SIGNAL(modificationChanged(bool)),this,SLOT(editorModifyChanged(bool)));
    }
    this->setWindowTitle(QDir::toNativeSeparators(title));
}

void MainWindow::editorModifyChanged(bool b)
{
    IEditor *editor = (IEditor*)sender();
    QString title = "LiteIDE";
    if (editor && !editor->filePath().isEmpty()) {
        title += " - " + editor->filePath();
        if (b == true) {
            title += " * ";
        }
        this->setWindowTitle(QDir::toNativeSeparators(title));
    }
}

void MainWindow::about()
{
    AboutDialog *dlg = new AboutDialog(m_liteApp,m_liteApp->mainWindow());
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

