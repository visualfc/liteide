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
// Module: litedebug.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litedebug.h"
#include "debugmanager.h"
#include "debugwidget.h"
#include "fileutil/fileutil.h"
#include "litedebug_global.h"
#include "selectexternaldialog.h"

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
    LiteApi::ILiteDebug(parent),
    m_liteApp(app),
    m_envManager(0),
    m_liteBuild(0),
    m_debugger(0),
    m_manager(new DebugManager(this)),
    m_widget(new QWidget),
    m_dbgWidget(new DebugWidget(app,this))
{
    m_manager->initWithApp(app);

    m_liteApp->extension()->addObject("LiteApi.ILiteDebug",this);
    m_liteApp->extension()->addObject("LiteApi.IDebuggerManager",m_manager);

    m_debugMimeTypes << "text/x-gosrc" << "text/x-csrc" << "text/x-chdr" << "text/x-c++src";

    m_output = new TextOutput(m_liteApp);
    m_output->setReadOnly(true);
    m_output->setFilterTermColor(true);

    QAction *clearAct = new QAction(tr("Clear"),this);
    clearAct->setIcon(QIcon("icon:images/cleanoutput.png"));
    connect(clearAct,SIGNAL(triggered()),m_output,SLOT(clear()));

    QVBoxLayout *layout = new QVBoxLayout;    
    QToolBar *widgetToolBar = new QToolBar;
    widgetToolBar->setIconSize(LiteApi::getToolBarIconSize(m_liteApp));
    layout->setMargin(0);
    layout->setSpacing(0);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->addWidget(widgetToolBar);
    QPushButton *close = new QPushButton();
    close->setIcon(QIcon("icon:images/closetool.png"));
    close->setIconSize(QSize(16,16));
    close->setFlat(true);
    close->setToolTip(tr("Close"));

    connect(close,SIGNAL(clicked()),this,SLOT(hideDebug()));
    hbox->addStretch(1);
    hbox->addWidget(close);

    layout->addLayout(hbox);
    layout->addWidget(m_dbgWidget->widget());
    m_widget->setLayout(layout);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"Debug");

    m_startDebugExternal = new QAction(tr("Start Debugging External Application..."),this);
    actionContext->regAction(m_startDebugExternal,"StartDebugExternal","");

    m_startDebugAct = new QAction(QIcon("icon:litedebug/images/startdebug.png"),tr("Start Debugging"),this);
    actionContext->regAction(m_startDebugAct,"StartDebug","F5");

    m_startDebugTestAct = new QAction(QIcon("icon:litedebug/images/startdebug.png"),tr("Start Debugging Tests"),this);
    actionContext->regAction(m_startDebugTestAct,"StartDebugTests","F6");


    m_continueAct = new QAction(QIcon("icon:litedebug/images/continue.png"),tr("Continue"),this);
    actionContext->regAction(m_continueAct,"Continue","F5");

    m_stopDebugAct = new QAction(QIcon("icon:litedebug/images/stopdebug.png"),tr("Stop"),this);
    actionContext->regAction(m_stopDebugAct,"StopDebug","Shift+F5");

    m_showLineAct = new QAction(QIcon("icon:litedebug/images/showline.png"),tr("Show Current Line"),this);
    actionContext->regAction(m_showLineAct,"ShowLine","");

    m_stepIntoAct = new QAction(QIcon("icon:litedebug/images/stepinto.png"),tr("Step Into"),this);
    actionContext->regAction(m_stepIntoAct,"StepInfo","F11");

    m_stepOverAct = new QAction(QIcon("icon:litedebug/images/stepover.png"),tr("Step Over"),this);
    actionContext->regAction(m_stepOverAct,"LiteDebug.StepOver","F10");

    m_stepOutAct = new QAction(QIcon("icon:litedebug/images/stepout.png"),tr("Step Out"),this);
    actionContext->regAction(m_stepOutAct,"StepOut","Shift+F11");

    m_runToLineAct = new QAction(QIcon("icon:litedebug/images/runtoline.png"),tr("Run to Line"),this);
    actionContext->regAction(m_runToLineAct,"RunToLine","Ctrl+F10");

    m_switchBreakAct = new QAction(QIcon("icon:litedebug/images/insertbreak.png"),tr("Insert/Remove Breakpoint"),this);
    actionContext->regAction(m_switchBreakAct,"Breakpoint","F9");

    m_removeAllBreakAct = new QAction(tr("Remove All Breakpoints"),this);
    actionContext->regAction(m_removeAllBreakAct,"RemoveAllBreakPoints","");

