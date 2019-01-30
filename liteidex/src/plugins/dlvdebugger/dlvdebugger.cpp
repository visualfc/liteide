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
// Module: dlvdebugger.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "dlvdebugger.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "dlvdebuggeroption.h"
#include "../litedebug/litedebug_global.h"

#include <QStandardItemModel>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
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

static void GdbMiValueToItem(QStandardItem *item, const GdbMiValue &value)
{
    switch (value.type()) {
    case GdbMiValue::Invalid:
        item->appendRow(new QStandardItem("Invalid"));
        break;
    case GdbMiValue::Const:
        if (value.name().isEmpty()) {
            item->appendRow(new QStandardItem(QString(value.data())));
        } else {
            item->appendRow(new QStandardItem(QString(value.name()+"="+value.data())));
        }
        break;
    case GdbMiValue::List: {
            QStandardItem *in = new QStandardItem(QString(value.name()));
            item->appendRow(in);
            for (int i = 0; i < value.childCount(); i++) {
                QStandardItem *iv = new QStandardItem(QString("[%1]").arg(i));
                in->appendRow(iv);
                GdbMiValueToItem(iv,value.childAt(i));
            }
            break;
        }
    case GdbMiValue::Tuple: {
            QStandardItem *iv = item;
            if (!value.name().isEmpty()) {
                iv = new QStandardItem(QString(value.name()));
                item->appendRow(iv);
            }
            foreach (const GdbMiValue &v, value.children()) {
                GdbMiValueToItem(iv,v);
            }
            break;
       }
    }
}

DlvDebugger::DlvDebugger(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IDebugger(parent),
    m_liteApp(app),
    m_envManager(0)
{
    m_process = new LiteProcess(m_liteApp,this);
    m_process->setUseCtrlC(true);

    m_asyncModel = new QStandardItemModel(this);
    m_asyncItem = new QStandardItem;
    m_asyncModel->appendRow(m_asyncItem);
    /*
    m_asyncModel->setHeaderData(0,Qt::Horizontal,"Reason");
    m_asyncModel->setHeaderData(1,Qt::Horizontal,"Address");
    m_asyncModel->setHeaderData(2,Qt::Horizontal,"Function");
    m_asyncModel->setHeaderData(3,Qt::Horizontal,"File");
    m_asyncModel->setHeaderData(4,Qt::Horizontal,"Line");
    m_asyncModel->setHeaderData(5,Qt::Horizontal,"Thread ID");
    m_asyncModel->setHeaderData(6,Qt::Horizontal,"Stoped Threads");
    */
    m_varsModel = new QStandardItemModel(0,2,this);
    m_varsModel->setHeaderData(0,Qt::Horizontal,"Name");
    m_varsModel->setHeaderData(1,Qt::Horizontal,"Value");
    //m_varsModel->setHeaderData(2,Qt::Horizontal,"Type");

    m_watchModel = new QStandardItemModel(0,2,this);
    m_watchModel->setHeaderData(0,Qt::Horizontal,"Name");
    m_watchModel->setHeaderData(1,Qt::Horizontal,"Value");
    //m_watchModel->setHeaderData(2,Qt::Horizontal,"Type");

    m_framesModel = new QStandardItemModel(0,5,this);
    m_framesModel->setHeaderData(0,Qt::Horizontal,"Level");
    m_framesModel->setHeaderData(1,Qt::Horizontal,"Address");
    m_framesModel->setHeaderData(2,Qt::Horizontal,"Function");
    m_framesModel->setHeaderData(3,Qt::Horizontal,"File");
    m_framesModel->setHeaderData(4,Qt::Horizontal,"Line");

    m_libraryModel = new QStandardItemModel(0,2,this);
    m_libraryModel->setHeaderData(0,Qt::Horizontal,"Id");
    m_libraryModel->setHeaderData(1,Qt::Horizontal,"Thread Groups");

    m_dlvInit = false;
    m_dlvExit = false;
    m_readDataBusy = false;
    m_writeDataBusy = false;

    m_headlessMode = true;

    m_headlessInitAddress = false;
    m_headlessProcess = new LiteProcess(m_liteApp,this);
    m_headlessProcess->setUseCtrlC(true);

    m_dlvRunningCmdList << "c" << "continue"
        << "n" << "next"
        << "s" << "step"
        << "si" << "step-instruction"
        << "stepout";

    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_process,SIGNAL(started()),this,SIGNAL(debugStarted()));
    connect(m_process,SIGNAL(finished(int)),this,SLOT(finished(int)));
    connect(m_process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
    connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readStdError()));
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdOutput()));

    connect(m_headlessProcess,SIGNAL(started()),this,SIGNAL(debugStarted()));
    connect(m_headlessProcess,SIGNAL(finished(int)),this,SLOT(headlessFinished(int)));
    connect(m_headlessProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(headlessError(QProcess::ProcessError)));
    connect(m_headlessProcess,SIGNAL(readyReadStandardError()),this,SLOT(headlessReadStdError()));
    connect(m_headlessProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(headlessReadStdOutput()));
}

