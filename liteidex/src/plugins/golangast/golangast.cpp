/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: golangast.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangast.h"
#include "golangastitem.h"
#include "golangasticon.h"
#include "astwidget.h"
#include "liteenvapi/liteenvapi.h"
#include "golangast_global.h"

#include <QStackedWidget>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QDir>
#include <QAction>
#include <QLabel>
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


GolangAst::GolangAst(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IGolangAst(parent),
    m_liteApp(app)
{
    m_currentEditor = 0;
    m_currentPlainTextEditor = 0;
    m_blankWidget = new QLabel(tr("No outline available"));
    m_blankWidget->setAlignment(Qt::AlignCenter);

    m_stackedWidget = new QStackedWidget;
    m_stackedWidget->addWidget(m_blankWidget);

    m_projectAstWidget = new AstWidget(false,m_liteApp);

    m_process = new QProcess(this);
    m_timer = new QTimer(this);

    m_processFile = new QProcess(this);
    m_timerFile = new QTimer(this);

    m_syncClassViewAct = new QAction(QIcon("icon:images/sync.png"),tr("Synchronize with editor"),this);
    m_syncClassViewAct->setCheckable(true);

    m_syncOutlineAct = new QAction(QIcon("icon:images/sync.png"),tr("Synchronize with editor"),this);
    m_syncOutlineAct->setCheckable(true);

    m_classViewToolAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_projectAstWidget,"GoClassView",tr("Go Class View"),false, QList<QAction*>() << m_syncClassViewAct);
    m_outlineToolAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_stackedWidget,"GoOutline",tr("Go Outline"),false, QList<QAction*>() << m_syncOutlineAct);

    connect(m_classViewToolAct,SIGNAL(toggled(bool)),this,SLOT(astProjectEnable(bool)));
    connect(m_outlineToolAct,SIGNAL(toggled(bool)),this,SLOT(astFileEnable(bool)));

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(m_liteApp->projectManager(),SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(projectChanged(LiteApi::IProject*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(editorChanged(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finishedProcess(int,QProcess::ExitStatus)));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(updateAstNow()));
    connect(m_processFile,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finishedProcessFile(int,QProcess::ExitStatus)));
    connect(m_timerFile,SIGNAL(timeout()),this,SLOT(updateAstNowFile()));
    connect(m_syncClassViewAct,SIGNAL(triggered(bool)),this,SLOT(syncClassView(bool)));
    connect(m_syncOutlineAct,SIGNAL(triggered(bool)),this,SLOT(syncOutline(bool)));

    m_liteApp->extension()->addObject("LiteApi.IGolangAst",this);

    m_isSyncClassView = m_liteApp->settings()->value(GOLANGAST_CLASSVIEW_SYNCEDITOR,false).toBool();
    m_syncClassViewAct->setChecked(m_isSyncClassView);

    m_isSyncOutline = m_liteApp->settings()->value(GOLANGAST_OUTLINE_SYNCEDITOR,false).toBool();
    m_syncOutlineAct->setChecked(m_isSyncOutline);
}

GolangAst::~GolangAst()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    delete m_process;
    if (m_timerFile->isActive()) {
        m_timerFile->stop();
    }
    delete m_processFile;
    m_liteApp->toolWindowManager()->removeToolWindow(m_projectAstWidget);
    m_liteApp->toolWindowManager()->removeToolWindow(m_stackedWidget);
    delete m_projectAstWidget;
    delete m_stackedWidget;
}

QIcon GolangAst::iconFromTag(const QString &tag, bool pub) const
{
    return GolangAstIcon::instance()->iconFromTag(tag,pub);
}

QIcon GolangAst::iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool pub) const
{
    return GolangAstIcon::instance()->iconFromTagEnum(tag,pub);
}

void GolangAst::astProjectEnable(bool b)
{
    if (b) {
        //loadProject(m_liteApp->projectManager()->currentProject());
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (!editor) {
            return;
        }
        QString fileName = editor->filePath();
        if (!fileName.isEmpty()) {
            QFileInfo info(fileName);
            loadProjectPath(info.path());
        }
    }
}

void GolangAst::astFileEnable(bool b)
{
    if (b) {
        editorChanged(m_liteApp->editorManager()->currentEditor());
    }
}