//    m_toolBar->addSeparator();
//    m_toolBar->addAction(m_startDebugAct);
//    m_toolBar->addAction(m_insertBreakAct);

    m_bLastDebugCmdInput = false;

    widgetToolBar->addAction(m_continueAct);
    widgetToolBar->addAction(m_stopDebugAct);
    widgetToolBar->addSeparator();
    widgetToolBar->addAction(m_switchBreakAct);
    widgetToolBar->addAction(m_showLineAct);
    widgetToolBar->addAction(m_stepIntoAct);
    widgetToolBar->addAction(m_stepOverAct);
    widgetToolBar->addAction(m_stepOutAct);
    widgetToolBar->addAction(m_runToLineAct);

    m_debugMenu = m_liteApp->actionManager()->insertMenu("menu/debug",tr("&Debug"),"menu/help");

    m_debugMenu->addAction(m_startDebugExternal);
    m_debugMenu->addSeparator();
    m_debugMenu->addAction(m_startDebugAct);
    m_debugMenu->addAction(m_startDebugTestAct);
    m_debugMenu->addAction(m_continueAct);
    m_debugMenu->addAction(m_stopDebugAct);
    m_debugMenu->addSeparator();
    m_debugMenu->addAction(m_showLineAct);
    m_debugMenu->addAction(m_stepIntoAct);
    m_debugMenu->addAction(m_stepOverAct);
    m_debugMenu->addAction(m_stepOutAct);
    m_debugMenu->addAction(m_runToLineAct);

    connect(m_manager,SIGNAL(currentDebuggerChanged(LiteApi::IDebugger*)),this,SLOT(setDebugger(LiteApi::IDebugger*)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));

    connect(m_startDebugExternal,SIGNAL(triggered()),this,SLOT(startDebugExternal()));
    connect(m_startDebugAct,SIGNAL(triggered()),this,SLOT(startDebug()));
    connect(m_startDebugTestAct,SIGNAL(triggered()),this,SLOT(startDebugTests()));

    connect(m_continueAct,SIGNAL(triggered()),this,SLOT(continueRun()));
    connect(m_runToLineAct,SIGNAL(triggered()),this,SLOT(runToLine()));
    connect(m_stopDebugAct,SIGNAL(triggered()),this,SLOT(stopDebug()));
    connect(m_stepOverAct,SIGNAL(triggered()),this,SLOT(stepOver()));
    connect(m_stepIntoAct,SIGNAL(triggered()),this,SLOT(stepInto()));
    connect(m_stepOutAct,SIGNAL(triggered()),this,SLOT(stepOut()));
    connect(m_switchBreakAct,SIGNAL(triggered()),this,SLOT(toggleBreakPoint()));
    connect(m_removeAllBreakAct,SIGNAL(triggered()),this,SLOT(removeAllBreakPoints()));
    connect(m_showLineAct,SIGNAL(triggered()),this,SLOT(showLine()));
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_output,SIGNAL(enterText(QString)),this,SLOT(enterAppInputText(QString)));
    connect(m_dbgWidget,SIGNAL(debugCmdInput()),this,SLOT(debugCmdInput()));

    m_outputAct = m_liteApp->toolWindowManager()->addToolWindow(
                Qt::BottomDockWidgetArea,m_output,"debugoutput",tr("Debug Output"),false,
                QList<QAction*>() << clearAct);

    m_continueAct->setEnabled(false);
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
    //QMenu *menu = new QMenu(tr("Select Debug"));
    QActionGroup *group = new QActionGroup(this);
    QString mimeType = m_liteApp->settings()->value(LITEDEBUG_DEBUGGER,"debugger/gdb").toString();
    foreach (LiteApi::IDebugger *debug, m_manager->debuggerList()) {
        QAction *act = new QAction(debug->mimeType(),this);
        act->setObjectName(debug->mimeType());
        act->setCheckable(true);
        group->addAction(act);
        if (mimeType == debug->mimeType()) {
            act->setChecked(true);
            m_manager->setCurrentDebugger(debug);
        }
    }
   // menu->addActions(group->actions());
    connect(group,SIGNAL(triggered(QAction*)),this,SLOT(selectedDebug(QAction*)));
    QAction *first = m_debugMenu->actions().at(0);
    //m_debugMenu->insertMenu(first,menu);
    m_debugMenu->insertActions(first,group->actions());
    m_debugMenu->insertSeparator(first);
}