DlvDebugger::~DlvDebugger()
{
    stop();
}

void DlvDebugger::appLoaded()
{
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
}

QString DlvDebugger::mimeType() const
{
    return QLatin1String("debugger/delve1");
}

QAbstractItemModel *DlvDebugger::debugModel(LiteApi::DEBUG_MODEL_TYPE type)
{
    if (type == LiteApi::ASYNC_MODEL) {
        return m_asyncModel;
    } else if (type == LiteApi::VARS_MODEL) {
        return m_varsModel;
    } else if (type == LiteApi::WATCHES_MODEL) {
        return m_watchModel;
    }else if (type == LiteApi::CALLSTACK_MODEL) {
        return m_framesModel;
    } else if (type == LiteApi::LIBRARY_MODEL) {
        return m_libraryModel;
    }
    return 0;
}

void DlvDebugger::setWorkingDirectory(const QString &dir)
{
    m_headlessProcess->setWorkingDirectory(dir);
    m_process->setWorkingDirectory(dir);
}

void DlvDebugger::setEnvironment (const QStringList &environment)
{
    m_headlessProcess->setEnvironment(environment);
    m_process->setEnvironment(environment);
}

bool DlvDebugger::start(const QString &cmd, const QString &arguments)
{
    if (!m_envManager) {
        return false;
    }

    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);

    QString goroot = env.value("GOROOT");
    if (!goroot.isEmpty()) {
        m_runtimeFilePath = QFileInfo(QDir(goroot),"src/pkg/runtime/").path();
    }

    QString dlv = FileUtil::lookupGoBin("dlv",m_liteApp,env,true);
    if (dlv.isEmpty()) {
        dlv = FileUtil::lookPath("dlv",env,false);
    }
    m_dlvFilePath = dlv;

    //m_checkFuncDecl = false;

    if (m_dlvFilePath.isEmpty()) {
        m_liteApp->appendLog("DlvDebugger","dlv was not found on system PATH (hint: is Delve installed?)",true);
        return false;
    }

    clear();

    if (m_headlessMode) {
        QStringList argsList;
        argsList << "--headless" << "--api-version=2" << "--accept-multiclient";
        argsList << "exec" << cmd;
        if (!arguments.isEmpty()) {
            argsList << "--" << arguments;
        }
#ifdef Q_OS_WIN
        //m_headlessProcess->setNativeArguments(argsList.join(" "));
        m_headlessProcess->startEx("\""+m_dlvFilePath+"\"", argsList.join(" "));
#else
        m_headlessProcess->startEx(m_dlvFilePath, argsList.join(" "));
#endif
        QString log = QString("%1 %2 [%3]").arg(m_dlvFilePath).arg(argsList.join(" ")).arg(m_headlessProcess->workingDirectory());
        emit debugLog(LiteApi::DebugRuntimeLog,log);
    } else {
        QStringList argsList;
        argsList << "exec" << cmd;
        if (!arguments.isEmpty()) {
            argsList << "--" << arguments;
        }
#ifdef Q_OS_WIN
        //m_process->setNativeArguments(argsList.join(" "));
        m_process->startEx("\""+m_dlvFilePath+"\"",argsList.join(" "));
#else
        m_process->startEx(m_dlvFilePath,argsList.join(" "));
#endif

        QString log = QString("%1 %2 [%3]").arg(m_dlvFilePath).arg(argsList.join(" ")).arg(m_process->workingDirectory());
        emit debugLog(LiteApi::DebugRuntimeLog,log);
    }

    return true;
}

