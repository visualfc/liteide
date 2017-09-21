/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
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
// Module: dlvclient.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "dlvclient.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QUrl>
#include <QDebug>
#include "qjsonrpc/src/json/qjson_export.h"
#include "qjsonrpc/src/json/qjsondocument.h"
#include "qjsonrpc/src/qjsonrpcservicereply.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

DlvClient::DlvClient(QObject *parent) : QObject(parent)
{
    m_isCommandBlock = false;
    m_callTimeout = 5000;
}

bool DlvClient::IsConnect() const
{
    return !m_dlv.isNull();
}

QString DlvClient::Address() const
{
    return m_addr;
}

bool DlvClient::Connect(const QString &service)
{
    QUrl serviceUrl = QUrl::fromUserInput(service);
    QHostAddress serviceAddress(serviceUrl.host());
    QTcpSocket *tcpSocket = new QTcpSocket(this);
    int servicePort = serviceUrl.port() ? serviceUrl.port() : 5555;
    tcpSocket->connectToHost(serviceAddress, servicePort);
    if (!tcpSocket->waitForConnected(5000)) {
        qDebug("could not connect to host at %s:%d", serviceUrl.host().toLocal8Bit().data(),
               servicePort);
        return false;
    }
    m_addr = service;
    m_isCommandBlock = false;
    m_dlv.reset(new QJsonRpcSocket(tcpSocket,this));
    return true;
}

void DlvClient::SetCallTimeout(int timeout)
{
    m_callTimeout = timeout;
}

int DlvClient::CallTimeout() const
{
    return m_callTimeout;
}

int DlvClient::ProcessPid() const
{
    ProcessPidIn in;
    ProcessPidOut out;
    callBlocked("ProcessPid",&in,&out);
    return out.Pid;
}

QDateTime DlvClient::LastModified() const
{
    LastModifiedIn in;
    LastModifiedOut out;
    callBlocked("LastModified",&in,&out);
    return out.Time;
}

bool DlvClient::Detach() const
{
    DetachIn in;
    in.Kill = true;
    DetachOut out;
    return callBlocked("Detach",&in,&out);
}

QList<DiscardedBreakpoint> DlvClient::Restart() const
{
    RestartIn in;
    RestartOut out;
    callBlocked("Restart",&in,&out);
    return out.DiscardedBreakpoints;
}

QList<DiscardedBreakpoint> DlvClient::RestartFrom(const QString &pos) const
{
    RestartIn in;
    in.Position = pos;
    RestartOut out;
    callBlocked("Restart",&in,&out);
    return out.DiscardedBreakpoints;
}

