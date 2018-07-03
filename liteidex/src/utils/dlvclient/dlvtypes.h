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
// Module: dlvtypes.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef DLVTYPES_H
#define DLVTYPES_H

#include <QVariant>
#include <QDateTime>
#include <QSharedPointer>
#include "varianthelper.h"
#include "qjsonrpc/src/json/qjsonobject.h"

#include <QDebug>

/*
type Function struct {
    // Name is the function name.
    Name   string `json:"name"`
    Value  uint64 `json:"value"`
    Type   byte   `json:"type"`
    GoType uint64 `json:"goType"`
}
*/
struct Function
{
    Function() : Value(0),Type(0),GoType(0)
    {
    }
    void fromMap(const QVariantMap &map)
    {
        Name = map["name"].toString();
        Value = map["value"].value<quint64>();
        Type = map["type"].value<qint8>();
        GoType = map["goType"].value<quint64>();
    }
    QString Name;
    quint64 Value;
    qint8 Type;
    quint64 GoType;
};
typedef QSharedPointer<Function> FunctionPointer;

/*
// Variable describes a variable.
type Variable struct {
    // Name of the variable or struct member
    Name string `json:"name"`
    // Address of the variable or struct member
    Addr uintptr `json:"addr"`
    // Only the address field is filled (result of evaluating expressions like &<expr>)
    OnlyAddr bool `json:"onlyAddr"`
    // Go type of the variable
    Type string `json:"type"`
    // Type of the variable after resolving any typedefs
    RealType string `json:"realType"`

    Kind reflect.Kind `json:"kind"`

    //Strings have their length capped at proc.maxArrayValues, use Len for the real length of a string
    //Function variables will store the name of the function in this field
    Value string `json:"value"`

    // Number of elements in an array or a slice, number of keys for a map, number of struct members for a struct, length of strings
    Len int64 `json:"len"`
    // Cap value for slices
    Cap int64 `json:"cap"`

    // Array and slice elements, member fields of structs, key/value pairs of maps, value of complex numbers
    // The Name field in this slice will always be the empty string except for structs (when it will be the field name) and for complex numbers (when it will be "real" and "imaginary")
    // For maps each map entry will have to items in this slice, even numbered items will represent map keys and odd numbered items will represent their values
    // This field's length is capped at proc.maxArrayValues for slices and arrays and 2*proc.maxArrayValues for maps, in the circumnstances where the cap takes effect len(Children) != Len
    // The other length cap applied to this field is related to maximum recursion depth, when the maximum recursion depth is reached this field is left empty, contrary to the previous one this cap also applies to structs (otherwise structs will always have all their member fields returned)
    Children []Variable `json:"children"`

    // Unreadable addresses will have this field set
    Unreadable string `json:"unreadable"`
}
*/

struct Variable
{
    Variable()
        : Addr(0), OnlyAddr(false),Kind(0),Len(0),Cap(0)
    {
    }

    QString Name;
    quintptr Addr;
    bool OnlyAddr;
    QString Type;
    QString RealType;
    int Kind;
    QString Value;
    qint64 Len;
    qint64 Cap;
    QList<Variable> Children;
    QString Unreadable;

    void fromMap(const QVariantMap &map)
    {
        Name = map["name"].toString();
        Addr = map["addr"].value<quintptr>();
        OnlyAddr = map["onlyAddr"].toBool();
        Type = map["type"].toString();
        RealType = map["realType"].toString();
        Kind = map["kind"].toInt();
        Value = map["value"].toString();
        Len = map["len"].value<qint64>();
        Cap = map["cap"].value<qint64>();
        foreach (QVariant m, map["children"].toList()) {
            Variable v;
            v.fromMap(m.toMap());
            Children.push_back(v);
        }
        Unreadable = map["unreadable"].toString();
    }
};
typedef QSharedPointer<Variable> VariablePointer;

/*
type EvalScope struct {
    GoroutineID int
    Frame       int
}
*/

struct EvalScope
{
    EvalScope(int gid = -1, int frame = 0) : GoroutineID(gid), Frame(frame)
    {
    }
    int GoroutineID;
    int Frame;
    QVariantMap toMap() const
    {
        QVariantMap map;
        map["GoroutineID"] = GoroutineID;
        map["Frame"] = Frame;
        return map;
    }
};
typedef QSharedPointer<EvalScope> EvalScopePointer;

/*
// LoadConfig describes how to load values from target's memory
type LoadConfig struct {
    // FollowPointers requests pointers to be automatically dereferenced.
    FollowPointers bool
    // MaxVariableRecurse is how far to recurse when evaluating nested types.
    MaxVariableRecurse int
    // MaxStringLen is the maximum number of bytes read from a string
    MaxStringLen int
    // MaxArrayValues is the maximum number of elements read from an array, a slice or a map.
    MaxArrayValues int
    // MaxStructFields is the maximum number of fields read from a struct, -1 will read all fields.
    MaxStructFields int
}

    LongLoadConfig  = api.LoadConfig{true, 1, 64, 64, -1}
    ShortLoadConfig = api.LoadConfig{false, 0, 64, 0, 3}

*/