void DlvDebugger::stop()
{
    if (m_dlvExit) {
        return;
    }
    m_dlvExit = true;
    if (m_headlessMode) {
        if (!m_headlessProcess->isStop()) {
            m_headlessProcess->interrupt();
        }
        if (!m_process->isStop()) {
            m_process->interrupt();
        }
        if (!m_headlessProcess->isStop() && !m_headlessProcess->waitForFinished(500)) {
            m_headlessProcess->kill();
        }
        if (!m_process->isStop() && !m_process->waitForFinished(500)) {
            command_helper("exit",true);
            if (!m_process->waitForFinished(500)) {
                m_process->kill();
            }
        }
    } else {
        if (!m_process->isStop()) {
            m_process->interrupt();
        }
        command_helper("exit",true);
        if (!m_process->isStop() && !m_process->waitForFinished(1000)) {
             m_process->kill();
        }
    }
}

bool DlvDebugger::isRunning()
{
    return m_process->state() != QProcess::NotRunning;
}

void DlvDebugger::continueRun()
{    
    command("continue");
}

void DlvDebugger::stepOver()
{
    command("next");
}

void DlvDebugger::stepInto()
{
    command("step");
}

void DlvDebugger::stepOut()
{
    command("stepout");
//    QString cmd = LiteApi::getGotools(m_liteApp);
//    QProcess process;
//    process.setEnvironment(LiteApi::getCurrentEnvironment(m_liteApp).toStringList());
//    QFileInfo info(m_lastFileName);
//    process.setWorkingDirectory(info.path());
//    QStringList args;
//    args << "finddecl" << "-file" << info.fileName() << "-line" << QString("%1").arg(m_lastFileLine+1);
//    process.start(cmd,args);
//    if (!process.waitForFinished(3000)) {
//        emit debugLog(LiteApi::DebugErrorLog,"error wait find decl process");
//        process.kill();
//        return;
//    }
//    if (process.exitCode() != 0) {
//        emit debugLog(LiteApi::DebugErrorLog,"error get find decl result");
//        return;
//    }
//    QByteArray data = process.readAll().trimmed();
//    QStringList ar = QString::fromUtf8(data).split(" ");
//    if (ar.size() != 4 || ar[0] != "func") {
//        emit debugLog(LiteApi::DebugErrorLog,"error find func decl in line");
//        return;
//    }
//    m_funcDecl.fileName = m_lastFileName;
//    m_funcDecl.funcName = ar[1];
//    m_funcDecl.start = ar[2].toInt()-1;
//    m_funcDecl.end = ar[3].toInt()-1;
//    m_checkFuncDecl = true;
//    command("next");
}

void DlvDebugger::runToLine(const QString &fileName, int line)
{
    bool find = findBreakPoint(fileName,line);
    if (!find) {
        insertBreakPoint(fileName,line);
        command("continue");
        removeBreakPoint(fileName,line);
    } else {
        command("continue");
    }
}

void DlvDebugger::createWatch(const QString &var)
{
    QString cmd = "vars "+QRegExp::escape(var);
    m_updateCmdHistroy.push_back(cmd);
    command_helper(cmd.toUtf8(),true);
}

void DlvDebugger::removeWatch(const QString &value)
{
    m_watchNameMap.remove(value);
    for (int i = 0; i < m_watchModel->rowCount(); i++) {
        QStandardItem *nameItem = m_watchModel->item(i,0);
        if (nameItem->text() == value) {
            m_watchModel->removeRow(i);
            break;
        }
    }
    emit watchRemoved(value);
}

void DlvDebugger::removeAllWatch()
{
    m_watchNameMap.clear();
    m_watchModel->removeRows(0,m_watchModel->rowCount());
}

void DlvDebugger::showFrame(QModelIndex index)
{
    QStandardItem* file = m_framesModel->item( index.row(), 3 );
    QStandardItem* line = m_framesModel->item( index.row(), 4 );
    if( !file || !line ) {
        return;
    }
    QString filename = file->text();
    int lineno = line->text().toInt();
    if( lineno <= 0 ) {
        return;
    }
    emit setFrameLine(filename, lineno - 1 );
}

