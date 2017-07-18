#include "dlvclient.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QUrl>
#include <QDebug>
#include "qjsonrpc/src/json/qjson_export.h"
#include "qjsonrpc/src/json/qjsondocument.h"
#include "qjsonrpc/src/qjsonrpcservicereply.h"

DlvClient::DlvClient(QObject *parent) : QObject(parent)
{

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
    m_dlv.reset(new QJsonRpcSocket(tcpSocket,this));
    return true;
}

int DlvClient::ProcessPid() const
{
    ProcessPidIn in;
    ProcessPidOut out;
    call("ProcessPid",&in,&out);
    return out.Pid;
}

QDateTime DlvClient::LastModified() const
{
    LastModifiedIn in;
    LastModifiedOut out;
    call("LastModified",&in,&out);
    return out.Time;
}

bool DlvClient::Detach() const
{
    DetachIn in;
    in.Kill = true;
    DetachOut out;
    return call("Detach",&in,&out);
}

QList<DiscardedBreakpoint> DlvClient::Restart() const
{
    RestartIn in;
    RestartOut out;
    call("Restart",&in,&out);
    return out.DiscardedBreakpoints;
}

QList<DiscardedBreakpoint> DlvClient::RestartFrom(const QString &pos) const
{
    RestartIn in;
    in.Position = pos;
    RestartOut out;
    call("Restart",&in,&out);
    return out.DiscardedBreakpoints;
}