struct LoadConfig
{
    LoadConfig(bool followPointers=false, int maxVariableRecurse = 0, int maxStringLen = 64, int maxArrayValues = 64,int maxStructFields = 3)
        : FollowPointers(followPointers),
          MaxVariableRecurse(maxVariableRecurse),
          MaxStringLen(maxStringLen),
          MaxArrayValues(maxArrayValues),
          MaxStructFields(maxStructFields)
    {
    }

    bool FollowPointers;
    int MaxVariableRecurse;
    int MaxStringLen;
    int MaxArrayValues;
    int MaxStructFields;

    static LoadConfig Long()
    {
        static LoadConfig lc(true,1,64,64,-1);
        return lc;
    }
    static LoadConfig Short()
    {
        static LoadConfig lc(false,0,64,0,3);
        return lc;
    }
    void fromMap(const QVariantMap &map)
    {
        FollowPointers = map["FollowPointers"].toBool();
        MaxVariableRecurse = map["MaxVariableRecurse"].toInt();
        MaxStringLen = map["MaxStringLen"].toInt();
        MaxArrayValues = map["MaxArrayValues"].toInt();
        MaxStructFields = map["MaxStructFields"].toInt();
    }
    QVariantMap toMap() const
    {
        QVariantMap map;
        map["FollowPointers"] = FollowPointers;
        map["MaxVariableRecurse"] = MaxVariableRecurse;
        map["MaxStringLen"] = MaxStringLen;
        map["MaxArrayValues"] = MaxArrayValues;
        map["MaxStructFields"] = MaxStructFields;
        return map;
    }
};
typedef QSharedPointer<LoadConfig> LoadConfigPointer;

/*
type ListPackageVarsIn struct {
    Filter string
    Cfg    api.LoadConfig
}

type ListPackageVarsOut struct {
    Variables []api.Variable
}
*/


/*
// Breakpoint addresses a location at which process execution may be
// suspended.
type Breakpoint struct {
    // ID is a unique identifier for the breakpoint.
    ID int `json:"id"`
    // User defined name of the breakpoint
    Name string `json:"name"`
    // Addr is the address of the breakpoint.
    Addr uint64 `json:"addr"`
    // File is the source file for the breakpoint.
    File string `json:"file"`
    // Line is a line in File for the breakpoint.
    Line int `json:"line"`
    // FunctionName is the name of the function at the current breakpoint, and
    // may not always be available.
    FunctionName string `json:"functionName,omitempty"`

    // Breakpoint condition
    Cond string

    // tracepoint flag
    Tracepoint bool `json:"continue"`
    // retrieve goroutine information
    Goroutine bool `json:"goroutine"`
    // number of stack frames to retrieve
    Stacktrace int `json:"stacktrace"`
    // expressions to evaluate
    Variables []string `json:"variables,omitempty"`
    // LoadArgs requests loading function arguments when the breakpoint is hit
    LoadArgs *LoadConfig
    // LoadLocals requests loading function locals when the breakpoint is hit
    LoadLocals *LoadConfig
    // number of times a breakpoint has been reached in a certain goroutine
    HitCount map[string]uint64 `json:"hitCount"`
    // number of times a breakpoint has been reached
    TotalHitCount uint64 `json:"totalHitCount"`
}
*/

struct Breakpoint
{
    Breakpoint()
        : ID(0),Addr(0),Line(0),Tracepoint(false),Goroutine(false),Stacktrace(0),TotalHitCount(0)
    {
    }
    int ID;
    QString Name;
    quint64 Addr;
    QString File;
    int Line;
    QString FunctionName;
    QString Cond;
    bool Tracepoint;
    bool Goroutine;
    int Stacktrace;
    QStringList Variables;
    LoadConfigPointer pLoadArgs;
    LoadConfigPointer pLoadLocals;
    QMap<QString,quint64> HitCout;
    quint64 TotalHitCount;

    QVariantMap toMap() const
    {
        QVariantMap map;
        map["id"] = ID;
        map["name"] = Name;
        map["addr"] = Addr;
        map["file"] = File;
        map["line"] = Line;
        map["functionName"] = FunctionName;
        map["Cond"] = Cond;
        map["continue"] = Tracepoint;
        map["goroutine"] = Goroutine;
        map["stacktrace"] = Stacktrace;
        map["variables"] = Variables;
        if (pLoadArgs) {
            map["LoadArgs"] = pLoadArgs->toMap();
        } else {
            map["LoadArgs"] = QVariant();
        }
        if (pLoadLocals) {
            map["LoadLocals"] = pLoadLocals->toMap();
        } else {
            map["LoadLocals"] = QVariant();
        }
        QVariantMap m;
        QMapIterator<QString,quint64> ih(HitCout);
        while(ih.hasNext()) {
            ih.next();
            m.insert(ih.key(),ih.value());
        }
        map["hitCout"] = m;
        map["totalHitCount"] = TotalHitCount;
        return map;
    }