void GolangAst::setEnable(bool b)
{
    if (b) {
        m_stackedWidget->setVisible(true);
        loadProject(m_liteApp->projectManager()->currentProject());
        editorChanged(m_liteApp->editorManager()->currentEditor());
        updateAst();
    } else {
        //m_stackedWidget->setVisible(false);
    }
}

void GolangAst::projectChanged(LiteApi::IProject *project)
{
    return;
    if (project) {
        m_projectAstWidget->clear();
    }
    loadProject(project);
    if (project) {
        connect(project,SIGNAL(reloaded()),this,SLOT(projectReloaded()));
    }
}

void GolangAst::projectReloaded()
{
    LiteApi::IProject *project = (LiteApi::IProject*)sender();
    loadProject(project);
}

void GolangAst::loadProjectPath(const QString &path)
{
    if (m_projectAstWidget->isHidden()) {
        return;
    }

    m_updateFileNames.clear();
    m_updateFilePaths.clear();
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    foreach (QFileInfo info, dir.entryInfoList(QStringList()<<"*.go",QDir::Files)) {
        m_updateFileNames.append(info.fileName());
        m_updateFilePaths.append(info.filePath());
    }

    m_workPath = path;
    m_process->setWorkingDirectory(m_workPath);
    m_projectAstWidget->setWorkPath(m_workPath);
    updateAst();
    /*
    if (project) {
        foreach(QString file, project->fileNameList()) {
            if (QFileInfo(file).suffix() == "go") {
                m_updateFileNames.append(file);
            }
        }
        foreach(QString file, project->filePathList()) {
            QFileInfo info(file);
            if (info.suffix() == "go") {
                m_updateFilePaths.append(info.filePath());
            }
        }
        QFileInfo info(project->filePath());
        if (info.isDir()) {
            m_workPath = info.filePath();
        } else {
            m_workPath = info.path();
        }
        m_process->setWorkingDirectory(m_workPath);
        m_projectAstWidget->setWorkPath(m_workPath);
        updateAst();
    } else {
        m_projectAstWidget->clear();
    }
    */
}

void GolangAst::loadProject(LiteApi::IProject *project)
{
    return;
    m_updateFileNames.clear();
    m_updateFilePaths.clear();
    if (project) {
        foreach(QString file, project->fileNameList()) {
            if (QFileInfo(file).suffix() == "go") {
                m_updateFileNames.append(file);
            }
        }
        foreach(QString file, project->filePathList()) {
            QFileInfo info(file);
            if (info.suffix() == "go") {
                m_updateFilePaths.append(info.filePath());
            }
        }
        QFileInfo info(project->filePath());
        if (info.isDir()) {
            m_workPath = info.filePath();
        } else {
            m_workPath = info.path();
        }
        m_process->setWorkingDirectory(m_workPath);
        m_projectAstWidget->setWorkPath(m_workPath);
        updateAst();
    } else {
        m_projectAstWidget->clear();
    }
}

void GolangAst::editorCreated(LiteApi::IEditor *editor)
{
    AstWidget *w = m_editorAstWidgetMap.value(editor);
    if (w) {
        return;
    }
    if (editor) {
        QString fileName = editor->filePath();
        if (!fileName.isEmpty()) {
            QFileInfo info(fileName);
            if (info.suffix() == "go") {
                AstWidget *w = new AstWidget(true,m_liteApp);
                w->setWorkPath(info.absolutePath());
                m_stackedWidget->addWidget(w);
                m_editorAstWidgetMap.insert(editor,w);
            }
        }
    }
}

void GolangAst::editorAboutToClose(LiteApi::IEditor *editor)
{
    AstWidget *w = m_editorAstWidgetMap.value(editor);
    if (w == 0) {
        return;
    }
    m_stackedWidget->removeWidget(w);
    m_editorAstWidgetMap.remove(editor);
}

void GolangAst::editorChanged(LiteApi::IEditor *editor)
{
    m_editorFileName.clear();
    m_editorFilePath.clear();


    if (m_currentPlainTextEditor) {
        disconnect(m_currentPlainTextEditor,0,this,0);
    }

    m_currentEditor = editor;
    m_currentPlainTextEditor = LiteApi::getPlainTextEdit(editor);
    if (m_currentPlainTextEditor) {
        connect(m_currentPlainTextEditor,SIGNAL(cursorPositionChanged()),this,SLOT(editorPositionChanged()));
    }

    AstWidget *w = m_editorAstWidgetMap.value(editor);
    if (w) {
        m_stackedWidget->setCurrentWidget(w);
    } else {
        m_stackedWidget->setCurrentWidget(m_blankWidget);
    }
    if (editor) {        
        QString fileName = editor->filePath();
        if (!fileName.isEmpty()) {
            QFileInfo info(fileName);
            m_workPath = info.absolutePath();
            if (info.suffix() == "go") {
                m_processFile->setWorkingDirectory(info.absolutePath());
                m_editorFileName.append(info.fileName());
                m_editorFilePath.append(info.filePath());
                loadProjectPath(info.path());
            }
            updateAstFile();
        }
    }
}