void DlvDebugger::expandItem(QModelIndex index, LiteApi::DEBUG_MODEL_TYPE type)
{
    QStandardItem *parent = 0;
    if (type == LiteApi::VARS_MODEL) {
        parent = m_varsModel->itemFromIndex(index);
    } else if (type == LiteApi::WATCHES_MODEL) {
        parent = m_watchModel->itemFromIndex(index);
    }
    if (!parent) {
        return;
    }
    if (parent->data(VarExpanded).toInt() == 1) {
        return;
    }
    parent->setData(1,VarExpanded);
}

void DlvDebugger::setInitBreakTable(const QMultiMap<QString,int> &bks)
{
    m_initBks = bks;
}

void DlvDebugger::setInitWatchList(const QStringList &names)
{
    foreach (QString name, names) {
        m_watchNameMap.insert(name,"");
    }
}

void DlvDebugger::insertBreakPoint(const QString &fileName, int line)
{
    insertBreakPointHelper(fileName,line,false);
}

void DlvDebugger::insertBreakPointHelper(const QString &fileName, int line, bool force)
{
    line++;
    QString location = QString("%1:%2").arg(fileName).arg(line);
    if (m_locationBkMap.contains(location)) {
        return;
    }
    QString id = QString("bk%1").arg(qHash(location));
    m_locationBkMap.insert(location,id);
    QStringList args;
    args << "break";
    args << id;
    args << QString("%1:%2").arg(fileName).arg(line);
    command_helper(args.join(" ").toUtf8(),force);
}

void DlvDebugger::removeBreakPoint(const QString &fileName, int line)
{
    line++;
    QString location = QString("%1:%2").arg(fileName).arg(line);
    QString id = m_locationBkMap.value(location);
    if (id.isEmpty()) {
        return;
    }
    m_locationBkMap.remove(location);
    QStringList args;
    args << "clear";
    args << id;
    command_helper(args.join(" ").toUtf8(),false);
}

bool DlvDebugger::findBreakPoint(const QString &fileName, int line)
{
    QString location = QString("%1:%2").arg(fileName).arg(line);
    QString id = m_locationBkMap.value(location);
    return m_locationBkMap.contains(location);
}

void DlvDebugger::command_helper(const QByteArray &cmd, bool force)
{
    if (m_writeDataBusy && !force) {
        return;
    }
    m_writeDataBusy = true;
    m_lastCmd = cmd;

    if (m_dlvRunningCmdList.contains(cmd)) {
        m_asyncItem->removeRows(0,m_asyncItem->rowCount());
        m_asyncItem->setText("runing");
    }
#ifdef Q_OS_WIN
    m_process->write(cmd+"\r\n");
#else
    m_process->write(cmd+"\n");
#endif
}

void DlvDebugger::enterAppText(const QString &text)
{
    m_updateCmdList.clear();
    m_updateCmdHistroy.clear();

    QString cmd = text.trimmed();
    if (cmd == "r" || cmd == "restart") {
        m_processId.clear();
    }

    if (m_headlessMode) {
        m_headlessProcess->write(text.toUtf8());
    } else {
        m_process->write(text.toUtf8());
    }
}

void DlvDebugger::enterDebugText(const QString &text)
{
    m_updateCmdList.clear();
    m_updateCmdHistroy.clear();

    QString cmd = text.trimmed();
    if (cmd == "r" || cmd == "restart") {
        m_processId.clear();
    }

    command(text.toUtf8());
}

void  DlvDebugger::command(const QByteArray &cmd)
{
    command_helper(cmd,false);
}

void DlvDebugger::readStdError()
{
    //Process 4084 has exited with status 0
    QString data = QString::fromUtf8(m_process->readAllStandardError());
   // qDebug() << data << m_processId;
    //QRegExp reg;
    emit debugLog(LiteApi::DebugConsoleLog,data);
    foreach (QString line, data.split("\n",QString::SkipEmptyParts)) {
        if (line.startsWith("Process "+m_processId)) {
            m_processId.clear();
            this->stop();
        }
    }
}



//static bool isNameChar(char c)
//{
//    // could be 'stopped' or 'shlibs-added'
//    return (c >= 'a' && c <= 'z') || c == '-';
//}