    void fromMap(const QVariantMap &map)
    {
        ID = map["id"].toInt();
        Name = map["name"].toString();
        Addr = map["addr"].value<quint64>();
        File = map["file"].toString();
        Line = map["line"].toInt();
        FunctionName = map["functionName"].toString();
        Cond = map["Cond"].toString();
        Tracepoint = map["continue"].toBool();
        Goroutine = map["goroutine"].toBool();
        Stacktrace = map["stacktrace"].toInt();
        Variables = map["variables"].toStringList();

        QVariantMap mla = map["LoadArgs"].toMap();
        if (!mla.isEmpty()) {
            pLoadArgs = LoadConfigPointer(new LoadConfig);
            pLoadArgs->fromMap(mla);
        }
        QVariantMap mll = map["LoadLocals"].toMap();
        if (!mll.isEmpty()) {
            pLoadLocals = LoadConfigPointer(new LoadConfig);
            pLoadLocals->fromMap(mll);
        }
        QVariantMap m = map["hitCout"].toMap();
        QMapIterator<QString,QVariant> im(m);
        while(im.hasNext()) {
            HitCout.insert(im.key(),im.value().value<quint64>());
        }
        TotalHitCount = map["totalHitCount"].value<quint64>();
    }
};
typedef QSharedPointer<Breakpoint> BreakpointPointer;

/*
type Location struct {
    PC       uint64    `json:"pc"`
    File     string    `json:"file"`
    Line     int       `json:"line"`
    Function *Function `json:"function,omitempty"`
}

type Stackframe struct {
    Location
    Locals    []Variable
    Arguments []Variable
}
*/

struct Location
{
    Location() : PC(0), Line(0)
    {
    }
    void fromMap(const QVariantMap &map)
    {
        PC = map["pc"].value<quint64>();
        File = map["file"].toString();
        Line = map["line"].toInt();
        QVariantMap mfn = map["function"].toMap();
        if (!mfn.isEmpty()) {
            pFunction = FunctionPointer(new Function);
            pFunction->fromMap(mfn);
        }
    }
    quint64 PC;
    QString File;
    int Line;
    FunctionPointer pFunction;
};
typedef QSharedPointer<Location> LocationPointer;

struct Stackframe : public Location
{
    void fromMap(const QVariantMap &map)
    {
        Location::fromMap(map);
        foreach (QVariant m, map["Locals"].toList()) {
            Variable v;
            v.fromMap(m.toMap());
            Locals.push_back(v);
        }
        foreach (QVariant m, map["Arguments"].toList()) {
            Variable v;
            v.fromMap(m.toMap());
            Arguments.push_back(v);
        }
    }
    QList<Variable> Locals;
    QList<Variable> Arguments;
};
typedef QSharedPointer<Stackframe> StackframePointer;

/*
type Goroutine struct {
    // ID is a unique identifier for the goroutine.
    ID int `json:"id"`
    // Current location of the goroutine
    CurrentLoc Location `json:"currentLoc"`
    // Current location of the goroutine, excluding calls inside runtime
    UserCurrentLoc Location `json:"userCurrentLoc"`
    // Location of the go instruction that started this goroutine
    GoStatementLoc Location `json:"goStatementLoc"`
    // ID of the associated thread for running goroutines
    ThreadID int `json:"threadID"`
}


// Informations about the current breakpoint
type BreakpointInfo struct {
    Stacktrace []Stackframe `json:"stacktrace,omitempty"`
    Goroutine  *Goroutine   `json:"goroutine,omitempty"`
    Variables  []Variable   `json:"variables,omitempty"`
    Arguments  []Variable   `json:"arguments,omitempty"`
    Locals     []Variable   `json:"locals,omitempty"`
}
*/

struct Goroutine
{
    Goroutine()
        : ID(0), ThreadId(0)
    {
    }

    void fromMap(const QVariantMap &map)
    {
        ID = map["id"].toInt();
        CurrentLoc.fromMap(map["currentLoc"].toMap());
        UserCurrentLoc.fromMap(map["userCurrentLoc"].toMap());
        GoStatementLoc.fromMap(map["goStatementLoc"].toMap());
        ThreadId = map["threadID"].toInt();
    }
    int ID;
    Location CurrentLoc;
    Location UserCurrentLoc;
    Location GoStatementLoc;
    int ThreadId;
};
typedef QSharedPointer<Goroutine> GoroutinePointer;


struct BreakpointInfo
{
    void fromMap(const QVariantMap &map)
    {
        foreach (QVariant m, map["Stacktrace"].toList()) {
            Stackframe v;
            v.fromMap(m.toMap());
            Stacktrace.push_back(v);
        }

        QVariantMap mg = map["goroutine"].toMap();
        if (!mg.isEmpty()) {
            pGoroutine = GoroutinePointer(new Goroutine);
            pGoroutine->fromMap(mg);
        }
        foreach (QVariant m, map["variables"].toList()) {
            Variable v;
            v.fromMap(m.toMap());
            Variables.push_back(v);
        }
        foreach (QVariant m, map["arguments"].toList()) {
            Variable v;
            v.fromMap(m.toMap());
            Arguments.push_back(v);
        }
        foreach (QVariant m, map["locals"].toList()) {
            Variable v;
            v.fromMap(m.toMap());
            Locals.push_back(v);
        }
    }

