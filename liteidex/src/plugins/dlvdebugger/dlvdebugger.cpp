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
// Module: dlvdebugger.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "dlvdebugger.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "dlvdebuggeroption.h"

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
    m_envManager(0),
    m_tty(0)
{
    m_process = new QProcess(this);
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

    m_watchModel = new QStandardItemModel(0,3,this);
    m_watchModel->setHeaderData(0,Qt::Horizontal,"Name");
    m_watchModel->setHeaderData(1,Qt::Horizontal,"Value");
    m_watchModel->setHeaderData(2,Qt::Horizontal,"Type");

    m_framesModel = new QStandardItemModel(0,5,this);
    m_framesModel->setHeaderData(0,Qt::Horizontal,"Level");
    m_framesModel->setHeaderData(1,Qt::Horizontal,"Address");
    m_framesModel->setHeaderData(2,Qt::Horizontal,"Function");
    m_framesModel->setHeaderData(3,Qt::Horizontal,"File");
    m_framesModel->setHeaderData(4,Qt::Horizontal,"Line");

    m_libraryModel = new QStandardItemModel(0,2,this);
    m_libraryModel->setHeaderData(0,Qt::Horizontal,"Id");
    m_libraryModel->setHeaderData(1,Qt::Horizontal,"Thread Groups");

    m_gdbinit = false;    
    m_gdbexit = false;

    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_process,SIGNAL(started()),this,SIGNAL(debugStarted()));
    connect(m_process,SIGNAL(finished(int)),this,SLOT(finished(int)));
    connect(m_process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
    connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readStdError()));
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdOutput()));
}

DlvDebugger::~DlvDebugger()
{
    if (m_process) {
         delete m_process;
    }
}

void DlvDebugger::appLoaded()
{
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
}

QString DlvDebugger::mimeType() const
{
    return QLatin1String("debugger/delve");
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
    m_process->setWorkingDirectory(dir);
}

void DlvDebugger::setEnvironment (const QStringList &environment)
{
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

    QString dlv = FileUtil::lookupGoBin("dlv",m_liteApp,true);
    if (dlv.isEmpty()) {
        dlv = FileUtil::lookPath("dlv",env,false);
    }
    m_dlvFilePath = dlv;
    m_lastFileLine = 0;
    m_lastFileName.clear();
    m_checkFuncDecl = false;

    if (m_dlvFilePath.isEmpty()) {
        m_liteApp->appendLog("DlvDebugger","dlv was not found on system PATH (hint: is GDB installed?)",true);
        return false;
    }
    QStringList argsList;
    argsList << "exec" << "\""+cmd+"\"";

    if (!arguments.isEmpty()) {
        argsList << "--" << arguments;
    }

    clear();
#ifdef Q_OS_WIN
    m_process->setNativeArguments(argsList.join(" "));
    m_process->start("\""+m_dlvFilePath+"\"");
#else
    m_process->start(m_gdbFilePath + " " + argsList.join(" "));
#endif

    QString log = QString("%1 %2 [%3]").arg(m_dlvFilePath).arg(argsList.join(" ")).arg(m_process->workingDirectory());
    emit debugLog(LiteApi::DebugRuntimeLog,log);

    return true;
}