void DlvDebugger::handleResponse(const QByteArray &buff)
{
    if (buff.isEmpty()) {
        return;
    }
    if (!m_headlessMode) {
        if (m_processId.isEmpty()) {
            //Process restarted with PID
            int n = buff.indexOf("PID");
            if (n != -1) {
                m_processId = buff.mid(n+3).replace("(dlv)","").trimmed();
            }
        }
    }
    //Process restarted with PID 4532
    //> main.main() H:/goproj/src/hello/main.go:13 (hits goroutine(1):1 total:1) (PC: 0x401172)
    //> main.main() H:/goproj/src/hello/main.go:14 (PC: 0x401179)
    //> main.main() H:/goproj/src/hello/main.go:21 (hits goroutine(1):1 total:1) (PC: 0x40161a)
    //> fmt.Println() c:/go/go1.6/src/fmt/print.go:263 (PC: 0x45aeca)
    //> runtime.convT2E() c:/go/go1.6/src/runtime/iface.go:128 (PC: 0x40caaa)"
    //> github.com/derekparker/delve/cmd/dlv/cmds.New() /src/github.com/derekparker/delve/cmd/dlv/cmds/commands.go:61 (PC: 0x45d09f)
    //> [bk6767010] main.test() H:/goproj/src/hello/main.go:12 (hits goroutine(1):1 total:1) (PC: 0x401066)
    //> [bk101903173] github.com/derekparker/delve/vendor/github.com/spf13/cobra.(*Command).Execute() github.com/derekparker/delve/vendor/github.com/spf13/cobra/command.go:615 (hits goroutine(1):1 total:1) (PC: 0x524ea6)
    //> qlang.io/qlang%2espec%2ev1.Import()
    //> main.main() goapi/_test/_testmain.go:50 (hits goroutine(1):1 total:1) (PC: 0x4011ca)
    if (buff.contains("> ")) {
        static QRegExp reg(">(\\s+\\[[\\w\\d]+\\])?\\s+([\\w\\d_\\.\\%\\*\\(\\)\\/]+)\\(\\)\\s+((?:[a-zA-Z]:)?[\\w\\d_\\s\\-\\/\\.\\\\]+):(\\d+)\\s?(.*)\\s?(\\(PC:\\s+.*)");
        int n = reg.indexIn(QString::fromUtf8(buff));
        if (n < 0) {
            return;
        }
        QString fileName = reg.cap(3);
        if (fileName.startsWith("./")) {
            fileName = QDir::cleanPath(m_process->workingDirectory()+"/"+fileName);
        }
        QString line = reg.cap(4);

        if (!fileName.isEmpty() && !line.isEmpty()) {
            bool ok = false;
            int n = line.toInt(&ok);
            if (ok) {
                m_lastFileName = fileName;
                m_lastFileLine = n-1;
                //check step out
                emit setCurrentLine(fileName,n-1);
            }
        }
        m_handleState.setStopped(true);

        m_asyncItem->removeRows(0,m_asyncItem->rowCount());
        m_asyncItem->setText("stopped");
        QString func = reg.cap(2).trimmed();
        //hack
        if (func.contains("%")) {
            func.replace("%2e",".");
        }
        QString hits = reg.cap(5).trimmed();
        QString pc = reg.cap(6).trimmed();
        int pos = pc.indexOf('\n');
        if (pos != -1) {
            pc.truncate(pos);
        }
        if (!hits.isEmpty()) {
            m_asyncItem->appendRow(new QStandardItem(hits));
        }
        m_asyncItem->appendRow(new QStandardItem(pc));
        m_asyncItem->appendRow(new QStandardItem("func="+func));
        m_asyncItem->appendRow(new QStandardItem("file="+fileName));
        m_asyncItem->appendRow(new QStandardItem("line="+line));
        emit setExpand(LiteApi::ASYNC_MODEL,m_asyncModel->indexFromItem(m_asyncItem),true);
    }
}

void DlvDebugger::cleanup()
{
    stop();
}

void DlvDebugger::clear()
{
    m_headlessInitAddress = false;
    m_lastFileLine = 0;
    m_lastFileName.clear();
    m_dlvInit = false;
    m_dlvExit = false;
    m_readDataBusy = false;
    m_writeDataBusy = false;
    m_handleState.clear();
    m_varNameMap.clear();
    m_watchNameMap.clear();
    m_watchList.clear();
    m_updateCmdHistroy.clear();
    m_nameItemMap.clear();
    m_varChangedItemList.clear();
    m_inbuffer.clear();
    m_locationBkMap.clear();
    m_cmdList.clear();
    m_framesModel->removeRows(0,m_framesModel->rowCount());
    m_libraryModel->removeRows(0,m_libraryModel->rowCount());
    m_varsModel->removeRows(0,m_varsModel->rowCount());
    m_watchModel->removeRows(0,m_watchModel->rowCount());
}