    QList<Stackframe> Stacktrace;
    GoroutinePointer pGoroutine;
    QList<Variable> Variables;
    QList<Variable> Arguments;
    QList<Variable> Locals;
};
typedef QSharedPointer<BreakpointInfo> BreakpointInfoPointer;

/*
// AsmInstruction represents one assembly instruction at some address
type AsmInstruction struct {
    // Loc is the location of this instruction
    Loc Location
    // Destination of CALL instructions
    DestLoc *Location
    // Text is the formatted representation of the instruction
    Text string
    // Bytes is the instruction as read from memory
    Bytes []byte
    // If Breakpoint is true a breakpoint is set at this instruction
    Breakpoint bool
    // In AtPC is true this is the instruction the current thread is stopped at
    AtPC bool
}
*/
struct AsmInstruction
{
    AsmInstruction()
        : Breakpoint(false),AtPC(false)
    {

    }

    void fromMap(const QVariantMap &map)
    {
        Loc.fromMap(map["Loc"].toMap());
        QVariantMap md = map["destLoc"].toMap();
        if (!md.isEmpty()) {
            pDestLoc = LocationPointer(new Location);
            pDestLoc->fromMap(md);
        }
        Text = map["Text"].toString();
        Bytes = map["Bytes"].toByteArray();
        Breakpoint = map["Breakpoint"].toBool();
        AtPC = map["AtPC"].toBool();
    }

    Location Loc;
    LocationPointer pDestLoc;
    QString Text;
    QByteArray Bytes;
    bool Breakpoint;
    bool AtPC;
};
typedef QSharedPointer<AsmInstruction> AsmInstructionPointer;

/*
type Register struct {
    Name  string
    Value string
}
*/

struct Register
{
    void fromMap(const QVariantMap &map)
    {
        Name = map["Name"].toString();
        Value = map["Value"].toString();
    }

    QString Name;
    QString Value;
};
typedef QSharedPointer<Register> RegisterPointer;

/*
type DiscardedBreakpoint struct {
    Breakpoint *Breakpoint
    Reason     string
}

type Checkpoint struct {
    ID    int
    When  string
    Where string
}
*/

struct DiscardedBreakpoint
{
    BreakpointPointer pBreakpoint;
    QString Reason;
    void fromMap(const QVariantMap &map)
    {
        QVariantMap mb = map["Breakpoint"].toMap();
        if (!mb.isEmpty()) {
            pBreakpoint = BreakpointPointer(new Breakpoint);
            pBreakpoint->fromMap(mb);
        }
        Reason = map["Reason"].toString();
    }
};
typedef QSharedPointer<DiscardedBreakpoint> DiscardedBreakpointPointer;

struct Checkpoint
{
    Checkpoint()
        : ID(-1)
    {
    }

    void fromMap(const QVariantMap &map)
    {
        ID = map["ID"].toInt();
        When = map["When"].toString();
        Where = map["Where"].toString();
    }

    int ID;
    QString When;
    QString Where;
};
typedef QSharedPointer<Checkpoint> CheckpointPointer;

/*
// Thread is a thread within the debugged process.
type Thread struct {
    // ID is a unique identifier for the thread.
    ID int `json:"id"`
    // PC is the current program counter for the thread.
    PC uint64 `json:"pc"`
    // File is the file for the program counter.
    File string `json:"file"`
    // Line is the line number for the program counter.
    Line int `json:"line"`
    // Function is function information at the program counter. May be nil.
    Function *Function `json:"function,omitempty"`

    // ID of the goroutine running on this thread
    GoroutineID int `json:"goroutineID"`

    // Breakpoint this thread is stopped at
    Breakpoint *Breakpoint `json:"breakPoint,omitempty"`
    // Informations requested by the current breakpoint
    BreakpointInfo *BreakpointInfo `json:"breakPointInfo,omitrempty"`
}
*/

struct Thread
{
    Thread() :
        ID(0), PC(0),Line(0),GoroutineID(0)
    {
    }

    void fromMap(const QVariantMap &map)
    {
        ID = map["id"].toInt();
        PC = map["pc"].value<quint64>();
        File = map["file"].toString();
        Line = map["line"].toInt();

        QVariantMap mf = map["function"].toMap();
        if (!mf.isEmpty()) {
            pFunction = FunctionPointer(new Function);
            pFunction->fromMap(mf);
        }
        GoroutineID = map["goroutineID"].toInt();
        QVariantMap mb = map["breakPoint"].toMap();
        if (!mb.isEmpty()) {
            pBreakpoint = BreakpointPointer(new Breakpoint);
            pBreakpoint->fromMap(mb);
        }
        QVariantMap mbi = map["breakPointInfo"].toMap();
        if (!mbi.isEmpty()) {
            pBreakpointInfo = BreakpointInfoPointer(new BreakpointInfo);
            pBreakpointInfo->fromMap(mbi);
        }
    }

    int ID;
    quint64 PC;
    QString File;
    int Line;
    FunctionPointer pFunction;
    int GoroutineID;
    BreakpointPointer pBreakpoint;
    BreakpointInfoPointer pBreakpointInfo;
};
typedef QSharedPointer<Thread> ThreadPointer;