void LiteDebug::selectedDebug(QAction *act)
{
    stopDebug();

    QString mimeType = act->objectName();
    LiteApi::IDebugger *debug = m_manager->findDebugger(mimeType);
    if (debug) {
        m_manager->setCurrentDebugger(debug);
        m_liteApp->settings()->setValue(LITEDEBUG_DEBUGGER,mimeType);
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

    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_switchBreakAct);
        menu->addAction(m_removeAllBreakAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_switchBreakAct);
    }

    QString filePath = editor->filePath();
    bool ok;
    m_fileBpMap.remove(filePath);
    foreach(QString bp, m_liteApp->globalCookie().value(QString("bp_%1").arg(filePath)).toStringList()) {
        int i = bp.toInt(&ok);
        if (ok) {
            editorMark->addMark(i,LiteApi::BreakPointMark);
            m_fileBpMap.insert(filePath,i);
        }
    }
    if (m_lastLine.fileName == filePath) {
        editorMark->addMark(m_lastLine.line,LiteApi::CurrentLineMark);
    }

    QToolBar *toolBar = LiteApi::getEditToolBar(editor);
    if (toolBar) {
        toolBar->addSeparator();
        toolBar->addAction(m_switchBreakAct);
        toolBar->addAction(m_startDebugAct);
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
    m_liteApp->globalCookie().insert(QString("bp_%1").arg(editor->filePath()),save);
}

void LiteDebug::currentEditorChanged(IEditor *editor)
{
    if (canDebug(editor)) {
        m_startDebugAct->setEnabled(true);
    } else {
        m_startDebugAct->setEnabled(false);
    }
}

void LiteDebug::startDebugExternal()
{
    SelectExternalDialog dlg(m_liteApp);
    dlg.loadSetting();
    if (dlg.exec() == QDialog::Accepted) {
        QString cmd = dlg.getCmd();
        QString args = dlg.getArgs();
        QString work = dlg.getWork();
        dlg.saveSetting();
        this->startDebug(cmd,args,work);
    }
}