void DlvDebugger::stop()
{
    command("exit");
    if (!m_process->waitForFinished(1000)) {
        m_process->kill();
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
    QString cmd = LiteApi::getGotools(m_liteApp);
    QProcess process;
    process.setEnvironment(LiteApi::getCurrentEnvironment(m_liteApp).toStringList());
    QFileInfo info(m_lastFileName);
    process.setWorkingDirectory(info.path());
    QStringList args;
    args << "finddecl" << "-file" << info.fileName() << "-line" << QString("%1").arg(m_lastFileLine+1);
    process.start(cmd,args);
    if (!process.waitForFinished(3000)) {
        emit debugLog(LiteApi::DebugErrorLog,"error wait find decl process");
        process.kill();
        return;
    }
    if (process.exitCode() != 0) {
        emit debugLog(LiteApi::DebugErrorLog,"error get find decl result");
        return;
    }
    QByteArray data = process.readAll().trimmed();
    QStringList ar = QString::fromUtf8(data).split(" ");
    if (ar.size() != 4 || ar[0] != "func") {
        emit debugLog(LiteApi::DebugErrorLog,"error find func decl in line");
        return;
    }
    m_funcDecl.fileName = m_lastFileName;
    m_funcDecl.funcName = ar[1];
    m_funcDecl.start = ar[2].toInt()-1;
    m_funcDecl.end = ar[3].toInt()-1;
    m_checkFuncDecl = true;
    command("next");
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

void DlvDebugger::createWatch(const QString &var, bool floating, bool watchModel)
{
    GdbCmd cmd;
    QStringList args;
    args << "-var-create";
    args << "-";
    if (floating) {
        args << "@";
    } else {
        args << "*";
    }
    args << var;
    cmd.setCmd(args);
    cmd.insert("var",var);
    if (watchModel) {
        cmd.insert("watchModel",true);
    }
    command(cmd);
}

void DlvDebugger::removeWatch(const QString &var, bool children)
{
    QString name = m_varNameMap.value(var);
    QStringList args;
    args << "-var-delete";
    if (children) {
        args << "-c";
    }
    args << name;
    GdbCmd cmd;
    cmd.setCmd(args);
    cmd.insert("var",var);
    cmd.insert("name",name);
    cmd.insert("children",children);
    command(cmd);
}

void DlvDebugger::removeWatchByName(const QString &name, bool children)
{
    QString var = m_varNameMap.key(name);
    QStringList args;
    args << "-var-delete";
    if (children) {
        args << "-c";
    }
    args << name;
    GdbCmd cmd;
    cmd.setCmd(args);
    cmd.insert("var",var);
    cmd.insert("name",name);
    cmd.insert("children",children);
    command(cmd);
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
    for (int i = 0; i < parent->rowCount(); i++) {
        QStandardItem *item = parent->child(i);
        QString name = item->data(VarNameRole).toString();
        int num = item->data(VarNumChildRole).toInt();
        if (num > 0) {
            updateVarListChildren(name);
        }
    }
}

void DlvDebugger::setInitBreakTable(const QMultiMap<QString,int> &bks)
{
    m_initBks = bks;
}

void DlvDebugger::insertBreakPoint(const QString &fileName, int line)
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
    GdbCmd cmd;
    cmd.setCmd(args);
    command(cmd);
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
    GdbCmd cmd;
    cmd.setCmd(args);
    command(cmd);
}

bool DlvDebugger::findBreakPoint(const QString &fileName, int line)
{
    QString location = QString("%1:%2").arg(fileName).arg(line);
    QString id = m_locationBkMap.value(location);
    return m_locationBkMap.contains(location);
}

void DlvDebugger::command_helper(const GdbCmd &cmd, bool emitOut)
{
    m_token++;
    QByteArray buf = cmd.makeCmd(m_token);
    m_lastCmd = buf;
    if (emitOut) {
     //   emit debugLog(LiteApi::DebugConsoleLog,">>> "+QString::fromUtf8(buf));
    }
#ifdef Q_OS_WIN
    buf.append("\r\n");
#else
    buf.append("\n");
#endif
    m_tokenCookieMap.insert(m_token,cmd.cookie());
    m_process->write(buf);
}

void DlvDebugger::command(const GdbCmd &cmd)
{
    command_helper(cmd,true);
}

void DlvDebugger::enterAppText(const QString &text)
{
    m_updateCmdList.clear();
    m_updateCmd.clear();

    QString cmd = text.trimmed();
    if (cmd == "r" || cmd == "restart") {
        m_processId.clear();
    }

    if (m_tty) {
        m_tty->write(text.toUtf8());
    } else {
        m_process->write(text.toUtf8());
    }
}

void DlvDebugger::enterDebugText(const QString &text)
{
    m_updateCmdList.clear();
    m_updateCmd.clear();

    QString cmd = text.trimmed();
    if (cmd == "r" || cmd == "restart") {
        m_processId.clear();
    }

    command(text);
}

void  DlvDebugger::command(const QByteArray &cmd)
{
    command_helper(GdbCmd(cmd),false);
}

void DlvDebugger::readStdError()
{
    //Process 4084 has exited with status 0
    QString data = QString::fromUtf8(m_process->readAllStandardError());
    //QRegExp reg;
    emit debugLog(LiteApi::DebugErrorLog,data);
    foreach (QString line, data.split("\n",QString::SkipEmptyParts)) {
        if (line.startsWith("Process "+m_processId)) {
            m_processId.clear();
            this->stop();
        }
    }
}



static bool isNameChar(char c)
{
    // could be 'stopped' or 'shlibs-added'
    return (c >= 'a' && c <= 'z') || c == '-';
}

/*
27.4.2 gdb/mi Output Syntax

The output from gdb/mi consists of zero or more out-of-band records followed, optionally,
by a single result record. This result record is for the most recent command. The sequence
of output records is terminated by (gdb).
If an input command was prefixed with a token then the corresponding output for that
command will also be prefixed by that same token.

If an input command was prefixed with a token then the corresponding output for that
command will also be prefixed by that same token.

output -> ( out-of-band-record )* [ result-record ] "(gdb)" nl
result-record ->
[ token ] "^" result-class ( "," result )* nl
out-of-band-record ->
async-record | stream-record
async-record ->
exec-async-output | status-async-output | notify-async-output
exec-async-output ->
[ token ] "*" async-output
status-async-output ->
[ token ] "+" async-output
notify-async-output ->
[ token ] "=" async-output
async-output ->
async-class ( "," result )* nl
result-class ->
"done" | "running" | "connected" | "error" | "exit"
async-class ->
"stopped" | others (where others will be added depending on the needs¡ªthis
is still in development).
result -> variable "=" value
variable ->
string
value -> const | tuple | list
const -> c-string
tuple -> "{}" | "{" result ( "," result )* "}"
list -> "[]" | "[" value ( "," value )* "]" | "[" result ( "," result )* "]"
stream-record ->
console-stream-output | target-stream-output | log-stream-output
console-stream-output ->
"~" c-string
target-stream-output ->
"@" c-string
log-stream-output ->
"&" c-string
nl -> CR | CR-LF
*/

void DlvDebugger::handleResponse(const QByteArray &buff)
{
    if (buff.isEmpty()) {
        return;
    }
    if (m_processId.isEmpty()) {
        //Process restarted with PID
        int n = buff.indexOf("PID");
        if (n != -1) {
            m_processId = buff.mid(n+3).trimmed();
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
    if (buff.contains("> ")) {
        static QRegExp reg(">(\\s+\\[[\\w\\d]+\\])?\\s+([\\w\\d_\\.\\*\\(\\)\\/]+)\\(\\)\\s+((?:[a-zA-Z]:)?[\\w\\d_\\s\\-\\/\\.\\\\]+):(\\d+)\\s?(.*)\\s?(\\(PC:\\s+.*)");
        int n = reg.indexIn(QString::fromUtf8(buff));
        if (n < 0) {
            return;
        }
        QString fileName = reg.cap(3);
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
    return;


    if (buff.isEmpty() || buff == "(gdb) ")
        return;

    const char *from = buff.constData();
    const char *to = from + buff.size();
    const char *inner;

    int token = -1;
    // Token is a sequence of numbers.
    for (inner = from; inner != to; ++inner)
        if (*inner < '0' || *inner > '9')
            break;
    if (from != inner) {
        token = QByteArray(from, inner - from).toInt();
        from = inner;
    }
    // Next char decides kind of response.
    const char c = *from++;
    switch (c) {
    case '*':
    case '+':
    case '=':
    {
        QByteArray asyncClass;
        for (; from != to; ++from) {
            const char c = *from;
            if (!isNameChar(c))
                break;
            asyncClass += *from;
        }
        GdbMiValue result;
        while (from != to) {
            GdbMiValue data;
            if (*from != ',') {
                // happens on archer where we get
                // 23^running <NL> *running,thread-id="all" <NL> (gdb)
                result.m_type = GdbMiValue::Tuple;
                break;
            }
            ++from; // skip ','
            data.parseResultOrValue(from, to);
            if (data.isValid()) {
                //qDebug() << "parsed result:" << data.toString();
                result.m_children += data;
                result.m_type = GdbMiValue::Tuple;
            }
        }
        handleAsyncClass(asyncClass,result);
        break;
    }
    case '~':
        handleConsoleStream(GdbMiValue::parseCString(from, to));
        break;
    case '@':
        handleTargetStream(GdbMiValue::parseCString(from, to));
        break;
    case '&':
        handleLogStream(GdbMiValue::parseCString(from, to));
        break;
    case '^': {
        GdbResponse response;

        response.token = token;

        for (inner = from; inner != to; ++inner)
            if (*inner < 'a' || *inner > 'z')
                break;

        QByteArray resultClass = QByteArray::fromRawData(from, inner - from);
        if (resultClass == "done") {
            response.resultClass = GdbResultDone;
        } else if (resultClass == "running") {
            response.resultClass = GdbResultRunning;
        } else if (resultClass == "connected") {
            response.resultClass = GdbResultConnected;
        } else if (resultClass == "error") {
            response.resultClass = GdbResultError;
        } else if (resultClass == "exit") {
            response.resultClass = GdbResultExit;
        } else {
            response.resultClass = GdbResultUnknown;
        }

        from = inner;
        if (from != to) {
            if (*from == ',') {
                ++from;
                response.data.parseTuple_helper(from, to);
                response.data.m_type = GdbMiValue::Tuple;
                response.data.m_name = "data";
            } else {
                // Archer has this.
                response.data.m_type = GdbMiValue::Tuple;
                response.data.m_name = "data";
            }
        }
        if (m_tokenCookieMap.contains(token)) {
            response.cookie = m_tokenCookieMap.take(token);
        }
        handleResultRecord(response);
        break;
    }
    default: {
        from--;
        QByteArray out(from,to-from);
        out.append("\n");
        emit debugLog(LiteApi::DebugApplationLog,QString::fromUtf8(out));
        break;
    }
    }
}

void DlvDebugger::handleStopped(const GdbMiValue &result)
{
    QByteArray reason = result.findChild("reason").data();
    m_handleState.setReason(reason);
    m_handleState.setStopped(true);
    if (reason.startsWith("exited")) {
        m_handleState.setExited(true);
        m_handleState.setReason(reason);
        return;
    }
    GdbMiValue frame = result.findChild("frame");
    if (frame.isValid()) {
        QString fullname = frame.findChild("fullname").data();
        QString file = frame.findChild("file").data();
        QString line = frame.findChild("line").data();
        if (!fullname.isEmpty()) {
            emit setCurrentLine(fullname,line.toInt()-1);
        } else if (!file.isEmpty()) {
            //fix go build bug, not find fullname
            //file="C:/Users/ADMINI~1/AppData/Local/Temp/2/bindist308287094/go/src/pkg/fmt/print.go"
            int i = file.indexOf("/go/src/pkg");
            if (i > 0) {
                QString fullname = LiteApi::getGOROOT(m_liteApp)+file.right(file.length()-i-3);
                emit setCurrentLine(fullname,line.toInt()-1);
            }
        }
    }
}

void DlvDebugger::handleLibrary(const GdbMiValue &result)
{
    QString id = result.findChild("id").data();
    QString thread_group = result.findChild("thread-group").data();
    m_libraryModel->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(id)
                              << new QStandardItem(thread_group)
                              );
}

void DlvDebugger::handleAsyncClass(const QByteArray &asyncClass, const GdbMiValue &result)
{
    m_asyncItem->removeRows(0,m_asyncItem->rowCount());
    m_asyncItem->setText(asyncClass);
    //QStandardItem *item = new QStandardItem(QString(asyncClass));
    GdbMiValueToItem(m_asyncItem,result);
    //m_asyncModel->clear();
    //m_asyncModel->appendRow(item);
    if (asyncClass == "stopped") {
        handleStopped(result);        
    } else if (asyncClass == "library-loaded") {
        handleLibrary(result);
    }
    emit setExpand(LiteApi::ASYNC_MODEL,m_asyncModel->indexFromItem(m_asyncItem),true);
}

void DlvDebugger::handleConsoleStream(const QByteArray&)
{

}

void DlvDebugger::handleTargetStream(const QByteArray&)
{

}

void DlvDebugger::handleLogStream(const QByteArray&)
{

}

void DlvDebugger::handleResultStackListFrame(const GdbResponse &response, QMap<QString,QVariant>&)
{
    //10000015^done,stack=[frame={level="0",addr="0x0040113f",func="main.main",file="F:/hg/debug_test/hello/main.go",fullname="F:/hg/debug_test/hello/main.go",line="36"},frame={level="1",addr="0x00401f8a",func="runtime.mainstart",file="386/asm.s",fullname="c:/go/src/pkg/runtime/386/asm.s",line="96"},frame={level="2",addr="0x0040bcfe",func="runtime.initdone",file="/go/src/pkg/runtime/proc.c",fullname="c:/go/src/pkg/runtime/proc.c",line="242"},frame={level="3",addr="0x00000000",func="??"}]
    m_framesModel->removeRows(0,m_framesModel->rowCount());
    if (response.resultClass != GdbResultDone) {
        return;
    }
    GdbMiValue stack = response.data.findChild("stack");
    if (stack.isList()) {
        for (int i = 0; i < stack.childCount(); i++) {
            GdbMiValue child = stack.childAt(i);
            if (child.isValid() && child.name() == "frame") {
                QString level = child.findChild("level").data();
                QString addr = child.findChild("addr").data();
                QString func = child.findChild("func").data();
                QString file = child.findChild("file").data();
                QString line = child.findChild("line").data();
                m_framesModel->appendRow(QList<QStandardItem*>()
                                         << new QStandardItem(level)
                                         << new QStandardItem(addr)
                                         << new QStandardItem(func)
                                         << new QStandardItem(file)
                                         << new QStandardItem(line)
                                         );
            }
        }
    }
}

void DlvDebugger::handleResultStackListVariables(const GdbResponse &response, QMap<QString,QVariant>&)
{
    //10000014^done,variables=[{name="v"},{name="x"},{name="pt"},{name="str"},{name="sum1"},{name="y"}]
    if (response.resultClass != GdbResultDone) {
        return;
    }
    GdbMiValue vars = response.data.findChild("variables");
    if (vars.isList()) {
        foreach (const GdbMiValue &child, vars.m_children) {
            if (child.isValid()) {
                QString var = child.findChild("name").data();
                if (!m_varNameMap.contains(var)) {
                    createWatch(var,true,false);
                }
            }
        }
    }
}

void DlvDebugger::handleResultVarCreate(const GdbResponse &response, QMap<QString,QVariant> &map)
{
    //10000018^done,name="var4",numchild="0",value="4265530",type="int",thread-id="1",has_more="0"
    //10000019^done,name="var5",numchild="2",value="{...}",type="struct string",thread-id="1",has_more="0"
    //10000020^done,name="var6",numchild="3",value="0x40bc38",type="struct main.pt *",thread-id="1",has_more="0"
    if (response.resultClass != GdbResultDone) {
        return;
    }
    QString name = response.data.findChild("name").data();
    QString numchild = response.data.findChild("numchild").data();
    QString value = response.data.findChild("value").data();
    QString type = response.data.findChild("type").data();
    QString var = map.value("var").toString();
    if (var.isEmpty()) {
        var = map.value("cmdList").toStringList().last();
    }
    if (m_varNameMap.contains(var)) {
        var += QString("-%1").arg(response.token);
    }
    m_varNameMap.insert(var,name);
    QStandardItem *item = new QStandardItem(var);
    item->setData(name,VarNameRole);
    m_nameItemMap.insert(name,item);
    if (map.value("watchModel",false).toBool()) {
        emit watchCreated(name,map.value("var").toString());
        m_watchList.append(name);
        m_watchModel->appendRow(QList<QStandardItem*>()
                               << item
                               << new QStandardItem(value)
                               << new QStandardItem(type)
                               );
    } else {
        m_varsModel->appendRow(QList<QStandardItem*>()
                               << item
                               << new QStandardItem(value)
                               << new QStandardItem(type)
                               );
    }
    int num = numchild.toInt();
    item->setData(num,VarNumChildRole);
    if (num > 0 ){
        updateVarListChildren(name);
    }
}

void DlvDebugger::handleResultVarListChildren(const GdbResponse &response, QMap<QString,QVariant> &map)
{
    //10000022^done,numchild="2",children=[child={name="var5.str",exp="str",numchild="1",value="0x4115c6 \"\\203\\304\\b\\303d\\213\\r,\"",type="uint8 *",thread-id="1"},child={name="var5.len",exp="len",numchild="0",value="4242460",type="int",thread-id="1"}],has_more="0"
    if (response.resultClass != GdbResultDone) {
        return;
    }
    GdbMiValue children = response.data.findChild("children");
    if (children.isList()) {
        QString name = map.value("name").toString();
        QStandardItem *parent = m_nameItemMap.value(name);
        if (parent == 0) {
            return;
        }
        int num = response.data.findChild("numchild").data().toInt();
        parent->setData(num,VarNumChildRole);
        for (int i = 0; i < children.childCount(); i++) {
            GdbMiValue child = children.childAt(i);
            if (child.name() == "child" && child.isTuple()) {
                QString name = child.findChild("name").data();
                QString exp = child.findChild("exp").data();
                QString numchild = response.data.findChild("numchild").data();
                QString value = child.findChild("value").data();
                QString type = child.findChild("type").data();
                QStandardItem *item = new QStandardItem(exp);
                item->setData(name,VarNameRole);
                m_nameItemMap.insert(name,item);
                parent->appendRow(QList<QStandardItem*>()
                                  << item
                                  << new QStandardItem(value)
                                  << new QStandardItem(type)
                                  );
                int num = numchild.toInt();
                item->setData(num,VarNumChildRole);
            }
        }
    }
}

void DlvDebugger::handleResultVarUpdate(const GdbResponse &response, QMap<QString,QVariant>&)
{
    //10000040^done,changelist=[{name="var2",in_scope="true",type_changed="false",has_more="0"}]
    if (response.resultClass != GdbResultDone) {
        return;
    }
    GdbMiValue list = response.data.findChild("changelist");
    if (list.isList()) {
        for (int i = 0; i < list.childCount(); i++) {
            GdbMiValue child = list.childAt(i);
            if (child.isValid()) {
                QString name = child.findChild("name").data();
                QString in_scope = child.findChild("in_scope").data();
                QString type_changed = child.findChild("type_changed").data();
                QString var = m_varNameMap.key(name);
                if (in_scope == "false") {
                    removeWatch(var,false);
                } else {
                    if (type_changed == "true") {
                        //remove watch children
                        removeWatch(var,true);
                        //update type
                        updateVarTypeInfo(name);
                        //udpate children
                        updateVarListChildren(name);
                    }
                    //update value
                    updateVarValue(name);
                }
            }
        }
    }
}

void DlvDebugger::handleResultVarDelete(const GdbResponse &response, QMap<QString,QVariant> &map)
{
    //10000062^done,ndeleted="1"
    //10000063^done,ndeleted="0"
    if (response.resultClass != GdbResultDone) {
        return;
    }
    bool ndeleted = true;
    if (response.data.findChild("ndeleted").data() == "0") {
        ndeleted = false;
    }
    QString var = map.value("var").toString();
    QString name = map.value("name").toString();
    QMutableMapIterator<QString,QStandardItem*> i(m_nameItemMap);
    QString cls = name+".";
    while (i.hasNext()) {
        i.next();
        if (i.key().startsWith(cls)) {
            i.remove();
        }
    }

    QStandardItemModel *model = m_varsModel;
    if (m_watchList.contains(name)) {
        emit watchRemoved(name);
        m_watchList.removeAll(name);
        model = m_watchModel;
        ndeleted = 1;
    }
    if (ndeleted) {
        m_varNameMap.remove(var);
        m_nameItemMap.remove(name);
    }
    for (int i = 0; i < model->rowCount(); i++) {
        QStandardItem *item = model->item(i,0);
        if (item->data() == name) {
            if (ndeleted) {
                model->removeRow(i);
            } else {
                item->removeRows(0,item->rowCount());
                item->setData(0,VarExpanded);
                emit setExpand(LiteApi::VARS_MODEL,model->indexFromItem(item),false);
            }
            break;
        }
    }
}

void DlvDebugger::handleResultVarUpdateValue(const GdbResponse &response, QMap<QString,QVariant> &map)
{
    //10000035^done,value="100"
    if (response.resultClass != GdbResultDone) {
        return;
    }
    QString value = response.data.findChild("value").data();
    QString name = map.value("name").toString();
    QStandardItem *item = m_nameItemMap.value(name);
    if (!item) {
        return;
    }
    QStandardItem *parent = item->parent();
    QStandardItem *v = 0;
    if (parent) {
        v = parent->child(item->row(),1);
    } else {
        v = item->model()->item(item->row(),1);
    }
    if (v) {
        v->setData(value,Qt::DisplayRole);
#if QT_VERSION >= 0x050000
        v->setData(QColor(Qt::red),Qt::TextColorRole);
#else
        v->setData(Qt::red,Qt::TextColorRole);
#endif
        m_varChangedItemList.insert(v);
    }
}

void DlvDebugger::handleResultVarUpdateType(const GdbResponse &response, QMap<QString,QVariant> &map)
{
    //10000060^done,type="struct string"
    if (response.resultClass != GdbResultDone) {
        return;
    }
    QString type = response.data.findChild("type").data();
    QString name = map.value("name").toString();
    QStandardItem *item = m_nameItemMap.value(name);
    if (!item) {
        return;
    }
    QStandardItem *parent = item->parent();
    QStandardItem *v = 0;
    if (parent) {
        v = parent->child(item->row(),2);
    } else {
        v = item->model()->item(item->row(),2);
    }
    if (v) {
        v->setData(type,Qt::DisplayRole);
#if QT_VERSION >= 0x050000
        v->setData(QColor(Qt::red),Qt::TextColorRole);
#else
        v->setData(Qt::red,Qt::TextColorRole);
#endif
        m_varChangedItemList.insert(v);
    }
}

void DlvDebugger::handleBreakInsert(const GdbResponse &response, QMap<QString,QVariant>&)
{
// >>> 10000029-break-insert F:/hg/debug_test/hello/main.go:31
// 10000029^done,bkpt={number="2",type="breakpoint",disp="keep",enabled="y",addr="0x004010dd",func="main.test",file="F:/hg/debug_test/hello/main.go",fullname="F:/hg/debug_test/hello/main.go",line="31",times="0",original-location="F:/hg/debug_test/hello/main.go:31"}

// >>> 10000046-break-insert F:/hg/debug_test/hello/main.go:37
// 10000046^done,bkpt={number="3",type="breakpoint",disp="keep",enabled="y",addr="0x0040118a",func="main.main",file="F:/hg/debug_test/hello/main.go",fullname="F:/hg/debug_test/hello/main.go",line="37",times="0",original-location="F:/hg/debug_test/hello/main.go:37"}
    if (response.resultClass != GdbResultDone) {
        return;
    }
    GdbMiValue bkpt = response.data.findChild("bkpt");
    if (bkpt.isTuple()) {
        QString number = bkpt.findChild("number").data();
        QString org_location= bkpt.findChild("original-location").data();
        m_locationBkMap.insert(number,org_location);
    }
}

void DlvDebugger::handleBreakDelete(const GdbResponse &response, QMap<QString,QVariant> &map)
{
    if (response.resultClass != GdbResultDone) {
        return;
    }
    QStringList cmdList = map.value("cmdList").toStringList();
    if (cmdList.size() != 2) {
        return;
    }
    QString number = cmdList.at(1);
    m_locationBkMap.remove(number);
}

void DlvDebugger::handleResultRecord(const GdbResponse &response)
{
    if (response.cookie.type() != QVariant::Map) {
        return;
    }
    QMap<QString,QVariant> map = response.cookie.toMap();
    QString cmd = map.value("cmd").toString();
    if (cmd.isEmpty()) {
        return;
    }
    QStringList cmdList = map.value("cmdList").toStringList();
    if (cmdList.isEmpty()) {
        return;
    }
    if (cmdList.at(0) == "-stack-list-variables") {
        handleResultStackListVariables(response,map);
    } else if (cmdList.at(0) == "-stack-list-frames") {
        handleResultStackListFrame(response,map);
    } else if (cmdList.at(0) == "-var-create") {
        handleResultVarCreate(response,map);
    }  else if (cmdList.at(0) == "-var-list-children") {
        handleResultVarListChildren(response,map);
    } else if (cmdList.at(0) == "-var-update") {
        handleResultVarUpdate(response,map);
    } else if (cmdList.at(0) == "-var-delete") {
        handleResultVarDelete(response,map);
    } else if (cmdList.at(0) == "-var-evaluate-expression") {
        handleResultVarUpdateValue(response,map);
    } else if (cmdList.at(0) == "-var-info-type") {
        handleResultVarUpdateType(response,map);
    } else if (cmdList.at(0) == "-break-insert") {
        handleBreakInsert(response,map);
    } else if (cmdList.at(0) == "-break-delete") {
        handleBreakDelete(response,map);
    }
}

void DlvDebugger::clear()
{
    m_gdbinit = false;
    m_gdbexit = false;
    m_busy = false;
    m_token = 10000000;
    m_handleState.clear();
    m_varNameMap.clear();
    m_watchList.clear();
    m_nameItemMap.clear();
    m_tokenCookieMap.clear();
    m_varChangedItemList.clear();
    m_inbuffer.clear();
    m_locationBkMap.clear();
    m_framesModel->removeRows(0,m_framesModel->rowCount());
    m_libraryModel->removeRows(0,m_libraryModel->rowCount());
    m_varsModel->removeRows(0,m_varsModel->rowCount());
    m_watchModel->removeRows(0,m_watchModel->rowCount());
}

void DlvDebugger::initDebug()
{
    /*
    command("set unwindonsignal on");
    command("set overload-resolution off");
    command("handle SIGSEGV nopass stop print");
    command("set breakpoint pending on");
    command("set width 0");
    command("set height 0");
    command("set auto-solib-add on");
    if (!m_runtimeFilePath.isEmpty()) {
#ifdef Q_OS_WIN
        QStringList pathList = LiteApi::getGOPATH(m_liteApp,false);
        QString paths;
        foreach(QString path, pathList) {
            paths += QDir::fromNativeSeparators(path)+"/src";
            paths += ";";
        }

        command("-environment-directory "+m_runtimeFilePath.toLatin1());
        //command("-environment-directory "+m_runtimeFilePath.toLatin1()+";"+paths.toLatin1());
        command("set substitute-path /go/src/pkg/runtime "+m_runtimeFilePath.toLatin1());
#else
        command("-environment-directory "+m_runtimeFilePath.toUtf8());
        command("set substitute-path /go/src/pkg/runtime "+m_runtimeFilePath.toUtf8());
#endif
    }
    //command("set ");
    */
    //get thread id
    m_processId.clear();
    command("restart");
    QMapIterator<QString,int> i(m_initBks);
    while (i.hasNext()) {
        i.next();
        QString fileName = i.key();
        QList<int> lines = m_initBks.values(fileName);
        foreach(int line, lines) {
            insertBreakPoint(fileName,line);
        }
    }
    command("break main.main");
    command("continue");
    emit debugLoaded();
}

void DlvDebugger::updateWatch()
{
    foreach(QStandardItem *item, m_varChangedItemList) {
#if QT_VERSION >= 0x050000
        item->setData(QColor(Qt::black),Qt::TextColorRole);
#else
        item->setData(Qt::black,Qt::TextColorRole);
#endif
    }
    m_varChangedItemList.clear();
    command("-var-update *");
}

void DlvDebugger::updateLocals()
{
    command("locals");
    command("args");
}

void DlvDebugger::updateFrames()
{
    command("stack");
}

void DlvDebugger::updateBreaks()
{
    command("-break-info");
}

void DlvDebugger::updateVarTypeInfo(const QString &name)
{
    QStringList args;
    args << "-var-info-type";
    args << name;
    GdbCmd cmd;
    cmd.setCmd(args);
    cmd.insert("name",name);
    command(cmd);
}

void DlvDebugger::updateVarListChildren(const QString &name)
{
    GdbCmd cmd;
    QStringList args;
    args << "-var-list-children";
    args << "1";
    args << name;
    cmd.setCmd(args);
    cmd.insert("name",name);
    command(cmd);
}

void DlvDebugger::updateVarValue(const QString &name)
{
    QStringList args;
    args << "-var-evaluate-expression";
    args << name;
    GdbCmd cmd;
    cmd.setCmd(args);
    cmd.insert("name",name);
    command(cmd);
}

void DlvDebugger::readStdOutput()
{
    int newstart = 0;
    int scan = m_inbuffer.size();
    m_inbuffer.append(m_process->readAllStandardOutput());

    // This can trigger when a dialog starts a nested event loop.
    if (m_busy)
        return;
    QStringList dataList;
    while (newstart < m_inbuffer.size()) {
        int start = newstart;
        int end = m_inbuffer.indexOf('\n', scan);
        if (end < 0) {
            m_inbuffer.remove(0, start);
            return;
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
        m_busy = true;
        QByteArray data = QByteArray::fromRawData(m_inbuffer.constData() + start, end - start);
        dataList.append(QString::fromUtf8(data));
        handleResponse(data);
        m_busy = false;
    }

    if (m_checkFuncDecl) {
        if (m_lastFileName == m_funcDecl.fileName && m_lastFileLine >= m_funcDecl.start && m_lastFileLine <= m_funcDecl.end) {
            command("next");
            m_inbuffer.clear();
            return;
        }
        m_checkFuncDecl = false;
        m_funcDecl.clear();
    }

    bool emitLog = true;
    if (m_updateCmd == m_lastCmd) {
        if (m_updateCmd == "stack") {
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
        } else if (m_updateCmd == "stack 0 -full") {
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
        }
        m_updateCmd.clear();
        emitLog = false;
    }
    if (emitLog) {
        emit debugLog(LiteApi::DebugConsoleLog,QString::fromUtf8(m_inbuffer));
    }
    m_inbuffer.clear();

    if (!m_gdbinit) {
        m_gdbinit = true;
        initDebug();
    }

    if (m_handleState.exited() && !m_gdbexit) {
        m_gdbexit = true;
        stop();
    } else if (m_handleState.stopped()) {
        m_updateCmdList.clear();
        m_updateCmdList << "stack" << "stack 0 -full";
    }

    m_handleState.clear();

    if (!m_updateCmdList.isEmpty()) {
        foreach(QString cmd, m_updateCmdList.takeFirst().split("|")) {
            command(cmd.trimmed());
            m_updateCmd = cmd;
        }
    }
}

void DlvDebugger::finished(int code)
{
    clear();
    if (m_tty) {
        m_tty->shutdown();
    }
    emit debugStoped();
    emit debugLog(LiteApi::DebugRuntimeLog,QString("Program exited with code %1").arg(code));
}

void DlvDebugger::error(QProcess::ProcessError err)
{
    clear();
    if (m_tty) {
        m_tty->shutdown();
    }
    emit debugStoped();
    emit debugLog(LiteApi::DebugRuntimeLog,QString("Error! %1").arg(ProcessEx::processErrorText(err)));
}

void DlvDebugger::readTty(const QByteArray &data)
{
    emit debugLog(LiteApi::DebugApplationLog,QString::fromUtf8(data));
}