/*
// DebuggerState represents the current context of the debugger.
type DebuggerState struct {
    // CurrentThread is the currently selected debugger thread.
    CurrentThread *Thread `json:"currentThread,omitempty"`
    // SelectedGoroutine is the currently selected goroutine
    SelectedGoroutine *Goroutine `json:"currentGoroutine,omitempty"`
    // List of all the process threads
    Threads []*Thread
    // NextInProgress indicates that a next or step operation was interrupted by another breakpoint
    // or a manual stop and is waiting to complete.
    // While NextInProgress is set further requests for next or step may be rejected.
    // Either execute continue until NextInProgress is false or call CancelNext
    NextInProgress bool
    // Exited indicates whether the debugged process has exited.
    Exited     bool `json:"exited"`
    ExitStatus int  `json:"exitStatus"`
    // When contains a description of the current position in a recording
    When string
    // Filled by RPCClient.Continue, indicates an error
    Err error `json:"-"`
}
*/

struct DebuggerState
{
    DebuggerState() : NextInProgress(false),Exited(false),ExitStatus(0)
    {
    }

    void fromMap(const QVariantMap &map)
    {
        QVariantMap mt = map["currentThread"].toMap();
        if (!mt.isEmpty()) {
            pCurrentThread = ThreadPointer(new Thread);
            pCurrentThread->fromMap(mt);
        }
        QVariantMap mg = map["currentGoroutine"].toMap();
        if (!mg.isEmpty()) {
            pSelectedGoroutine = GoroutinePointer(new Goroutine);
            pSelectedGoroutine->fromMap(mg);
        }
        foreach (QVariant m, map["Threads"].toList()) {
            Thread v;
            v.fromMap(m.toMap());
            Threads.push_back(v);
        }
        NextInProgress = map["NextInProgress"].toBool();
        Exited = map["exited"].toBool();
        ExitStatus = map["exitStatus"].toBool();
        When = map["When"].toString();
    }

    ThreadPointer pCurrentThread;
    GoroutinePointer pSelectedGoroutine;
    QList<Thread> Threads;
    bool NextInProgress;
    bool Exited;
    int ExitStatus;
    QString When;
    QString Err;
};
typedef QSharedPointer<DebuggerState> DebuggerStatePointer;

struct JsonDataIn
{
    virtual void toMap(QVariantMap &map) const = 0;
};

struct JsonDataOut
{
    virtual void fromMap(const QVariantMap &/*map*/) = 0;
};

struct EmptyJsonDataIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &/*map*/) const
    {
    }
};

struct EmptyJsonDataOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &/*map*/)
    {
    }
};


/*
type GetVersionIn struct {
}

type GetVersionOut struct {
    DelveVersion string
    APIVersion   int
}

type SetAPIVersionIn struct {
    APIVersion int
}

type SetAPIVersionOut struct {
}
*/
struct GetVersionIn : public EmptyJsonDataIn
{
};

struct GetVersionOut : public JsonDataOut
{
    GetVersionOut() : APIVersion(0)
    {
    }
    virtual void fromMap(const QVariantMap &map)
    {
        APIVersion = map["APIVersion"].toInt();
        DelveVersion = map["DelveVersion"].toString();
    }
    int APIVersion;
    QString DelveVersion;
};

struct SetAPIVersionIn : public JsonDataIn
{
    SetAPIVersionIn() : APIVersion(2)
    {
    }

    virtual void toMap(QVariantMap &map) const
    {
        map["APIVersion"] = APIVersion;
    }
    int APIVersion;
};

struct SetAPIVersionOut : public EmptyJsonDataOut
{

};

/*
type ProcessPidIn struct {
}

type ProcessPidOut struct {
    Pid int
}
*/
struct ProcessPidIn : public EmptyJsonDataIn
{
};

struct ProcessPidOut : public JsonDataOut
{
    ProcessPidOut() : Pid(-1)
    {
    }
    virtual void fromMap(const QVariantMap &map)
    {
        Pid = map["Pid"].toInt();
    }
    int Pid;
};

/*
type LastModifiedIn struct {
}

type LastModifiedOut struct {
    Time time.Time
}
*/

struct LastModifiedIn : public EmptyJsonDataIn
{
};

struct LastModifiedOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        Time = map["Time"].toDateTime();
    }
    QDateTime Time;
};

/*
type DetachIn struct {
    Kill bool
}

type DetachOut struct {
}
*/
struct DetachIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Kill"] = Kill;
    }
    bool Kill;
};
struct DetachOut : public EmptyJsonDataOut
{
};

/*
type RestartIn struct {
    // Position to restart from, if it starts with 'c' it's a checkpoint ID,
    // otherwise it's an event number. Only valid for recorded targets.
    Position string
}

type RestartOut struct {
    DiscardedBreakpoints []api.DiscardedBreakpoint
}
*/
struct RestartIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Position"] = Position;
    }
    QString Position;
};

struct RestartOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        QVariantList all = map["DiscardedBreakpoints"].toList();
        foreach (QVariant v, all) {
            DiscardedBreakpoint db;
            db.fromMap(v.toMap());
            DiscardedBreakpoints.push_back(db);
        }
    }
    QList<DiscardedBreakpoint> DiscardedBreakpoints;
};