void GolangAst::editorSaved(LiteApi::IEditor *editor)
{
    if (editor) {
        QString fileName = editor->filePath();
        QFileInfo info(fileName);
        if (!fileName.isEmpty() && info.suffix() == "go") {
            updateAstFile();
            if (m_updateFilePaths.contains(info.filePath())) {
                updateAst();
            }
        }
    }
}

void GolangAst::updateAst()
{
    m_timer->start(1500);
}

void GolangAst::updateAstNow()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    if (m_updateFileNames.isEmpty()) {
        return;
    }
    QString cmd = LiteApi::getGotools(m_liteApp);
    QStringList args;
    args << "astview" << "-end";
    args << "-todo";
    args << m_updateFileNames;
    m_process->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_process->start(cmd,args);
}

void GolangAst::updateAstFile()
{
    m_timerFile->start(1000);
}

void GolangAst::updateAstNowFile()
{
    if (m_timerFile->isActive()) {
        m_timerFile->stop();
    }
    if (m_editorFileName.isEmpty()) {
        return;
    }
    QString cmd = LiteApi::getGotools(m_liteApp);
    QStringList args;
    args << "astview" << "-end";
    args << "-todo";
    args << m_editorFileName;
    m_processFile->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_processFile->start(cmd,args);
}

void GolangAst::syncClassView(bool b)
{
    m_isSyncClassView = b;
    m_liteApp->settings()->setValue(GOLANGAST_CLASSVIEW_SYNCEDITOR,m_isSyncClassView);
}

void GolangAst::syncOutline(bool b)
{
    m_isSyncOutline = b;
    m_liteApp->settings()->setValue(GOLANGAST_OUTLINE_SYNCEDITOR,m_isSyncOutline);
}

void GolangAst::editorPositionChanged()
{
    if (!m_currentEditor || !m_currentPlainTextEditor) {
        return;
    }
    QTextCursor cursor = m_currentPlainTextEditor->textCursor();
    if (m_isSyncClassView && m_classViewToolAct->isChecked()) {
        m_projectAstWidget->trySyncIndex(m_currentEditor->filePath(),cursor.blockNumber(),cursor.positionInBlock());
    }
    if (m_isSyncOutline && m_outlineToolAct->isChecked()) {
        AstWidget *w = m_editorAstWidgetMap.value(m_currentEditor);
        if (w) {
            w->trySyncIndex(m_currentEditor->filePath(),cursor.blockNumber(),cursor.positionInBlock());
        }
    }
}

void GolangAst::finishedProcess(int code,QProcess::ExitStatus status)
{
    if (code == 0 && status == QProcess::NormalExit) {
       // if (m_liteApp->projectManager()->currentProject()) {
        m_projectAstWidget->updateModel(m_process->readAllStandardOutput());
        if (m_isSyncClassView && m_currentPlainTextEditor) {
            QTextCursor cursor = m_currentPlainTextEditor->textCursor();
            m_projectAstWidget->trySyncIndex(m_currentEditor->filePath(),cursor.blockNumber(),cursor.positionInBlock());
        }
       // }
    } else {
        //qDebug() << m_process->readAllStandardError();
    }
}

void GolangAst::finishedProcessFile(int code,QProcess::ExitStatus status)
{
    if (code == 0 && status == QProcess::NormalExit) {
        if (m_currentEditor) {
            AstWidget *w = m_editorAstWidgetMap.value(m_currentEditor);
            if (w) {
                w->updateModel(m_processFile->readAllStandardOutput());
                if (m_isSyncOutline && m_currentPlainTextEditor) {
                    QTextCursor cursor = m_currentPlainTextEditor->textCursor();
                    w->trySyncIndex(m_currentEditor->filePath(),cursor.blockNumber(),cursor.positionInBlock());
                }
            }
        }
    } else {
        //qDebug() << m_process->readAllStandardError();
    }
}
