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

class DlvThread : public QThread
{
    Q_OBJECT
public:
    DlvThread(QJsonRpcSocket *dlv, QObject *parent = 0);
    void call(const QString &command);
    virtual void run();
protected:
    QString m_command;
    DebuggerStatePointer m_pDebuggerState;
    QJsonRpcSocket *m_dlv;
};

class DlvClient : public QObject
{
    Q_OBJECT
public:
    DlvClient(QObject *parent);
    bool IsConnect() const;
    QString Address() const;
    bool Connect(const QString &service);
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
public slots:
    void finishedReply();
    bool callCommand(const QString &cmd);
    bool waitReply();
protected:
    void callMethod(bool notification, const QString &method);
    bool call(const QString &method, const JsonDataIn *in, JsonDataOut *out, int timeout = 5000) const;
protected:
    QScopedPointer<QJsonRpcSocket> m_dlv;
    QString m_addr;
    QScopedPointer<QJsonRpcServiceReply> m_lastReply;
    QMutex m_replyMutext;
    QWaitCondition m_replayCondition;
};

#endif // DLVCLIENT_H