void DlvDebugger::initDebug()
{
    //get thread id
    m_processId.clear();

    if (!m_headlessMode) {
        command_helper("restart",true);
    }

    QMapIterator<QString,int> i(m_initBks);

    while (i.hasNext()) {
        i.next();
        QString fileName = i.key();
        QList<int> lines = m_initBks.values(fileName);
        foreach(int line, lines) {
            insertBreakPointHelper(fileName,line,true);
        }
    }
    if (m_liteApp->settings()->value(LITEDEBUG_AUTOBREAKMAIN,false).toBool()) {
        command_helper("break main.main",true);
    }
    command_helper("continue",true);

    emit debugLoaded();
}

static QString valueToolTip(const QString &value)
{
    int offset = 0;
    QString toolTip;
    QString text = value;
    text.replace(", ",",");
    for (int i = 0; i < text.size(); i++) {
//        if (text[i] == '[')  {
//            int j = i;
//            for (; j++; j < text.size()) {
//                if (text[j] == ']') {
//                    break;
//                }
//            }
//            toolTip += text.mid(i,j+1-i);
//            i = j;
//            continue;
//        }
        if (text[i] == '{') {
            if ( (i+1) < text.size() && text[i+1] == '}' ) {
                toolTip += "{}";
                i++;
            } else {
                offset++;
                toolTip += text[i];
                toolTip += "\n"+QString("\t").repeated(offset);
            }
        } else if (text[i] == '}') {
            offset--;
            toolTip += "\n"+QString("\t").repeated(offset);
            toolTip += text[i];
        } else if (text[i] == ',') {
            toolTip += text[i];
            int pos = text.lastIndexOf(QRegExp("\\{|\\[|\\]|\\}"),i-1);
            if (pos != -1 && text[pos] == '[') {
                continue;
            }
            toolTip += "\n"+QString("\t").repeated(offset);
        } else {
            toolTip += text[i];
        }
    }
    return toolTip;
}

