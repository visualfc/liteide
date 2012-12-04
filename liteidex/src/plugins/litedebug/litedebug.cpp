/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: litedebug.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: litedebug.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "litedebug.h"
#include "debugmanager.h"
#include "debugwidget.h"
#include "fileutil/fileutil.h"

#include <QDir>
#include <QFileInfo>
#include <QLayout>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QPushButton>
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


LiteDebug::LiteDebug(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_envManager(0),
    m_liteBuild(0),
    m_debugger(0),
    m_manager(new DebugManager(this)),
    m_widget(new QWidget),
    m_dbgWidget(new DebugWidget(app,this))
{
    m_manager->initWithApp(app);

    m_debugMimeTypes << "text/x-gosrc" << "text/x-csrc" << "text/x-chdr" << "text/x-c++src";

//    m_toolBar =  m_liteApp->actionManager()->loadToolBar("toolbar/build");
//    if (!m_toolBar) {
//        m_toolBar = m_liteApp->actionManager()->insertToolBar("toolbar/build",tr("Debug ToolBar"),"toolbar/nav");
//    }

//    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuToolBarPos,m_toolBar->toggleViewAction());

    m_output = new TextOutput;
    m_output->setReadOnly(true);
    m_output->setMaxLine(1024);

    QAction *clearAct = new QAction(tr("Clear"),this);
    clearAct->setIcon(QIcon("icon:images/cleanoutput.png"));

    connect(clearAct,SIGNAL(triggered()),m_output,SLOT(clear()));

    QVBoxLayout *layout = new QVBoxLayout;    
    QToolBar *widgetToolBar = new QToolBar;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(widgetToolBar);
    layout->addWidget(m_dbgWidget->widget());
    m_widget->setLayout(layout);

    m_startDebugAct = new QAction(QIcon("icon:litedebug/images/startdebug.png"),tr("Go"),this);
    m_startDebugAct->setShortcut(QKeySequence(Qt::Key_F5));
    m_startDebugAct->setToolTip(tr("Start Debugging (F5)"));

    m_stopDebugAct = new QAction(QIcon("icon:litedebug/images/stopdebug.png"),tr("Stop"),this);
    m_stopDebugAct->setShortcut(QKeySequence(Qt::SHIFT+Qt::Key_F5));
    m_stopDebugAct->setToolTip(tr("Stop Debugger (Shift+F5)"));

    m_showLineAct = new QAction(QIcon("icon:litedebug/images/showline.png"),tr("ShowLine"),this);
    m_showLineAct->setToolTip(tr("Show Current Line"));

    m_stepIntoAct = new QAction(QIcon("icon:litedebug/images/stepinto.png"),tr("StepInto"),this);
    m_stepIntoAct->setShortcut(QKeySequence(Qt::Key_F11));
    m_stepIntoAct->setToolTip(tr("Step Info (F11)"));

    m_stepOverAct = new QAction(QIcon("icon:litedebug/images/stepover.png"),tr("StepOver"),this);
    m_stepOverAct->setShortcut(QKeySequence(Qt::Key_F10));
    m_stepOverAct->setToolTip(tr("Step Over (F10)"));

    m_stepOutAct = new QAction(QIcon("icon:litedebug/images/stepout.png"),tr("StepOut"),this);
    m_stepOutAct->setShortcut(QKeySequence(Qt::SHIFT+Qt::Key_F11));
    m_stepOutAct->setToolTip(tr("Step Out (Shift+F11)"));

    m_runToLineAct = new QAction(QIcon("icon:litedebug/images/runtoline.png"),tr("RunToLine"),this);
    m_runToLineAct->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_F10));
    m_runToLineAct->setToolTip(tr("Run to Line (Ctrl+F10)"));

    m_insertBreakAct = new QAction(QIcon("icon:litedebug/images/insertbreak.png"),tr("Insert/Remove BreakPoint"),this);
    m_insertBreakAct->setShortcut(QKeySequence(Qt::Key_F9));
    m_insertBreakAct->setToolTip(tr("Insert/Remove Breakpoint (F9)"));

    m_removeAllBreakAct = new QAction(tr("Remove All Break Points"),this);

