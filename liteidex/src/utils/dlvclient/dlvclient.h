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
// Module: dlvclient.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef DLVCLIENT_H
#define DLVCLIENT_H

#include <QTcpSocket>
#include <QDateTime>
#include <QScopedPointer>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "qjsonrpc/src/qjsonrpcservicereply.h"
#include "qjsonrpc/src/qjsonrpcsocket.h"
#include "dlvtypes.h"

struct ResponseError
{
    QString error;
    QString id;
    QVariant result;
    void fromMap(const QVariantMap &map)
    {
        error = map["error"].toString();
        id = map["id"].toInt();
        result = map["result"];
    }
};

#define DLV_COMMAND_NEXT "next"
#define DLV_COMMAND_STEP "step"
#define DLV_COMMAND_STEP_OUT "stepOut"
#define DLV_COMMAND_STEP_INSTRUCTION "stepInstruction"
#define DLV_COMMAND_CONTINUE "continue"
#define DLV_COMMAND_REWIND "rewind"

class DlvClient : public QObject
{
    Q_OBJECT
public:
    DlvClient(QObject *parent);
    bool IsConnect() const;
    QString Address() const;
    bool Connect(const QString &service);
    void SetCallTimeout(int timeout);
    int CallTimeout() const;
    int ProcessPid() const;
    QDateTime LastModified() const;
    bool Detach() const;
    QList<DiscardedBreakpoint> Restart() const;
    QList<DiscardedBreakpoint> RestartFrom(const QString &pos) const;
    DebuggerState Next() const;
    DebuggerState GetState() const;
    DebuggerState Step() const;
    DebuggerState StepOut() const;
    DebuggerState StepInstruction() const;
    DebuggerState SwitchThread(int threadID) const;
    DebuggerState SwitchGoroutine(int goroutineID) const;
    DebuggerState Halt() const;
    DebuggerState Continue() const;
    DebuggerState Rewind() const;
    bool CallNext();
    bool CallStep();
    bool CallStepOut();
    bool CallStepInstruction();
    bool CallSwitchThread(int threadID);
    bool CallSwitchGoroutine(int goroutineID);
    bool CallHalt();
    bool CallContinue();
    bool CallRewind();
    BreakpointPointer CreateBreakpoint(const Breakpoint &bp) const;
    BreakpointPointer CreateBreakpointByFuncName(const QString &funcName) const;
    BreakpointPointer GetBreakpoint(int id) const;
    BreakpointPointer GetBreakpointByName(const QString &name) const;
    QList<Breakpoint> ListBreakpoints() const;
    BreakpointPointer ClearBreakpoint(int id) const;
    BreakpointPointer ClearBreakpointByName(const QString &name) const;
    bool AmendBreakpoint(const Breakpoint &bp) const;
    bool CancelNext() const;
    QList<Thread> ListThreads() const;
    ThreadPointer GetThread(int id) const;
    VariablePointer EvalVariable(const EvalScope &scope, const QString &expr, const LoadConfig &cfg) const;
    bool SetVariable(const EvalScope &scope, const QString &symbol,const QString &value);
    QStringList ListSources(const QString &filter);
    QStringList ListFunctions(const QString &filter);
    QStringList ListTypes(const QString &filter) const;
    QList<Variable> ListPackageVariables(const QString &filter, const LoadConfig &cfg) const;
    QList<Variable> ListLocalVariables(const EvalScope &scope, const LoadConfig &cfg) const;
    QList<Register> ListRegisters(int threadID, bool includeFp) const;
    QList<Variable> ListFunctionArgs(const EvalScope &scope, const LoadConfig &cfg) const;
    QList<Goroutine> ListGoroutines() const;
    QList<Stackframe> Stacktrace(int goroutineId, int depth, const LoadConfig &cfg) const;
    bool AttachedToExistingProcess() const;
    QList<Location> FindLocation(const EvalScope &scope, const QString &loc) const;
    QList<AsmInstruction> DisassembleRange(const EvalScope &scope,quint64 startPC, quint64 endPC, AssemblyFlavour flavour) const;
    QList<AsmInstruction> DisassemblePC(const EvalScope &scope,quint64 pc, AssemblyFlavour flavour) const;
    bool Recorded() const;
    QString TraceDirectory() const;
    int Checkpoint(const QString &where) const;
    QList<struct Checkpoint> ListCheckpoints() const;
    bool ClearCheckpoint(int id) const;
    int GetAPIVersion() const;
    QString GetDelveVersion() const;
    bool SetAPIVersion(int version) const;
public:
    bool isCommandBlocked() const;
signals:
    void commandSuccess(const QString &method, const DebuggerState &state);
    void commandError(int code, const QString &errorMessage);
public slots:
    bool callCommand(const QString &cmd);
    bool callDebuggerCommand(const DebuggerCommand &cmd);
protected slots:
    void finishedCommandReply();
protected:
    bool callBlocked(const QString &method, const JsonDataIn *in, JsonDataOut *out) const;
protected:
    QScopedPointer<QJsonRpcSocket> m_dlv;
    QString m_addr;
    QScopedPointer<QJsonRpcServiceReply> m_lastReply;
    DebuggerCommand m_lastCommand;
    bool m_isCommandBlock;
    int m_callTimeout;
};

#endif // DLVCLIENT_H