void DlvDebugger::readStdOutput()
{
    QByteArray data = m_process->readAllStandardOutput();
    if (!m_dlvInit) {
        m_dlvInit = true;
        initDebug();
    }
    m_writeDataBusy = false;

    if (m_dlvExit) {
        return;
    }

    int newstart = 0;
    int scan = m_inbuffer.size();
    m_inbuffer.append(data);

    //hack check (dlv)
    static bool first_check = true;
    static bool dlv_check = false;
    if (first_check) {
        first_check = false;
        dlv_check = m_inbuffer.indexOf("(dlv)") != -1;
    }
    if (dlv_check && !m_inbuffer.endsWith("(dlv) ")) {
        return;
    }

    // This can trigger when a dialog starts a nested event loop.
    if (m_readDataBusy)
        return;
    QStringList dataList;
    while (newstart < m_inbuffer.size()) {
        int start = newstart;
        int end = m_inbuffer.indexOf('\n', scan);
        if (end < 0) {
            //m_inbuffer.remove(0, start);
            //return;
            end = m_inbuffer.size()-1;
        }
        newstart = end + 1;
        scan = newstart;
        if (end == start)
            continue;
#ifdef Q_OS_WIN
        if (m_inbuffer.at(end - 1) == '\r') {
            --end;
            if (end == start)
                continue;
        }
#endif
        m_readDataBusy = true;
        QByteArray data = QByteArray::fromRawData(m_inbuffer.constData() + start, end - start);
        dataList.append(QString::fromUtf8(data));
        handleResponse(data);
        m_readDataBusy = false;
    }

//    if (m_checkFuncDecl) {
//        if (m_lastFileName == m_funcDecl.fileName && m_lastFileLine >= m_funcDecl.start && m_lastFileLine <= m_funcDecl.end) {
//            command("next");
//            m_inbuffer.clear();
//            return;
//        }
//        m_checkFuncDecl = false;
//        m_funcDecl.clear();
//    }

    bool emitLog = true;
    if (!m_updateCmdHistroy.isEmpty()) {
        QString cmdHistroy = m_updateCmdHistroy.takeFirst();
        if (cmdHistroy == "stack") {
//            0  0x000000000040135a in main.main
//               at H:/goproj/src/hello/main.go:24
//            1  0x000000000042c629 in runtime.main
//               at c:/go/go1.6/src/runtime/proc.go:188
//            2  0x0000000000456560 in runtime.goexit
//               at c:/go/go1.6/src/runtime/asm_amd64.s:1998
             m_framesModel->removeRows(0,m_framesModel->rowCount());
             QString data = QString::fromUtf8(m_inbuffer);
             QStringList dataList = data.split("\n",QString::SkipEmptyParts);
             bool head = true;
             QList<QStandardItem*> items;
             foreach (QString data, dataList) {
                 if (head) {
                    // data.
                     items.clear();
                     QStringList ar = data.split(" ",QString::SkipEmptyParts);
                     if (ar.size() == 4) {
                         items << new QStandardItem(ar[0]);
                         items << new QStandardItem(ar[1]);
                         items << new QStandardItem(ar[3]);
                     }
                 } else {
                    data = data.trimmed();
                    if (data.startsWith("at")) {
                        data = data.mid(2).trimmed();
                        int n = data.lastIndexOf(":");
                        if (n > 0) {
                            items << new QStandardItem(data.left(n));
                            items << new QStandardItem(data.mid(n+1));
                            m_framesModel->appendRow(items);
                        }
                    }
                 }
                 head = !head;
             }
        } else if (cmdHistroy == "stack 0 -full") {
            // s = " \x04S\x00\x00\x00\x00\x00\x1d\x00\x00\x00\x00\x00\x00\x00"
            // v = []int len: 0, cap: 4257785, []"
            // args = []string len: 1, cap: 1, ["H:\\goproj\\src\\hello\\debug"]
            m_varsModel->removeRows(0,m_varsModel->rowCount());
            QString data = QString::fromUtf8(m_inbuffer);
            QStringList dataList = data.split("\n",QString::SkipEmptyParts);
            QMap<QString,QString> nameMap;
            foreach(QString text, dataList) {
                int n = text.indexOf("=");
                if (n == -1) {
                    continue;
                }
                QString name = text.left(n).trimmed();
                QString value = text.mid(n+1).trimmed();
                n = value.indexOf("(unreadable");
                if (n != -1) {
                    value = value.left(n)+"(unreadable ...";
                }
                nameMap.insert(name,value);
                QStandardItem *nameItem = new QStandardItem(name);
                QStandardItem *valueItem = new QStandardItem(value);
                valueItem->setToolTip(valueToolTip(value));
                QMap<QString,QString>::iterator it = m_varNameMap.find(name);
                if (it != m_varNameMap.end() && it.value() != value) {
#if QT_VERSION >= 0x050000
        valueItem->setData(QColor(Qt::red),Qt::TextColorRole);
#else
        valueItem->setData(Qt::red,Qt::TextColorRole);
#endif
                }
                m_varsModel->appendRow(QList<QStandardItem*>() << nameItem << valueItem);
            }
            m_varNameMap = nameMap;
        } else if (cmdHistroy.startsWith("vars ")) {
            foreach (QString data, QString::fromUtf8(m_inbuffer).split("\n",QString::SkipEmptyParts)) {
                int n = data.indexOf("=");
                if (n >= 0) {
                    QString name = data.left(n-1);
                    QString value = data.mid(n+1).trimmed();
                    if (name.isEmpty() || value.isEmpty() || name.contains(" ")) {
                        continue;
                    }

                    bool find = false;
                    for (int i = 0; i < m_watchModel->rowCount(); i++) {
                        QStandardItem *nameItem = m_watchModel->item(i,0);
                        QStandardItem *valueItem = m_watchModel->item(i,1);
                        if (nameItem->text() == name) {
                            find = true;
                            if (m_watchNameMap.value(name) == value) {
#if QT_VERSION >= 0x050000
                                valueItem->setData(QColor(Qt::black),Qt::TextColorRole);
#else
                                valueItem->setData(Qt::black,Qt::TextColorRole);
#endif
                            } else {
#if QT_VERSION >= 0x050000
                                valueItem->setData(QColor(Qt::red),Qt::TextColorRole);
#else
                                valueItem->setData(Qt::red,Qt::TextColorRole);
#endif
                                valueItem->setText(value);
                            }
                        }
                    }
                    if (!find) {
                        QStandardItem *nameItem = new QStandardItem(name);
                        nameItem->setData(name,VarNameRole);
                        QStandardItem *valueItem = new QStandardItem(value);
                        valueItem->setToolTip(valueToolTip(value));
                        m_watchModel->appendRow(QList<QStandardItem*>() << nameItem << valueItem );

                        emit watchCreated(name,name);
                    }
                    m_watchNameMap.insert(name,value);
                }
            }
        }
        emitLog = false;
    } else if (!m_headlessMode) {
         emit debugLog(LiteApi::DebugApplationLog,QString::fromUtf8(m_inbuffer));
    }
    if (emitLog) {
         emit debugLog(LiteApi::DebugConsoleLog,QString::fromUtf8(m_inbuffer));
    }
    m_inbuffer.clear();

    if (m_handleState.exited() && !m_dlvExit) {
        m_dlvExit = true;
        stop();
    } else if (m_handleState.stopped()) {
        m_updateCmdList.clear();
        m_updateCmdList << "stack" << "stack 0 -full";
        foreach (QString s, m_watchNameMap.keys()) {
            if (s.isEmpty()) {
                continue;
            }
            m_updateCmdList << "vars "+QRegExp::escape(s);
        }
    }

    m_handleState.clear();

    if (!m_updateCmdList.isEmpty()) {
        foreach(QString cmd, m_updateCmdList.takeFirst().split("|")) {
            m_updateCmdHistroy.push_back(cmd.trimmed());
            command(cmd.trimmed().toUtf8());
        }
    }
}