DebuggerState DlvClient::Next() const
{
    DebuggerCommand in("next");
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::Step() const
{
    DebuggerCommand in("step");
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::StepOut() const
{
    DebuggerCommand in("stepOut");
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::StepInstruction() const
{
    DebuggerCommand in("stepInstruction");
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::SwitchThread(int threadID) const
{
    DebuggerCommand in("switchThread");
    in.ThreadID = threadID;
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::SwitchGoroutine(int goroutineID) const
{
    DebuggerCommand in("switchGoroutine");
    in.GoroutineID = goroutineID;
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::Halt() const
{
    DebuggerCommand in("halt");
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::Continue() const
{
    DebuggerCommand in("continue");
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::Rewind() const
{
    DebuggerCommand in("rewind");
    CommandOut out;
    callBlocked("Command",&in,&out);
    return out.State;
}

bool DlvClient::CallNext()
{
   DebuggerCommand cmd("next");
   return callDebuggerCommand(cmd);
}

bool DlvClient::CallStep()
{
    DebuggerCommand cmd("step");
    return callDebuggerCommand(cmd);
}

bool DlvClient::CallStepOut()
{
    DebuggerCommand cmd("stepOut");
    return callDebuggerCommand(cmd);
}

bool DlvClient::CallStepInstruction()
{
    DebuggerCommand cmd("stepInstruction");
    return callDebuggerCommand(cmd);
}

bool DlvClient::CallSwitchThread(int threadID)
{
    DebuggerCommand cmd("switchThread");
    cmd.ThreadID = threadID;
    return callDebuggerCommand(cmd);
}

bool DlvClient::CallSwitchGoroutine(int goroutineID)
{
    DebuggerCommand cmd("switchGoroutine");
    cmd.GoroutineID = goroutineID;
    return callDebuggerCommand(cmd);
}

bool DlvClient::CallHalt()
{
    DebuggerCommand cmd("halt");
    return callDebuggerCommand(cmd);
}

bool DlvClient::CallContinue()
{
    DebuggerCommand cmd("continue");
    return callDebuggerCommand(cmd);
}

bool DlvClient::CallRewind()
{
    DebuggerCommand cmd("rewind");
    return callDebuggerCommand(cmd);
}

BreakpointPointer DlvClient::CreateBreakpoint(const Breakpoint &bp) const
{
    CreateBreakpointIn in;
    in.Breakpoint = bp;
    CreateBreakpointOut out;
    callBlocked("CreateBreakpoint",&in,&out);
    return out.pBreakpoint;
}

BreakpointPointer DlvClient::CreateBreakpointByFuncName(const QString &funcName) const
{
    Breakpoint bp;
    bp.FunctionName = funcName;
    return CreateBreakpoint(bp);
}

BreakpointPointer DlvClient::GetBreakpoint(int id) const
{
    GetBreakpointIn in;
    in.Id = id;
    GetBreakpointOut out;
    callBlocked("GetBreakpoint",&in,&out);
    return out.pBreakpoint;
}

BreakpointPointer DlvClient::GetBreakpointByName(const QString &name) const
{
    GetBreakpointIn in;
    in.Name = name;
    GetBreakpointOut out;
    callBlocked("GetBreakpoint",&in,&out);
    return out.pBreakpoint;
}

QList<Breakpoint> DlvClient::ListBreakpoints() const
{
    ListBreakpointsIn in;
    ListBreakpointsOut out;
    callBlocked("ListBreakpoints",&in,&out);
    return out.Breakpoints;
}

BreakpointPointer DlvClient::ClearBreakpoint(int id) const
{
    CleartBreakpointIn in;
    in.Id = id;
    ClearBreakpointOut out;
    callBlocked("ClearBreakpoint",&in,&out);
    return out.pBreakpoint;
}

BreakpointPointer DlvClient::ClearBreakpointByName(const QString &name) const
{
    CleartBreakpointIn in;
    in.Name = name;
    ClearBreakpointOut out;
    callBlocked("ClearBreakpoint",&in,&out);
    return out.pBreakpoint;
}

bool DlvClient::AmendBreakpoint(const Breakpoint &bp) const
{
    AmendBreakpointIn in;
    in.Breakpoint = bp;
    AmendBreakpointOut out;
    return callBlocked("AmendBreakpoint",&in,&out);
}

bool DlvClient::CancelNext() const
{
    CancelNextIn in;
    CancelNextOut out;
    return callBlocked("CancelNext",&in,&out);
}

QList<Thread> DlvClient::ListThreads() const
{
    ListThreadsIn in;
    ListThreadsOut out;
    callBlocked("ListThreads",&in,&out);
    return out.Threads;
}

ThreadPointer DlvClient::GetThread(int id) const
{
    GetThreadIn in;
    in.Id = id;
    GetThreadOut out;
    callBlocked("GetThread",&in,&out);
    return out.pThread;
}

VariablePointer DlvClient::EvalVariable(const EvalScope &scope, const QString &expr, const LoadConfig &cfg) const
{
    EvalIn in;
    in.Scope = scope;
    in.Expr = expr;
    in.Cfg = cfg;
    EvalOut out;
    callBlocked("Eval",&in,&out);
    return out.pVariable;
}

bool DlvClient::SetVariable(const EvalScope &scope, const QString &symbol, const QString &value)
{
    SetIn in;
    in.Scope = scope;
    in.Symbol = symbol;
    in.Value = value;
    SetOut out;
    return callBlocked("Set",&in,&out);
}

QStringList DlvClient::ListSources(const QString &filter)
{
    ListSourcesIn in;
    in.Filter = filter;
    ListSourcesOut out;
    callBlocked("ListSources",&in,&out);
    return out.Sources;
}

QStringList DlvClient::ListFunctions(const QString &filter)
{
    ListFunctionsIn in;
    in.Filter = filter;
    ListFunctionsOut out;
    callBlocked("ListFunctions",&in,&out);
    return out.Funcs;
}

DebuggerState DlvClient::GetState() const
{
    StateIn in;
    StateOut out;
    callBlocked("State",&in,&out);
    return out.State;
}

QStringList DlvClient::ListTypes(const QString &filter) const
{
    ListTypesIn in;
    in.Filter = filter;
    ListTypesOut out;
    callBlocked("ListTypes",&in,&out);
    return out.Types;
}

QList<Variable> DlvClient::ListPackageVariables(const QString &filter, const LoadConfig &cfg) const
{
    ListPackageVarsIn in;
    in.Filter = filter;
    in.Cfg = cfg;
    ListPackageVarsOut out;
    callBlocked("ListPackageVars",&in,&out);
    return out.Variables;
}

QList<Variable> DlvClient::ListLocalVariables(const EvalScope &scope, const LoadConfig &cfg) const
{
    ListLocalVarsIn in;
    in.Scope = scope;
    in.Cfg = cfg;
    ListLocalVarsOut out;
    callBlocked("ListLocalVars",&in,&out);
    return out.Variables;
}

QList<Register> DlvClient::ListRegisters(int threadID, bool includeFp) const
{
    ListRegistersIn in;
    in.ThreadID = threadID;
    in.IncludeFp = includeFp;
    ListRegistersOut out;
    callBlocked("ListRegisters",&in,&out);
    return out.Regs;
}

QList<Variable> DlvClient::ListFunctionArgs(const EvalScope &scope, const LoadConfig &cfg) const
{
    ListFunctionArgsIn in;
    in.Scope = scope;
    in.Cfg = cfg;
    ListFunctionArgsOut out;
    callBlocked("ListFunctionArgs",&in,&out);
    return out.Variables;
}

QList<Goroutine> DlvClient::ListGoroutines() const
{
    ListGoroutinesIn in;
    ListGoroutinesOut out;
    callBlocked("ListGoroutines",&in,&out);
    return out.Goroutines;
}

QList<Stackframe> DlvClient::Stacktrace(int goroutineId, int depth, const LoadConfig &cfg) const
{
    StacktraceIn in;
    in.Id = goroutineId;
    in.Depth = depth;
    in.Full = true;
    in.Cfg = cfg;
    StacktraceOut out;
    callBlocked("Stacktrace",&in,&out);
    return out.Locations;
}

bool DlvClient::AttachedToExistingProcess() const
{
    AttachedToExistingProcessIn in;
    AttachedToExistingProcessOut out;
    callBlocked("AttachedToExistingProcess",&in,&out);
    return out.Answer;
}

QList<Location> DlvClient::FindLocation(const EvalScope &scope, const QString &loc) const
{
    FindLocationIn in;
    in.Scope= scope;
    in.Loc = loc;
    FindLocationOut out;
    callBlocked("FindLocation",&in,&out);
    return out.Locations;
}

QList<AsmInstruction> DlvClient::DisassembleRange(const EvalScope &scope, quint64 startPC, quint64 endPC, AssemblyFlavour flavour) const
{
    DisassembleIn in;
    in.Scope = scope;
    in.StartPC = startPC;
    in.EndPC = endPC;
    in.Flavour = flavour;
    DisassembleOut out;
    callBlocked("Disassemble",&in,&out);
    return out.Disassemble;
}

QList<AsmInstruction> DlvClient::DisassemblePC(const EvalScope &scope, quint64 pc, AssemblyFlavour flavour) const
{
    DisassembleIn in;
    in.Scope = scope;
    in.StartPC = pc;
    in.EndPC = 0;
    in.Flavour = flavour;
    DisassembleOut out;
    callBlocked("Disassemble",&in,&out);
    return out.Disassemble;
}

bool DlvClient::Recorded() const
{
    RecordedIn in;
    RecordedOut out;
    callBlocked("Recorded",&in,&out);
    return out.Recorded;
}

QString DlvClient::TraceDirectory() const
{
    RecordedIn in;
    RecordedOut out;
    callBlocked("Recorded",&in,&out);
    return out.TraceDirectory;
}

int DlvClient::Checkpoint(const QString &where) const
{
    CheckpointIn in;
    in.Where = where;
    CheckpointOut out;
    callBlocked("Checkpoint",&in,&out);
    return out.ID;
}

QList<struct Checkpoint> DlvClient::ListCheckpoints() const
{
    ListCheckpointsIn in;
    ListCheckpointsOut out;
    callBlocked("ListCheckpoints",&in,&out);
    return out.Checkpoints;
}

bool DlvClient::ClearCheckpoint(int id) const
{
    ClearCheckpointIn in;
    in.ID = id;
    ClearBreakpointOut out;
    return callBlocked("ClearCheckpoint",&in,&out);
}

int DlvClient::GetAPIVersion() const
{
    GetVersionIn in;
    GetVersionOut out;
    callBlocked("GetVersion",&in,&out);
    return out.APIVersion;
}

QString DlvClient::GetDelveVersion() const
{
    GetVersionIn in;
    GetVersionOut out;
    callBlocked("GetVersion",&in,&out);
    return out.DelveVersion;
}

bool DlvClient::SetAPIVersion(int version) const
{
    SetAPIVersionIn in;
    in.APIVersion = version;
    SetAPIVersionOut out;
    return callBlocked("SetApiVersion",&in,&out);
}

bool DlvClient::callCommand(const QString &cmd)
{
    DebuggerCommand dc(cmd);
    return callDebuggerCommand(dc);
}

bool DlvClient::callDebuggerCommand(const DebuggerCommand &cmd)
{
    if (m_isCommandBlock) {
        return false;
    }
    m_isCommandBlock = true;
    m_lastCommand = cmd;
    QVariantMap param;
    cmd.toMap(param);
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("RPCServer.Command", QJsonValue::fromVariant(param));
    QJsonRpcServiceReply *reply = m_dlv->sendMessage(request);
    if (!reply) {
        m_isCommandBlock = false;
        return false;
    }
    connect(reply,SIGNAL(finished()),this,SLOT(finishedCommandReply()));
    m_lastReply.reset(reply);
    return true;
}

bool DlvClient::isCommandBlocked() const
{
    return m_isCommandBlock;
}

void DlvClient::finishedCommandReply()
{
    m_isCommandBlock = false;
    QJsonRpcServiceReply *reply = (QJsonRpcServiceReply*)(sender());
    if (reply->response().type() == QJsonRpcMessage::Error) {
        int code = reply->response().errorCode();
        QString msg = reply->response().errorMessage();
        if (msg.isEmpty()) {
            ResponseError resp;
            resp.fromMap(reply->response().toObject().toVariantMap());
            msg = resp.error;
        }
        emit commandError(code,msg);
    } else {
        CommandOut out;
        out.fromMap(reply->response().result().toVariant().toMap());
        emit commandSuccess(m_lastCommand.Name,out.State);
    }
}

bool DlvClient::callBlocked(const QString &method, const JsonDataIn *in, JsonDataOut *out) const
{
    QVariantMap param;
    in->toMap(param);
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("RPCServer."+method, QJsonValue::fromVariant(param));

    QJsonRpcMessage response = m_dlv->sendMessageBlocking(request,m_callTimeout);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());//
        return false;
    }
    out->fromMap(response.result().toVariant().toMap());
    return true;
}