/*
type StateIn struct {
}

type StateOut struct {
    State *api.DebuggerState
}
*/

struct StateIn : public EmptyJsonDataIn
{
};

struct StateOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        State.fromMap(map["State"].toMap());
    }

    DebuggerState State;
};

/*
// DebuggerCommand is a command which changes the debugger's execution state.
type DebuggerCommand struct {
    // Name is the command to run.
    Name string `json:"name"`
    // ThreadID is used to specify which thread to use with the SwitchThread
    // command.
    ThreadID int `json:"threadID,omitempty"`
    // GoroutineID is used to specify which thread to use with the SwitchGoroutine
    // command.
    GoroutineID int `json:"goroutineID,omitempty"`
}

type CommandOut struct {
    State api.DebuggerState
}
*/

struct DebuggerCommand : public JsonDataIn
{
    DebuggerCommand()
        : ThreadID(0),GoroutineID(0)
    {

    }
    DebuggerCommand(const QString &name)
        : Name(name), ThreadID(0),GoroutineID(0)
    {

    }

    virtual void toMap(QVariantMap &map) const
    {
        map["name"] = Name;
        map["threadID"] = ThreadID;
        map["goroutineID"] = GoroutineID;
    }

    void fromMap(const QVariantMap &map)
    {
        Name = map["name"].toString();
        ThreadID = map["threadID"].toInt();
        GoroutineID = map["goroutineID"].toInt();
    }

    QString Name;
    int ThreadID;
    int GoroutineID;
};


struct CommandOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        jsonData = map["State"];
        State.fromMap(jsonData.toMap());
    }
    QVariant      jsonData;
    DebuggerState State;
};

/*
type GetBreakpointIn struct {
    Id   int
    Name string
}

type GetBreakpointOut struct {
    Breakpoint api.Breakpoint
}
*/
struct GetBreakpointIn : public JsonDataIn
{
    GetBreakpointIn()
        : Id(0)
    {
    }

    virtual void toMap(QVariantMap &map) const
    {
        map["Id"] = Id;
        map["Name"] = Name;
    }

    int Id;
    QString Name;
};

struct GetBreakpointOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        QVariantMap m = map["Breakpoint"].toMap();
        if (!m.isEmpty()) {
            pBreakpoint = BreakpointPointer(new Breakpoint);
            pBreakpoint->fromMap(m);
        }
    }
    BreakpointPointer pBreakpoint;
};

/*
type ListBreakpointsIn struct {
}

type ListBreakpointsOut struct {
    Breakpoints []*api.Breakpoint
}
*/
struct ListBreakpointsIn : public EmptyJsonDataIn
{
};

struct ListBreakpointsOut :public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        foreach (QVariant m, map.value("Breakpoints").toList()) {
            Breakpoint v;
            v.fromMap(m.toMap());
            Breakpoints.push_back(v);
        }
    }

    QList<Breakpoint> Breakpoints;
};

/*
type ClearBreakpointIn struct {
    Id   int
    Name string
}

type ClearBreakpointOut struct {
    Breakpoint *api.Breakpoint
}
*/
struct CleartBreakpointIn : public JsonDataIn
{
    CleartBreakpointIn()
        : Id(0)
    {
    }

    virtual void toMap(QVariantMap &map) const
    {
        map["Id"] = Id;
        map["Name"] = Name;
    }

    int Id;
    QString Name;
};

struct ClearBreakpointOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        QVariantMap mb = map["Brekpoint"].toMap();
        if (!mb.isEmpty()) {
            pBreakpoint = BreakpointPointer(new Breakpoint);
            pBreakpoint->fromMap(mb);
        }
    }
    BreakpointPointer pBreakpoint;
};

/*
type AmendBreakpointIn struct {
    Breakpoint api.Breakpoint
}

type AmendBreakpointOut struct {
}
*/
struct AmendBreakpointIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Breakpoint"] = Breakpoint.toMap();
    }
    struct Breakpoint Breakpoint;
};

struct AmendBreakpointOut : public EmptyJsonDataOut
{
};

/*
type CreateBreakpointIn struct {
    Breakpoint api.Breakpoint
}

type CreateBreakpointOut struct {
    Breakpoint api.Breakpoint
}
*/
struct CreateBreakpointIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Breakpoint"] = Breakpoint.toMap();
    }
    struct Breakpoint Breakpoint;
};

struct CreateBreakpointOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        QVariantMap m = map["Breakpoint"].toMap();
        if (!m.isEmpty()) {
            pBreakpoint = BreakpointPointer(new Breakpoint);
            pBreakpoint->fromMap(map["Breakpoint"].toMap());
        }
    }
    BreakpointPointer pBreakpoint;
};

/*
type ListTypesIn struct {
    Filter string
}

type ListTypesOut struct {
    Types []string
}
*/

struct ListTypesIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Filter"] = Filter;
    }
    QString Filter;
};

struct ListTypesOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        Types = map["Types"].toStringList();
    }
    QStringList Types;
};


