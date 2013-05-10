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
// Module: gdbdebugger.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "gdbdebugger.h"
#include "fileutil/fileutil.h"

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

GdbDebugger::GdbDebugger(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IDebugger(parent),
    m_liteApp(app),
    m_envManager(0)
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
    m_varsModel = new QStandardItemModel(0,3,this);
    m_varsModel->setHeaderData(0,Qt::Horizontal,"Name");
    m_varsModel->setHeaderData(1,Qt::Horizontal,"Value");
    m_varsModel->setHeaderData(2,Qt::Horizontal,"Type");

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
    connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readStdError()));
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdOutput()));
}

GdbDebugger::~GdbDebugger()
{
    if (m_process) {
         delete m_process;
    }
}

void GdbDebugger::appLoaded()
{
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
}

QString GdbDebugger::mimeType() const
{
    return QLatin1String("debuger/gdb");
}

QAbstractItemModel *GdbDebugger::debugModel(LiteApi::DEBUG_MODEL_TYPE type)
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

void GdbDebugger::setWorkingDirectory(const QString &dir)
{
    m_process->setWorkingDirectory(dir);
}

void GdbDebugger::setEnvironment (const QStringList &environment)
{
    m_process->setEnvironment(environment);
}

bool GdbDebugger::start(const QString &program, const QString &arguments)
{
    if (!m_envManager) {
        return false;
    }

    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);

    QString goroot = env.value("GOROOT");
    if (!goroot.isEmpty()) {
        m_runtimeFilePath = QFileInfo(QDir(goroot),"src/pkg/runtime/").path();
    }

    QString args = QString("--interpreter=mi --args %1").arg(program);
    if (!arguments.isEmpty()) {
        args  += " " + arguments;
    }

    QString gdb = env.value("LITEIDE_GDB","gdb");
#ifdef Q_OS_WIN
    if (gdb.isEmpty()) {
        if (env.value("GOARCH") == "386") {
            gdb = "gdb";
        } else {
            gdb = "gdb64";
        }
    }
#endif

    m_gdbFilePath = FileUtil::lookPath(gdb,env,true);
    if (m_gdbFilePath.isEmpty()) {
        m_liteApp->appendLog("GdbDebugger",gdb+" was not found on system PATH (hint: is GDB installed?)",true);
        return false;
    }

    clear();

#ifdef Q_OS_WIN
        m_process->setNativeArguments(args);
        m_process->start("\""+m_gdbFilePath+"\"");
#else
        m_process->start(m_gdbFilePath + " " + args);
#endif

    QString log = QString("%1 %2 [%3]").arg(m_gdbFilePath).arg(args).arg(m_process->workingDirectory());
    emit debugLog(LiteApi::DebugRuntimeLog,log);

    return true;
}

void GdbDebugger::stop()
{
    command("-gdb-exit");
    if (!m_process->waitForFinished(300)) {
        m_process->kill();
    }
}

bool GdbDebugger::isRunning()
{
    return m_process->state() != QProcess::NotRunning;
}

void GdbDebugger::continueRun()
{
    command("-exec-continue");
}

void GdbDebugger::stepOver()
{
    command("-exec-next");
}

void GdbDebugger::stepInto()
{
    command("-exec-step");
}

void GdbDebugger::stepOut()
{
    command("-exec-finish");
}

void GdbDebugger::runToLine(const QString &fileName, int line)
{
    line++;
    GdbCmd cmd;
    QStringList args;
    args << "-break-insert";
    args << "-t";
    args << QString("%1:%2").arg(fileName).arg(line);
    cmd.setCmd(args);
    command(cmd);
    command("-exec-continue");
}

void GdbDebugger::createWatch(const QString &var, bool floating, bool watchModel)
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

void GdbDebugger::removeWatch(const QString &var, bool children)
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

void GdbDebugger::removeWatchByName(const QString &name, bool children)
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

void GdbDebugger::expandItem(QModelIndex index, LiteApi::DEBUG_MODEL_TYPE type)
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

void GdbDebugger::setInitBreakTable(const QMultiMap<QString,int> &bks)
{
    m_initBks = bks;
}

void GdbDebugger::insertBreakPoint(const QString &fileName, int line)
{
    line++;
    QString location = QString("%1:%2").arg(fileName).arg(line);
    if (m_locationBkMap.contains(location)) {
        return;
    }
    QStringList args;
    args << "-break-insert";
    args << QString("%1:%2").arg(fileName).arg(line);
    GdbCmd cmd;
    cmd.setCmd(args);
    cmd.insert("file",fileName);
    cmd.insert("line",line);
    command(cmd);
}

void GdbDebugger::removeBreakPoint(const QString &fileName, int line)
{
    line++;
    QString location = QString("%1:%2").arg(fileName).arg(line);
    QString number = m_locationBkMap.key(location);
    if (number.isEmpty()) {
        return;
    }
    QStringList args;
    args << "-break-delete";
    args << number;
    GdbCmd cmd;
    cmd.setCmd(args);
    command(cmd);
}

void GdbDebugger::command_helper(const GdbCmd &cmd, bool emitOut)
{
    m_token++;
    QByteArray buf = cmd.makeCmd(m_token);
    if (emitOut) {
        emit debugLog(LiteApi::DebugConsoleLog,">>> "+QString::fromUtf8(buf));
    }
#ifdef Q_OS_WIN
    buf.append("\r\n");
#else
    buf.append("\n");
#endif
    m_tokenCookieMap.insert(m_token,cmd.cookie());
    m_process->write(buf);
}

void GdbDebugger::command(const GdbCmd &cmd)
{
    command_helper(cmd,true);
}