//    m_toolBar->addSeparator();
//    m_toolBar->addAction(m_startDebugAct);
//    m_toolBar->addAction(m_insertBreakAct);

    widgetToolBar->addAction(m_startDebugAct);
    widgetToolBar->addAction(m_stopDebugAct);
    widgetToolBar->addSeparator();
    widgetToolBar->addAction(m_insertBreakAct);
    widgetToolBar->addAction(m_showLineAct);
    widgetToolBar->addAction(m_stepIntoAct);
    widgetToolBar->addAction(m_stepOverAct);
    widgetToolBar->addAction(m_stepOutAct);
    widgetToolBar->addAction(m_runToLineAct);

    m_debugMenu = m_liteApp->actionManager()->insertMenu("menu/debug",tr("&Debug"),"menu/help");

    m_gdbMenu = new QMenu("Debug");
    if (m_gdbMenu) {
        m_gdbMenu->addAction(m_startDebugAct);
        m_gdbMenu->addAction(m_stopDebugAct);
        m_gdbMenu->addSeparator();
        m_gdbMenu->addAction(m_showLineAct);
        m_gdbMenu->addAction(m_stepIntoAct);
        m_gdbMenu->addAction(m_stepOverAct);
        m_gdbMenu->addAction(m_stepOutAct);
        m_gdbMenu->addAction(m_runToLineAct);
    }

    connect(m_manager,SIGNAL(currentDebuggerChanged(LiteApi::IDebugger*)),this,SLOT(setDebugger(LiteApi::IDebugger*)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));

    connect(m_startDebugAct,SIGNAL(triggered()),this,SLOT(startDebug()));
    connect(m_runToLineAct,SIGNAL(triggered()),this,SLOT(runToLine()));
    connect(m_stopDebugAct,SIGNAL(triggered()),this,SLOT(stopDebug()));
    connect(m_stepOverAct,SIGNAL(triggered()),this,SLOT(stepOver()));
    connect(m_stepIntoAct,SIGNAL(triggered()),this,SLOT(stepInto()));
    connect(m_stepOutAct,SIGNAL(triggered()),this,SLOT(stepOut()));
    connect(m_insertBreakAct,SIGNAL(triggered()),this,SLOT(toggleBreakPoint()));
    connect(m_removeAllBreakAct,SIGNAL(triggered()),this,SLOT(removeAllBreakPoints()));
    connect(m_showLineAct,SIGNAL(triggered()),this,SLOT(showLine()));
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_output,SIGNAL(enterText(QString)),this,SLOT(enterAppInputText(QString)));

    m_liteApp->extension()->addObject("LiteApi.IDebugManager",m_manager);

    m_outputAct = m_liteApp->toolWindowManager()->addToolWindow(
                Qt::BottomDockWidgetArea,m_output,"debugoutput",tr("Debug Output"),false,
                QList<QAction*>() << clearAct);

    m_startDebugAct->setToolTip(tr("Start Debugging (F5)"));
    m_stopDebugAct->setEnabled(false);
    m_stepOverAct->setEnabled(false);
    m_showLineAct->setEnabled(false);
    m_stepIntoAct->setEnabled(false);
    m_stepOutAct->setEnabled(false);
    m_runToLineAct->setEnabled(false);
}

LiteDebug::~LiteDebug()
{
    if (m_output) {
        delete m_output;
    }
}

void LiteDebug::appLoaded()
{
    m_liteBuild = LiteApi::findExtensionObject<LiteApi::ILiteBuild*>(m_liteApp,"LiteApi.ILiteBuild");
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");

    LiteApi::IEditorMarkTypeManager *markTypeManager = LiteApi::findExtensionObject<LiteApi::IEditorMarkTypeManager*>(m_liteApp,"LiteApi.IEditorMarkTypeManager");
    if (markTypeManager) {
        markTypeManager->registerMark(BreakPointMark,QIcon("icon:litedebug/images/breakmark.png"));
        markTypeManager->registerMark(CurrentLineMark,QIcon("icon:litedebug/images/linemark.png"));
    }
}

