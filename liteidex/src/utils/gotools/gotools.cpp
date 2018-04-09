#include "gotools.h"

GopherLib::GopherLib(QObject *parent) : QObject(parent)
{
    lib.setFileName("libgopher");
    fnInvokeAsync = (InvokeAsyncFunc)lib.resolve("InvokeAsync");
    fnInvoke = (InvokeFunc)lib.resolve("Invoke");
    fnSetenv = (SetenvFunc)lib.resolve("Setenv");
    fnSetBuildEnv = (SetBuildEnvFunc)lib.resolve("SetBuildEnv");
    fnClearBuildEnv = (ClearBuildEnvFunc)lib.resolve("ClearBuildEnv");
}