void DlvDebugger::finished(int code)
{
    emit debugStoped();
    emit debugLog(LiteApi::DebugRuntimeLog,QString("Dlv exited with code %1").arg(code));
    cleanup();
}

void DlvDebugger::error(QProcess::ProcessError err)
{
    emit debugStoped();
    emit debugLog(LiteApi::DebugRuntimeLog,QString("Dlv error! %1").arg(ProcessEx::processErrorText(err)));
    cleanup();
}

void DlvDebugger::readTty(const QByteArray &data)
{
    emit debugLog(LiteApi::DebugApplationLog,QString::fromUtf8(data));
}

void DlvDebugger::headlessReadStdError()
{
    QString data = QString::fromUtf8(m_headlessProcess->readAllStandardError());
    //qDebug() << data;
    emit debugLog(LiteApi::DebugErrorLog,data);
}

void DlvDebugger::headlessReadStdOutput()
{
    QString data = QString::fromUtf8(m_headlessProcess->readAllStandardOutput());
    //API server listening at: 127.0.0.1:54151
    if (!m_headlessInitAddress) {
        QString tmp = data.trimmed();
        QString addr;
        if (tmp.startsWith("API")) {
            int pos = tmp.lastIndexOf(" ");
            if (pos != -1) {
                addr = tmp.mid(pos+1);
                if (addr.indexOf(":") > 0) {
                    m_headlessInitAddress = true;
                }
            }
        }
        if (m_headlessInitAddress) {
            QStringList argsList;
            argsList << "connect" << addr;
#ifdef Q_OS_WIN
            m_process->setNativeArguments(argsList.join(" "));
            m_process->start("\""+m_dlvFilePath+"\"");
#else
            m_process->start(m_dlvFilePath + " " + argsList.join(" "));
#endif
            QString log = QString("%1 %2 [%3]").arg(m_dlvFilePath).arg(argsList.join(" ")).arg(m_process->workingDirectory());
           emit debugLog(LiteApi::DebugRuntimeLog,log);
        }
    }

    emit debugLog(LiteApi::DebugApplationLog,data);
}

void DlvDebugger::headlessFinished(int code)
{
    emit debugStoped();
    emit debugLog(LiteApi::DebugRuntimeLog,QString("Dlv server exited with code %1").arg(code));
    cleanup();
}

void DlvDebugger::headlessError(QProcess::ProcessError err)
{
    emit debugStoped();
    emit debugLog(LiteApi::DebugRuntimeLog,QString("Dlv server error! %1").arg(ProcessEx::processErrorText(err)));
    cleanup();
}