void LiteDebug::editorCreated(LiteApi::IEditor *editor)
{
    if (!canDebug(editor)) {
        return;
    }

    LiteApi::IEditorMark *editorMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(editor,"LiteApi.IEditorMark");
    if (!editorMark) {
        return;
    }

    QMenu *menu = LiteApi::findExtensionObject<QMenu*>(editor,"LiteApi.ContextMenu");
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_insertBreakAct);
        menu->addAction(m_removeAllBreakAct);
    }

    QString filePath = editor->filePath();
    bool ok;
    m_fileBpMap.remove(filePath);
    foreach(QString bp, m_liteApp->settings()->value(QString("bp_%1").arg(filePath)).toStringList()) {
        int i = bp.toInt(&ok);
        if (ok) {
            editorMark->addMark(i,LiteApi::BreakPointMark);
            m_fileBpMap.insert(filePath,i);
        }
    }
    if (m_lastLine.fileName == filePath) {
        editorMark->addMark(m_lastLine.line,LiteApi::CurrentLineMark);
    }

    QToolBar *toolBar = LiteApi::findExtensionObject<QToolBar*>(editor,"LiteApi.QToolBar");
    if (toolBar) {
        QAction *spacer = LiteApi::findExtensionObject<QAction*>(editor,"LiteApi.QToolBar.Spacer");
        toolBar->insertSeparator(spacer);
        toolBar->insertAction(spacer,m_insertBreakAct);
        toolBar->insertAction(spacer,m_startDebugAct);
    }
}

void LiteDebug::editorAboutToClose(LiteApi::IEditor *editor)
{
    if (!canDebug(editor)) {
        return;
    }

    LiteApi::IEditorMark *editorMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(editor,"LiteApi.IEditorMark");
    if (!editorMark) {
        return;
    }
    QList<int> bpList = editorMark->markList(LiteApi::BreakPointMark);
    QStringList save;
    foreach(int bp, bpList) {
        save.append(QString("%1").arg(bp));
    }
    m_liteApp->settings()->setValue(QString("bp_%1").arg(editor->filePath()),save);
}

void LiteDebug::currentEditorChanged(IEditor *editor)
{
    if (canDebug(editor)) {
        m_debugMenu->menuAction()->setMenu(m_gdbMenu);
    } else {
        m_debugMenu->menuAction()->setMenu(0);
    }
}

QWidget *LiteDebug::widget()
{
    return m_widget;
}

bool LiteDebug::canDebug(IEditor *editor) const
{
    if (editor && m_debugMimeTypes.contains(editor->mimeType())) {
        return true;
    }
    return false;
}

void LiteDebug::setDebugger(LiteApi::IDebugger *debug)
{
    m_debugger = debug;
    if (m_debugger) {
        connect(m_debugger,SIGNAL(debugStarted()),this,SLOT(debugStarted()));
        connect(m_debugger,SIGNAL(debugStoped()),this,SLOT(debugStoped()));
        connect(m_debugger,SIGNAL(debugLog(LiteApi::DEBUG_LOG_TYPE,QString)),this,SLOT(debugLog(LiteApi::DEBUG_LOG_TYPE,QString)));
        connect(m_debugger,SIGNAL(setCurrentLine(QString,int)),this,SLOT(setCurrentLine(QString,int)));
        connect(m_debugger,SIGNAL(debugLoaded()),this,SLOT(debugLoaded()));
    }
    m_dbgWidget->setDebugger(m_debugger);
}

void LiteDebug::debugLog(LiteApi::DEBUG_LOG_TYPE type, const QString &log)
{
    switch(type) {
    case LiteApi::DebugConsoleLog:
        m_dbgWidget->appendLog(log);
        break;
    case LiteApi::DebugRuntimeLog:
        m_output->appendTag1(QString("%1\n").arg(log));
        break;
    case LiteApi::DebugErrorLog:
        m_output->append(QString("%1\n").arg(log));
        break;
    case LiteApi::DebugApplationLog:
        m_output->append(log);
        break;
    }
}