/*
type ListLocalVarsIn struct {
    Scope api.EvalScope
    Cfg   api.LoadConfig
}

type ListLocalVarsOut struct {
    Variables []api.Variable
}
*/
struct ListLocalVarsIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Scope"] = Scope.toMap();
        map["Cfg"] = Cfg.toMap();
    }
    EvalScope Scope;
    LoadConfig Cfg;
};

struct ListLocalVarsOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        QVariantList all = map["Variables"].toList();
        foreach (QVariant e, all) {
            Variable v;
            v.fromMap(e.toMap());
            Variables.push_back(v);
        }
    }
    QList<Variable> Variables;
};

struct ListPackageVarsIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Filter"] = Filter;
        map["Cfg"] = Cfg.toMap();
    }
    QString Filter;
    LoadConfig Cfg;
};

struct ListPackageVarsOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        foreach (QVariant e, map["Variables"].toList()) {
            Variable v;
            v.fromMap(e.toMap());
            Variables.push_back(v);
        }
    }
    QList<Variable> Variables;
};

/*
type CancelNextIn struct {
}

type CancelNextOut struct {
}
*/
struct CancelNextIn : public EmptyJsonDataIn
{
};
struct CancelNextOut : public EmptyJsonDataOut
{
};

/*
type ListThreadsIn struct {
}

type ListThreadsOut struct {
    Threads []*api.Thread
}
*/
struct ListThreadsIn : public EmptyJsonDataIn
{
};

struct ListThreadsOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        foreach (QVariant m, map["Threads"].toList()) {
            Thread v;
            v.fromMap(m.toMap());
            Threads.push_back(v);
        }
    }
    QList<Thread> Threads;
};

/*
type GetThreadIn struct {
    Id int
}

type GetThreadOut struct {
    Thread *api.Thread
}
*/
struct GetThreadIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Id"] = Id;
    }
    int Id;
};

struct GetThreadOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        QVariantMap m = map["Thread"].toMap();
        if (!m.isEmpty()) {
            pThread = ThreadPointer(new Thread);
            pThread->fromMap(m);
        }
    }

    ThreadPointer pThread;
};

/*
type EvalIn struct {
    Scope api.EvalScope
    Expr  string
    Cfg   *api.LoadConfig
}

type EvalOut struct {
    Variable *api.Variable
}
*/

struct EvalIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Scope"] = Scope.toMap();
        map["Expr"] = Expr;
        map["Cfg"] = Cfg.toMap();
    }
    EvalScope Scope;
    QString Expr;
    LoadConfig Cfg;
};

struct EvalOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        QVariantMap m = map["Variable"].toMap();
        if (!m.isEmpty()) {
            pVariable = VariablePointer(new Variable);
            pVariable->fromMap(m);
        }
    }
    VariablePointer pVariable;
};

/*
type SetIn struct {
    Scope  api.EvalScope
    Symbol string
    Value  string
}

type SetOut struct {
}
*/
struct SetIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Scope"] = Scope.toMap();
        map["Symbol"] = Symbol;
        map["Value"] = Value;
    }

    EvalScope Scope;
    QString Symbol;
    QString Value;
};
struct SetOut : public EmptyJsonDataOut
{
};
/*
type ListSourcesIn struct {
    Filter string
}

type ListSourcesOut struct {
    Sources []string
}
*/
struct ListSourcesIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Filter"] = Filter;
    }
    QString Filter;
};
struct ListSourcesOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        Sources = map["Sources"].toStringList();
    }
    QStringList Sources;
};

/*
type ListFunctionsIn struct {
    Filter string
}

type ListFunctionsOut struct {
    Funcs []string
}
*/
struct ListFunctionsIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Filter"] = Filter;
    }
    QString Filter;
};
struct ListFunctionsOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        Funcs = map["Funcs"].toStringList();
    }
    QStringList Funcs;
};

/*
type ListRegistersIn struct {
    ThreadID  int
    IncludeFp bool
}

type ListRegistersOut struct {
    Registers string
    Regs      api.Registers
}
*/

struct ListRegistersIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["ThreadID"] = ThreadID;
        map["IncludeFp"] = IncludeFp;
    }
    int ThreadID;
    bool IncludeFp;
};

struct ListRegistersOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap &map)
    {
        Registers = map["Registers"].toString();
        foreach (QVariant m, map["Regs"].toList()) {
            Register v;
            v.fromMap(m.toMap());
            Regs.push_back(v);
        }
    }
    QString Registers;
    QList<Register> Regs;
};

/*
type ListFunctionArgsIn struct {
    Scope api.EvalScope
    Cfg   api.LoadConfig
}

type ListFunctionArgsOut struct {
    Args []api.Variable
}
*/

struct ListFunctionArgsIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Scope"] = Scope.toMap();
        map["Cfg"] = Cfg.toMap();
    }
    EvalScope Scope;
    LoadConfig Cfg;
};

struct ListFunctionArgsOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        foreach (QVariant e, map["Args"].toList()) {
            Variable v;
            v.fromMap(e.toMap());
            Variables.push_back(v);
        }
    }
    QList<Variable> Variables;
};