void LiteDebug::startDebug(const QString &cmd, const QString &args, const QString &work)
{
    if (!m_debugger) {
        return;
    }
    if (m_debugger->isRunning()) {
        this->continueRun();
        return;
    }

    if (!m_envManager) {
        return;
    }

    emit debugBefore();

    m_dbgWidget->clearLog();

    if (cmd.isEmpty()) {
        m_liteApp->appendLog("LiteDebug","No debugger command specified",true);
        return;
    }
    if (QFileInfo(cmd).isAbsolute()) {
        m_debugInfoId = "litedebug/"+cmd;
    } else {
        m_debugInfoId = "litedebug/"+work+"/"+cmd;
    }

    QDir dir(work);
    foreach (QFileInfo info, dir.entryInfoList(QStringList() << "*.go",QDir::Files)) {
        QString filePath = info.filePath();
        bool ok = false;
        if (m_liteApp->editorManager()->findEditor(filePath,true)) {
            continue;
        }
        m_fileBpMap.remove(filePath);
        foreach(QString bp,m_liteApp->globalCookie().value(QString("bp_%1").arg(filePath)).toStringList()) {
            int i = bp.toInt(&ok);
            if (ok && i >= 0) {
                m_fileBpMap.insert(filePath,i);
            }
        }
    }

    foreach(LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
        LiteApi::IEditorMark *editorMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(editor,"LiteApi.IEditorMark");
        if (!editorMark) {
            continue;
        }
        QString path = editor->filePath();
        m_fileBpMap.remove(path);
        foreach(int i,editorMark->markList(LiteApi::BreakPointMark)) {
            m_fileBpMap.insert(path,i);
        }
    }

    m_debugger->setInitBreakTable(m_fileBpMap);
    m_debugger->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_debugger->setWorkingDirectory(work);
    if (!m_debugger->start(cmd,args)) {
        m_liteApp->appendLog("LiteDebug","Failed to start debugger",true);
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

LiteApi::IDebuggerManager *LiteDebug::debugManager() const
{
    return m_manager;
}

bool LiteDebug::isRunning() const
{
    return m_debugger && m_debugger->isRunning();
}

void LiteDebug::setDebugger(LiteApi::IDebugger *debug)
{
    if (m_debugger) {
        disconnect(m_debugger,0,this,0);
    }
    m_debugger = debug;
    if (m_debugger) {
        connect(m_debugger,SIGNAL(debugStarted()),this,SLOT(debugStarted()));
        connect(m_debugger,SIGNAL(debugStoped()),this,SLOT(debugStoped()));
        connect(m_debugger,SIGNAL(debugLog(LiteApi::DEBUG_LOG_TYPE,QString)),this,SLOT(debugLog(LiteApi::DEBUG_LOG_TYPE,QString)));
        connect(m_debugger,SIGNAL(setCurrentLine(QString,int)),this,SLOT(setCurrentLine(QString,int)));
        connect(m_debugger,SIGNAL(setFrameLine(QString,int)),this,SLOT(setFrameLine(QString,int)));
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
        m_output->appendTag(QString("%1\n").arg(log));
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


    if (!m_liteBuild) {
        return;
    }

    m_liteApp->editorManager()->saveAllEditors();

    LiteApi::TargetInfo info = m_liteBuild->getTargetInfo();

    QString tags = LiteApi::getGoBuildFlagsArgument(m_liteApp,info.workDir,"-tags");

    QStringList args;
    args << "build" << "-gcflags" << "\"-N -l\"" ;
    if (!tags.isEmpty()) {
        args << "-tags" << tags;
    }
    args << "-o" << info.cmd;
    bool b = m_liteBuild->execGoCommand(args,info.workDir,true);
    if (!b) {
        return;
    }

    QString cmd = FileUtil::lookPathInDir(info.cmd,info.workDir);
    if (cmd.isEmpty()) {
        m_liteApp->appendLog("debug",QString("not find execute file in path %2").arg(info.workDir),true);
        return;
    }

    //QString fileName = QFileInfo(cmd).fileName();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor) {
        m_startDebugFile = editor->filePath();
    }

    this->startDebug(info.cmd,info.args,info.workDir);
}

void LiteDebug::startDebugTests()
{
    if (!m_debugger) {
        return;
    }
    if (m_debugger->isRunning()) {
        m_debugger->continueRun();
        return;
    }


    if (!m_liteBuild) {
        return;
    }

    m_liteApp->editorManager()->saveAllEditors();

    LiteApi::TargetInfo info = m_liteBuild->getTargetInfo();

    QStringList args;
    args << "test" << "-gcflags" << "\"-N -l\"" << "-c";
    bool b = m_liteBuild->execGoCommand(args,info.workDir,true);
    if (!b) {
        return;
    }

    QString cmd = FileUtil::lookPathInDir(info.cmd+".test",info.workDir);
    if (cmd.isEmpty()) {
        m_liteApp->appendLog("debug",QString("not find execute test file in path %2").arg(info.workDir),true);
        return;
    }

    QString fileName = QFileInfo(cmd).fileName();

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor) {
        m_startDebugFile = editor->filePath();
    }

    this->startDebug(fileName,info.args,info.workDir);
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
    //QString fileName = QFileInfo(filePath).fileName();
    m_debugger->runToLine(filePath,textEditor->line());
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
    LiteApi::gotoLine(m_liteApp,m_lastLine.fileName,m_lastLine.line,0,true,true);
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
    m_startDebugAct->setEnabled(false);
    m_continueAct->setEnabled(true);
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
    m_startDebugAct->setEnabled(true);
    m_continueAct->setEnabled(false);
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

    if (!m_startDebugFile.isEmpty()) {
        m_liteApp->fileManager()->openEditor(m_startDebugFile,true);
    }

    emit debugVisible(false);

    emit debugEnd();
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
    if (m_bLastDebugCmdInput) {
        m_bLastDebugCmdInput = false;
        m_dbgWidget->setInputFocus();
    }
}

void LiteDebug::setFrameLine(const QString &fileName, int line)
{
    if (QFile::exists(fileName)) {
        LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName,true);
        if (editor) {
            LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
            if (textEditor) {
                textEditor->gotoLine(line,0,true);
            }
            LiteApi::IEditorMark *editMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(editor,"LiteApi.IEditorMark");
            if (editMark) {
                editMark->addMark(line,LiteApi::CurrentLineMark);
            }
        }
    }
}

void LiteDebug::debugCmdInput()
{
    m_bLastDebugCmdInput = true;
}

bool LiteDebug::execGocommand(const QStringList &args, const QString &work, bool showLog)
{
    QString gocmd = FileUtil::lookupGoBin("go",m_liteApp,false);
    if (gocmd.isEmpty()) {
        debugLog(LiteApi::DebugRuntimeLog,QString("go command not find!").arg(args.join(" "),work));
        return false;
    }
    debugLog(LiteApi::DebugRuntimeLog,QString("%1 %2 [%3]").arg(gocmd).arg(args.join(" "),work));
    QProcess process;
    process.setWorkingDirectory(work);
    process.setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    process.start(gocmd,args);
    if (!process.waitForFinished()) {
        return false;
    }
    int code = process.exitCode();
    if (code == 0) {
        return true;
    }
    if (showLog) {
        QByteArray data = process.readAllStandardError();
        debugLog(LiteApi::DebugErrorLog,QString::fromUtf8(data));
    }
    return false;
}

void LiteDebug::enterAppInputText(QString text)
{
    if (m_debugger && m_debugger->isRunning()) {
        m_debugger->enterAppText(text);
    }
}


void LiteDebug::hideDebug()
{
    m_widget->hide();
    emit debugVisible(false);
}