void LiteDebug::startDebug()
{
    if (!m_debugger) {
        return;
    }
    if (m_debugger->isRunning()) {
        m_debugger->continueRun();
        return;
    }
    if (!m_liteBuild || !m_liteBuild->buildManager()->currentBuild()) {
        return;
    }
    if (!m_envManager) {
        return;
    }
    m_dbgWidget->clearLog();

    QString targetFilepath = m_liteBuild->targetFilePath();
    m_debugInfoId = targetFilepath;
    if (targetFilepath.isEmpty() || !QFile::exists(targetFilepath)) {
        m_liteApp->appendLog("litedebug",QString("not find execute target %1").arg(targetFilepath),true);
        return;
    }
    QMap<QString,QString> m = m_liteBuild->buildEnvMap();
    QString workDir = m.value("WORKDIR");
    QString target = m.value("TARGETNAME");
    QString args = m.value("TARGETARGS");
    int index = targetFilepath.lastIndexOf(target);
    if (index != -1) {
        target = targetFilepath.right(targetFilepath.length()-index);
    }
    QDir dir(workDir);
    foreach (QFileInfo info, dir.entryInfoList(QStringList() << "*.go",QDir::Files)) {
        QString filePath = info.filePath();
        bool ok = false;
        if (m_liteApp->editorManager()->findEditor(filePath,true)) {
            continue;
        }
        m_fileBpMap.remove(filePath);
        foreach(QString bp,m_liteApp->settings()->value(QString("bp_%1").arg(filePath)).toStringList()) {
            int i = bp.toInt(&ok);
            if (ok && i >= 0) {
                m_fileBpMap.insert(filePath,i);
            }
        }
    }

    /*
    LiteApi::IBuild *build = m_liteBuild->buildManager()->currentBuild();
    QList<LiteApi::BuildDebug*> debugList = build->debugList();
    if (debugList.isEmpty()) {
        return;
    }
    LiteApi::BuildDebug *buildDebug = debugList.first();
    QString workDir = m_liteBuild->envValue(build,buildDebug->work());
    QString cmd = m_liteBuild->envValue(build,buildDebug->cmd());
    QString args = m_liteBuild->envValue(build,buildDebug->args());
    //QString target = FileUtil::lookPath(cmd,m_envManager->currentEnvironment(),true);
    QString target = cmd;
    if (!QFileInfo(target).exists()) {
        target = FileUtil::lookPathInDir(cmd,m_envManager->currentEnvironment(),workDir);
    }

    if (!QFileInfo(target).exists()) {
        m_liteApp->appendLog("litebuild",QString("no find target %1").arg(cmd),true);
        return;
    }
    */
    m_debugger->setInitBreakTable(m_fileBpMap);
    m_debugger->setEnvironment(m_envManager->currentEnvironment().toStringList());
    m_debugger->setWorkingDirectory(workDir);
    m_debugger->start(target,args);
}

void LiteDebug::continueRun()
{
    if (!m_debugger || !m_debugger->isRunning()) {
        return;
    }
    m_debugger->continueRun();
}

void LiteDebug::runToLine()
{
    if (!m_debugger || !m_debugger->isRunning()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
    if (!textEditor) {
        return;
    }
    QString filePath = textEditor->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    QString fileName = QFileInfo(filePath).fileName();
    m_debugger->runToLine(fileName,textEditor->line()+1);
}

void LiteDebug::stopDebug()
{
    if (!m_debugger || !m_debugger->isRunning()) {
        return;
    }
    m_debugger->stop();
}

void LiteDebug::stepOver()
{
    if (!m_debugger || !m_debugger->isRunning()) {
        return;
    }
    m_debugger->stepOver();
}

void LiteDebug::stepInto()
{
    if (!m_debugger || !m_debugger->isRunning()) {
        return;
    }
    m_debugger->stepInto();
}

void LiteDebug::stepOut()
{
    if (!m_debugger || !m_debugger->isRunning()) {
        return;
    }
    m_debugger->stepOut();
}

void LiteDebug::showLine()
{
    if (m_lastLine.fileName.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(m_lastLine.fileName,true);
    if (editor) {
        LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
        if (textEditor) {
            textEditor->gotoLine(m_lastLine.line,0,true);
        }
    }
}

void LiteDebug::removeAllBreakPoints()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::IEditorMark *editorMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(editor,"LiteApi.IEditorMark");
    if (!editorMark) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
    if (!textEditor) {
        return;
    }
    QString filePath = textEditor->filePath();
    foreach(int line, editorMark->markList(LiteApi::BreakPointMark)) {
        editorMark->removeMark(line,LiteApi::BreakPointMark);
        m_fileBpMap.remove(filePath,line);
        if (m_debugger && m_debugger->isRunning()) {
            m_debugger->removeBreakPoint(filePath,line);
        }
    }
}

void LiteDebug::toggleBreakPoint()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::IEditorMark *editorMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(editor,"LiteApi.IEditorMark");
    if (!editorMark) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
    if (!textEditor) {
        return;
    }
    int line = textEditor->line();
    QString fileName = editor->filePath();
    if (fileName.isEmpty()) {
        return;
    }
    QList<int> marks = editorMark->lineTypeList(line);
    if (marks.contains(LiteApi::BreakPointMark)) {
        editorMark->removeMark(line,LiteApi::BreakPointMark);
        m_fileBpMap.remove(fileName,line);
        if (m_debugger && m_debugger->isRunning()) {
            m_debugger->removeBreakPoint(fileName,line);
        }
    } else {
        editorMark->addMark(line,LiteApi::BreakPointMark);
        m_fileBpMap.insert(fileName,line);
        if (m_debugger && m_debugger->isRunning()) {
            m_debugger->insertBreakPoint(fileName,line);
        }
    }
}

