#ifndef DLVTYPES_H
#define DLVTYPES_H

#include <QVariant>
#include <QDateTime>
#include <QSharedPointer>
#include "varianthelper.h"
#include "qjsonrpc/src/json/qjsonobject.h"

#include <QDebug>

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
    QString Name;
    quint64 Value;
    qint8 Type;
    quint64 GoType;
};

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
        QList<QVariant> childList = map["children"].toList();
        foreach (QVariant child, childList) {
            Variable v;
            v.fromMap(child.toMap());
            Children.push_back(v);
        }
        Unreadable = map["unreadable"].toString();
    }
};

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
    static QVariantMap toMap(const EvalScope &v)
    {
        QVariantMap map;
        map["GoroutineID"] = v.GoroutineID;
        map["Frame"] = v.Frame;
        return map;
    }
};

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
    bool FollowPointers;
    int MaxVariableRecurse;
    int MaxStringLen;
    int MaxArrayValues;
    int MaxStructFields;
    static QVariantMap toMap(const LoadConfig &v)
    {
        QVariantMap map;
        map["FollowPointers"] = v.FollowPointers;
        map["MaxVariableRecurse"] = v.MaxVariableRecurse;
        map["MaxStringLen"] = v.MaxStringLen;
        map["MaxArrayValues"] = v.MaxArrayValues;
        map["MaxStructFields"] = v.MaxStructFields;
        return map;
    }
};


inline LoadConfig LongLoadConfig()
{
    return LoadConfig{true,1,64,64,-1};
}

inline LoadConfig ShortLoadConfig()
{
    return LoadConfig{false,0,64,0,3};
}


/*
type ListPackageVarsIn struct {
    Filter string
    Cfg    api.LoadConfig
}

type ListPackageVarsOut struct {
    Variables []api.Variable
}
*/

struct ListPackageVarsIn : public JsonDataIn
{
    virtual void toMap(QVariantMap &map) const
    {
        map["Filter"] = Filter;
        map["Cfg"] = LoadConfig::toMap(Cfg);
    }
    QString Filter;
    LoadConfig Cfg;
};

struct ListPackageVarsOut : public JsonDataOut
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

struct Breakpoint {
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
    LoadConfig LoadLocals;
    QMap<QString,quint64> HitCout;
    quint64 TotalHitCount;
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
        map["Scope"] = EvalScope::toMap(Scope);
        map["Cfg"] = LoadConfig::toMap(Cfg);
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


#endif // DLVTYPES_H