void GdbDebugger::enterText(const QString &text)
{
    m_process->write(text.toUtf8());
}

void  GdbDebugger::command(const QByteArray &cmd)
{
    command_helper(GdbCmd(cmd),false);
}

void GdbDebugger::readStdError()
{
    emit debugLog(LiteApi::DebugErrorLog,QString::fromUtf8(m_process->readAllStandardError()));
}

/*
27.4.2 gdb/mi Output Syntax

The output from gdb/mi consists of zero or more out-of-band records followed, optionally,
by a single result record. This result record is for the most recent command. The sequence
of output records is terminated by ¡®(gdb)¡¯.
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

static bool isNameChar(char c)
{
    // could be 'stopped' or 'shlibs-added'
    return (c >= 'a' && c <= 'z') || c == '-';
}

void GdbDebugger::handleResponse(const QByteArray &buff)
{
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

void GdbDebugger::handleStopped(const GdbMiValue &result)
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
                QString fullname = LiteApi::getGoroot(m_liteApp)+file.right(file.length()-i-3);
                emit setCurrentLine(fullname,line.toInt()-1);
            }
        }
    }
}

void GdbDebugger::handleLibrary(const GdbMiValue &result)
{
    QString id = result.findChild("id").data();
    QString thread_group = result.findChild("thread-group").data();
    m_libraryModel->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(id)
                              << new QStandardItem(thread_group)
                              );
}

void GdbDebugger::handleAsyncClass(const QByteArray &asyncClass, const GdbMiValue &result)
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

void GdbDebugger::handleConsoleStream(const QByteArray&)
{

}

void GdbDebugger::handleTargetStream(const QByteArray&)
{

}

void GdbDebugger::handleLogStream(const QByteArray&)
{

}

void GdbDebugger::handleResultStackListFrame(const GdbResponse &response, QMap<QString,QVariant>&)
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

void GdbDebugger::handleResultStackListVariables(const GdbResponse &response, QMap<QString,QVariant>&)
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

void GdbDebugger::handleResultVarCreate(const GdbResponse &response, QMap<QString,QVariant> &map)
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

void GdbDebugger::handleResultVarListChildren(const GdbResponse &response, QMap<QString,QVariant> &map)
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

void GdbDebugger::handleResultVarUpdate(const GdbResponse &response, QMap<QString,QVariant>&)
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

void GdbDebugger::handleResultVarDelete(const GdbResponse &response, QMap<QString,QVariant> &map)
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

void GdbDebugger::handleResultVarUpdateValue(const GdbResponse &response, QMap<QString,QVariant> &map)
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

void GdbDebugger::handleResultVarUpdateType(const GdbResponse &response, QMap<QString,QVariant> &map)
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

void GdbDebugger::handleBreakInsert(const GdbResponse &response, QMap<QString,QVariant>&)
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

void GdbDebugger::handleBreakDelete(const GdbResponse &response, QMap<QString,QVariant> &map)
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

void GdbDebugger::handleResultRecord(const GdbResponse &response)
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

void GdbDebugger::clear()
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

void GdbDebugger::initGdb()
{
    command("set unwindonsignal on");
    command("set overload-resolution off");
    command("handle SIGSEGV nopass stop print");
    command("set breakpoint pending on");
    command("set width 0");
    command("set height 0");
    command("set auto-solib-add on");
    if (!m_runtimeFilePath.isEmpty()) {
#ifdef Q_OS_WIN
        QStringList pathList = LiteApi::getGopathList(m_liteApp,false);
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

    QMapIterator<QString,int> i(m_initBks);
    while (i.hasNext()) {
        i.next();
        QString fileName = i.key();
        QList<int> lines = m_initBks.values(fileName);
        foreach(int line, lines) {
            insertBreakPoint(fileName,line);
        }
    }
    command("-break-insert main.main");

    command("-exec-run");
    debugLoaded();
}

void GdbDebugger::updateWatch()
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

void GdbDebugger::updateLocals()
{
    command("-stack-list-variables 0");
}

void GdbDebugger::updateFrames()
{
    command("-stack-list-frames");
}

void GdbDebugger::updateBreaks()
{
    command("-break-info");
}

void GdbDebugger::updateVarTypeInfo(const QString &name)
{
    QStringList args;
    args << "-var-info-type";
    args << name;
    GdbCmd cmd;
    cmd.setCmd(args);
    cmd.insert("name",name);
    command(cmd);
}

void GdbDebugger::updateVarListChildren(const QString &name)
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

void GdbDebugger::updateVarValue(const QString &name)
{
    QStringList args;
    args << "-var-evaluate-expression";
    args << name;
    GdbCmd cmd;
    cmd.setCmd(args);
    cmd.insert("name",name);
    command(cmd);
}

void GdbDebugger::readStdOutput()
{
    int newstart = 0;
    int scan = m_inbuffer.size();
    m_inbuffer.append(m_process->readAllStandardOutput());

    // This can trigger when a dialog starts a nested event loop.
    if (m_busy)
        return;

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
        handleResponse(data);
        m_busy = false;
    }
    emit debugLog(LiteApi::DebugConsoleLog,QString::fromUtf8(m_inbuffer));
    m_inbuffer.clear();

    if (!m_gdbinit) {
        m_gdbinit = true;
        initGdb();
    }

    if (m_handleState.exited() && !m_gdbexit) {
        m_gdbexit = true;
        stop();
    } else if (m_handleState.stopped()) {
        updateWatch();
        updateLocals();
        updateFrames();
    }

    m_handleState.clear();
}

void GdbDebugger::finished(int code)
{
    clear();
    emit debugStoped();
    emit debugLog(LiteApi::DebugRuntimeLog,QString("Program exited with code %1").arg(code));
}