void LiteDebug::clearLastLine()
{
    if (!m_lastLine.fileName.isEmpty()) {
        LiteApi::IEditor *lastEditor = m_liteApp->editorManager()->findEditor(m_lastLine.fileName,true);
        if (lastEditor) {
            LiteApi::IEditorMark *lastMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(lastEditor,"LiteApi.IEditorMark");
            if (lastMark) {
                lastMark->removeMark(m_lastLine.line,LiteApi::CurrentLineMark);
            }
        }
    }
    m_lastLine.fileName.clear();
}

void LiteDebug::debugLoaded()
{
    if (!m_debugInfoId.isEmpty()) {
        m_dbgWidget->loadDebugInfo(m_debugInfoId);
    }
}

void LiteDebug::debugStarted()
{
    m_startDebugAct->setToolTip(tr("Continue (F5)"));
    m_stopDebugAct->setEnabled(true);
    m_showLineAct->setEnabled(true);
    m_stepOverAct->setEnabled(true);
    m_stepIntoAct->setEnabled(true);
    m_stepOutAct->setEnabled(true);
    m_runToLineAct->setEnabled(true);
    m_output->setReadOnly(false);
    //m_liteApp->outputManager()->setCurrentOutput(m_output);
    m_outputAct->setChecked(true);
    m_widget->show();    
    emit debugVisible(true);
}

void LiteDebug::debugStoped()
{
    m_startDebugAct->setToolTip(tr("Start Debugging (F5)"));
    m_stopDebugAct->setEnabled(false);
    m_stepOverAct->setEnabled(false);
    m_showLineAct->setEnabled(false);
    m_stepIntoAct->setEnabled(false);
    m_stepOutAct->setEnabled(false);
    m_runToLineAct->setEnabled(false);
    clearLastLine();
    m_output->setReadOnly(true);
    //m_liteApp->outputManager()->setCurrentOutput(m_output);
    if (!m_debugInfoId.isEmpty())
        m_dbgWidget->saveDebugInfo(m_debugInfoId);
    m_widget->hide();
    emit debugVisible(false);
}

void LiteDebug::setCurrentLine(const QString &fileName, int line)
{ 
    bool center = true;
    if (m_lastLine.fileName == fileName) {
        center = false;
    }
    clearLastLine();
    if (QFile::exists(fileName)) {
        LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName,true);
        if (editor) {
            m_lastLine.fileName = fileName;
            m_lastLine.line = line;
            LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
            if (textEditor) {
                textEditor->gotoLine(line,0,center);
            }
            LiteApi::IEditorMark *editMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(editor,"LiteApi.IEditorMark");
            if (editMark) {
                editMark->addMark(line,LiteApi::CurrentLineMark);
            }
        }
    }
}

void LiteDebug::enterAppInputText(QString text)
{
    if (m_debugger && m_debugger->isRunning()) {
        m_debugger->enterText(text);
    }
}