DebuggerState DlvClient::Next() const
{
    DebuggerCommand in("next");
    CommandOut out;
    call("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::Step() const
{
    DebuggerCommand in("step");
    CommandOut out;
    call("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::StepOut() const
{
    DebuggerCommand in("stepOut");
    CommandOut out;
    call("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::StepInstruction() const
{
    DebuggerCommand in("stepInstruction");
    CommandOut out;
    call("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::SwitchThread(int threadID) const
{
    DebuggerCommand in("switchThread");
    in.ThreadID = threadID;
    CommandOut out;
    call("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::SwitchGoroutine(int goroutineID) const
{
    DebuggerCommand in("switchThread");
    in.GoroutineID = goroutineID;
    CommandOut out;
    call("Command",&in,&out);
    return out.State;
}

DebuggerState DlvClient::Halt() const
{
    DebuggerCommand in("halt");
    CommandOut out;
    call("Command",&in,&out);
    return out.State;
}

BreakpointPointer DlvClient::CreateBreakpoint(const Breakpoint &bp) const
{
    CreateBreakpointIn in;
    in.Breakpoint = bp;
    CreateBreakpointOut out;
    call("CreateBreakpoint",&in,&out);
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
    call("GetBreakpoint",&in,&out);
    return out.pBreakpoint;
}

BreakpointPointer DlvClient::GetBreakpointByName(const QString &name) const
{
    GetBreakpointIn in;
    in.Name = name;
    GetBreakpointOut out;
    call("GetBreakpoint",&in,&out);
    return out.pBreakpoint;
}

QList<Breakpoint> DlvClient::ListBreakpoints() const
{
    ListBreakpointsIn in;
    ListBreakpointsOut out;
    call("ListBreakpoints",&in,&out);
    return out.Breakpoints;
}

BreakpointPointer DlvClient::ClearBreakpoint(int id) const
{
    CleartBreakpointIn in;
    in.Id = id;
    ClearBreakpointOut out;
    call("ClearBreakpoint",&in,&out);
    return out.pBreakpoint;
}

BreakpointPointer DlvClient::ClearBreakpointByName(const QString &name) const
{
    CleartBreakpointIn in;
    in.Name = name;
    ClearBreakpointOut out;
    call("ClearBreakpoint",&in,&out);
    return out.pBreakpoint;
}

bool DlvClient::AmendBreakpoint(const Breakpoint &bp) const
{
    AmendBreakpointIn in;
    in.Breakpoint = bp;
    AmendBreakpointOut out;
    return call("AmendBreakpoint",&in,&out);
}

bool DlvClient::CancelNext() const
{
    CancelNextIn in;
    CancelNextOut out;
    return call("CancelNext",&in,&out);
}

QList<Thread> DlvClient::ListThreads() const
{
    ListThreadsIn in;
    ListThreadsOut out;
    call("ListThreads",&in,&out);
    return out.Threads;
}

ThreadPointer DlvClient::GetThread(int id) const
{
    GetThreadIn in;
    in.Id = id;
    GetThreadOut out;
    call("GetThread",&in,&out);
    return out.pThread;
}

VariablePointer DlvClient::EvalVariable(const EvalScope &scope, const QString &expr, const LoadConfig &cfg) const
{
    EvalIn in;
    in.Scope = scope;
    in.Expr = expr;
    in.Cfg = cfg;
    EvalOut out;
    call("Eval",&in,&out);
    return out.pVariable;
}

bool DlvClient::SetVariable(const EvalScope &scope, const QString &symbol, const QString &value)
{
    SetIn in;
    in.Scope = scope;
    in.Symbol = symbol;
    in.Value = value;
    SetOut out;
    return call("Set",&in,&out);
}

QStringList DlvClient::ListSources(const QString &filter)
{
    ListSourcesIn in;
    in.Filter = filter;
    ListSourcesOut out;
    call("ListSources",&in,&out);
    return out.Sources;
}

QStringList DlvClient::ListFunctions(const QString &filter)
{
    ListFunctionsIn in;
    in.Filter = filter;
    ListFunctionsOut out;
    call("ListFunctions",&in,&out);
    return out.Funcs;
}

DebuggerState DlvClient::GetState() const
{
    StateIn in;
    StateOut out;
    call("State",&in,&out);
    return out.State;
}

QStringList DlvClient::ListTypes(const QString &filter) const
{
    ListTypesIn in;
    in.Filter = filter;
    ListTypesOut out;
    call("ListTypes",&in,&out);
    return out.Types;
}

QList<Variable> DlvClient::ListPackageVariables(const QString &filter, const LoadConfig &cfg) const
{
    ListPackageVarsIn in;
    in.Filter = filter;
    in.Cfg = cfg;
    ListPackageVarsOut out;
    call("ListPackageVars",&in,&out);
    return out.Variables;
}

QList<Variable> DlvClient::ListLocalVariables(const EvalScope &scope, const LoadConfig &cfg) const
{
    ListLocalVarsIn in;
    in.Scope = scope;
    in.Cfg = cfg;
    ListLocalVarsOut out;
    call("ListLocalVars",&in,&out);
    return out.Variables;
}

QList<Register> DlvClient::ListRegisters(int threadID, bool includeFp) const
{
    ListRegistersIn in;
    in.ThreadID = threadID;
    in.IncludeFp = includeFp;
    ListRegistersOut out;
    call("ListRegisters",&in,&out);
    return out.Regs;
}

QList<Variable> DlvClient::ListFunctionArgs(const EvalScope &scope, const LoadConfig &cfg) const
{
    ListFunctionArgsIn in;
    in.Scope = scope;
    in.Cfg = cfg;
    ListFunctionArgsOut out;
    call("ListFunctionArgs",&in,&out);
    return out.Variables;
}

QList<Goroutine> DlvClient::ListGoroutines() const
{
    ListGoroutinesIn in;
    ListGoroutinesOut out;
    call("ListGoroutines",&in,&out);
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
    call("Stacktrace",&in,&out);
    return out.Locations;
}

bool DlvClient::AttachedToExistingProcess() const
{
    AttachedToExistingProcessIn in;
    AttachedToExistingProcessOut out;
    call("AttachedToExistingProcess",&in,&out);
    return out.Answer;
}

QList<Location> DlvClient::FindLocation(const EvalScope &scope, const QString &loc) const
{
    FindLocationIn in;
    in.Scope= scope;
    in.Loc = loc;
    FindLocationOut out;
    call("FindLocation",&in,&out);
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
    call("Disassemble",&in,&out);
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
    call("Disassemble",&in,&out);
    return out.Disassemble;
}

bool DlvClient::Recorded() const
{
    RecordedIn in;
    RecordedOut out;
    call("Recorded",&in,&out);
    return out.Recorded;
}

QString DlvClient::TraceDirectory() const
{
    RecordedIn in;
    RecordedOut out;
    call("Recorded",&in,&out);
    return out.TraceDirectory;
}

int DlvClient::Checkpoint(const QString &where) const
{
    CheckpointIn in;
    in.Where = where;
    CheckpointOut out;
    call("Checkpoint",&in,&out);
    return out.ID;
}

QList<struct Checkpoint> DlvClient::ListCheckpoints() const
{
    ListCheckpointsIn in;
    ListCheckpointsOut out;
    call("ListCheckpoints",&in,&out);
    return out.Checkpoints;
}

bool DlvClient::ClearCheckpoint(int id) const
{
    ClearCheckpointIn in;
    in.ID = id;
    ClearBreakpointOut out;
    return call("ClearCheckpoint",&in,&out);
}

int DlvClient::GetAPIVersion() const
{
    GetVersionIn in;
    GetVersionOut out;
    call("GetVersion",&in,&out);
    return out.APIVersion;
}

QString DlvClient::GetDelveVersion() const
{
    GetVersionIn in;
    GetVersionOut out;
    call("GetVersion",&in,&out);
    return out.DelveVersion;
}

bool DlvClient::SetAPIVersion(int version) const
{
    SetAPIVersionIn in;
    in.APIVersion = version;
    SetAPIVersionOut out;
    return call("SetApiVersion",&in,&out);
}

void DlvClient::callMethod(bool notification, const QString &method)
{
    QVariantList arguments;

    QJsonRpcMessage request = notification ?
        QJsonRpcMessage::createNotification(method, QJsonArray::fromVariantList(arguments)) :
        QJsonRpcMessage::createRequest(method, QJsonArray::fromVariantList(arguments));
    qDebug() << request.toJson();

    QJsonRpcMessage response = m_dlv->sendMessageBlocking(request, 5000);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());
        return ;
    }

    qDebug() << response.result();
    ProcessPidOut out;
    qDebug() << response.result().toVariant().toMap();
    //VariantHelper::VarintToQObject(response.result().toVariant(),&out);
    //qDebug() << out.pid();
}

bool DlvClient::callCommand(const QString &cmd)
{
    if (!m_replyMutext.tryLock(5000)) {
        return false;
    }

    QVariantMap m;
    m.insert("name",cmd);
    QVariantList arguments;
    arguments << m;

    QJsonRpcMessage request = QJsonRpcMessage::createRequest("RPCServer.Command", QJsonArray::fromVariantList(arguments));

    QJsonRpcServiceReply *reply = m_dlv->sendMessage(request);
    if (!reply) {
        m_replyMutext.unlock();
        return false;
    }

    connect(reply,SIGNAL(finished()),this,SLOT(finishedReply()));

    m_lastReply.reset(reply);
    return true;
}

bool DlvClient::waitReply()
{
    if (!m_replyMutext.tryLock(5000)) {
        return false;
    }
    m_replyMutext.unlock();
    return true;
}

void DlvClient::finishedReply()
{
   QJsonRpcServiceReply *reply = (QJsonRpcServiceReply*)(sender());
   qDebug() << reply->request().toJson() << reply->response().toJson();
   m_replyMutext.unlock();
}

bool DlvClient::call(const QString &method, const JsonDataIn *in, JsonDataOut *out, int timeout) const
{
    QVariantMap param;
    in->toMap(param);
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("RPCServer."+method, QJsonValue::fromVariant(param));

    qDebug() << "->" << QJsonDocument(request.toObject()).toJson(QJsonDocument::Compact);

    QJsonRpcMessage response = m_dlv->sendMessageBlocking(request,timeout);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());//
        return false;
    }
    qDebug() << "<-" << QJsonDocument(response.result().toObject()).toJson(QJsonDocument::Compact);;
    out->fromMap(response.result().toVariant().toMap());
    return true;
    /*
   QJsonRpcServiceReply *reply = m_dlv->sendMessage(request);

   connect(reply,SIGNAL(finished()),this,SLOT(finishedReply()));

    QJsonRpcMessage response = m_dlv->sendMessageBlocking(request, timeout);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());//
        return false;
    }
    qDebug() << "<-" << QJsonDocument(response.result().toObject()).toJson(QJsonDocument::Compact);;
    out->fromMap(response.result().toVariant().toMap());
    */
    return true;
}

DlvThread::DlvThread(QJsonRpcSocket *dlv, QObject *parent)
    : QThread(parent), m_dlv(dlv)
{

}

void DlvThread::call(const QString &command)
{
    m_command = command;
    m_pDebuggerState.clear();
    QThread::start();
}

void DlvThread::run()
{
    QVariantMap m;
    m.insert("name",m_command);
    QVariantList arguments;
    arguments << m;

    QJsonRpcMessage request = QJsonRpcMessage::createRequest("RPCServer.Command", QJsonArray::fromVariantList(arguments));

    //QJsonRpcServiceReply *reply = m_dlv->sendMessageBlocking(request,);
}