/*
type ListGoroutinesIn struct {
}

type ListGoroutinesOut struct {
    Goroutines []*api.Goroutine
}
*/

struct ListGoroutinesIn : public EmptyJsonDataIn
{
};

struct ListGoroutinesOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        foreach (QVariant e, map["Goroutines"].toList()) {
            Goroutine v;
            v.fromMap(e.toMap());
            Goroutines.push_back(v);
        }
    }
    QList<Goroutine> Goroutines;
};

/*
type StacktraceIn struct {
    Id    int
    Depth int
    Full  bool
    Cfg   *api.LoadConfig
}

type StacktraceOut struct {
    Locations []api.Stackframe
}
*/
struct StacktraceIn : public EmptyJsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Id"] = Id;
        map["Depth"] = Depth;
        map["Full"] = Full;
        map["Cfg"] = Cfg.toMap();
    }
    int Id;
    int Depth;
    bool Full;
    LoadConfig Cfg;
};

struct StacktraceOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        foreach (QVariant e, map["Locations"].toList()) {
            Stackframe v;
            v.fromMap(e.toMap());
            Locations.push_back(v);
        }
    }
    QList<Stackframe> Locations;
};

/*
type AttachedToExistingProcessIn struct {
}

type AttachedToExistingProcessOut struct {
    Answer bool
}
*/

struct AttachedToExistingProcessIn : public EmptyJsonDataIn
{
};

struct AttachedToExistingProcessOut : public JsonDataOut
{
    AttachedToExistingProcessOut()
        : Answer(false)
    {
    }

    virtual void fromMap(const QVariantMap & map)
    {
        Answer = map["Answer"].toBool();
    }
    bool Answer;
};

/*
type FindLocationIn struct {
    Scope api.EvalScope
    Loc   string
}

type FindLocationOut struct {
    Locations []api.Location
}
*/

struct FindLocationIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Scope"] = Scope.toMap();
        map["Loc"] = Loc;
    }
    EvalScope Scope;
    QString Loc;
};

struct FindLocationOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        foreach (QVariant e, map["Locations"].toList()) {
            Location v;
            v.fromMap(e.toMap());
            Locations.push_back(v);
        }
    }
    QList<Location> Locations;
};

enum AssemblyFlavour
{
    GNUFlavour = 0,
    IntelFlavour = 1
};

/*
type DisassembleIn struct {
    Scope          api.EvalScope
    StartPC, EndPC uint64
    Flavour        api.AssemblyFlavour
}

type DisassembleOut struct {
    Disassemble api.AsmInstructions
}
*/
struct DisassembleIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Scope"] = Scope.toMap();
        map["StartPC"] = StartPC;
        map["EndPC"] = EndPC;
        map["Flavour"] = Flavour;
    }
    EvalScope Scope;
    quint64 StartPC;
    quint64 EndPC;
    AssemblyFlavour Flavour;
};

struct DisassembleOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        foreach (QVariant e, map["Disassemble"].toList()) {
            AsmInstruction v;
            v.fromMap(e.toMap());
            Disassemble.push_back(v);
        }
    }
    QList<AsmInstruction> Disassemble;
};

/*
type RecordedIn struct {
}

type RecordedOut struct {
    Recorded       bool
    TraceDirectory string
}
*/
struct RecordedIn : public EmptyJsonDataIn
{
};

struct RecordedOut : public JsonDataOut
{
    RecordedOut() : Recorded(false)
    {
    }

    virtual void fromMap(const QVariantMap & map)
    {
        Recorded = map["Recorded"].toBool();
        TraceDirectory = map["TraceDirectory"].toString();
    }
    bool Recorded;
    QString TraceDirectory;
};

/*
type CheckpointIn struct {
    Where string
}

type CheckpointOut struct {
    ID int
}
*/

struct CheckpointIn : public JsonDataIn
{
    virtual void toMap(QVariantMap & map) const
    {
        map["Where"] = Where;
    }
    QString Where;
};

struct CheckpointOut : public JsonDataOut
{
    CheckpointOut() : ID(-1)
    {
    }

    virtual void fromMap(const QVariantMap & map)
    {
        ID = map["ID"].toInt();
    }
    int ID;
};

/*
type ListCheckpointsIn struct {
}

type ListCheckpointsOut struct {
    Checkpoints []api.Checkpoint
}
*/
struct ListCheckpointsIn : public EmptyJsonDataIn
{
};

struct ListCheckpointsOut : public JsonDataOut
{
    virtual void fromMap(const QVariantMap & map)
    {
        foreach (QVariant e, map["Checkpoints"].toList()) {
            Checkpoint v;
            v.fromMap(e.toMap());
            Checkpoints.push_back(v);
        }
    }
    QList<Checkpoint> Checkpoints;
};

/*
type ClearCheckpointIn struct {
    ID int
}

type ClearCheckpointOut struct {
}
*/

struct ClearCheckpointIn : public JsonDataIn
{
    virtual void toMap(QVariantMap & map) const
    {
        map["ID"] = ID;
    }
    int ID;
};

struct ClearCheckpointOut : public EmptyJsonDataOut
{
};

#endif // DLVTYPES_H
