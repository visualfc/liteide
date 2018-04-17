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
// Module: litedebugapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEDEBUGAPI_H
#define LITEDEBUGAPI_H

#include "liteapi/liteapi.h"
#include <QAbstractItemModel>

namespace LiteApi {

enum DEBUG_MODEL_TYPE{
    ASYNC_MODEL = 1,
    VARS_MODEL,
    WATCHES_MODEL,
    CALLSTACK_MODEL,
    BREAKPOINTS_MODEL,
    THREADS_MODEL,
    LIBRARY_MODEL
};

enum DEBUG_LOG_TYPE {
    DebugConsoleLog = 1,
    DebugApplationLog,
    DebugRuntimeLog,
    DebugErrorLog
};

enum DEBUG_EDITOR_MARKTYPE {
    BreakPointMarkType = 2000,
    CurrentLineMarkType = 3000
};

class IDebugger : public QObject
{
    Q_OBJECT
public:
    IDebugger(QObject *parent = 0): QObject(parent) {}
    virtual ~IDebugger() {}
public:
    virtual QString mimeType() const = 0;
    virtual QAbstractItemModel *debugModel(DEBUG_MODEL_TYPE type) = 0;
    virtual void setWorkingDirectory(const QString &dir) = 0;
    virtual void setEnvironment (const QStringList &environment) = 0;
    virtual bool start(const QString &cmd, const QString &arguments) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() = 0;
    virtual void stepOver() = 0;
    virtual void stepInto() = 0;
    virtual void stepOut() = 0;
    virtual void continueRun() = 0;
    virtual void runToLine(const QString &fileName, int line) = 0;
    virtual void command(const QByteArray &cmd) = 0;
    virtual void enterAppText(const QString &text) = 0;
    virtual void enterDebugText(const QString &text) = 0;
    virtual void expandItem(QModelIndex index, DEBUG_MODEL_TYPE type) = 0;
    virtual void setInitBreakTable(const QMultiMap<QString,int> &bks) = 0;
    virtual void setInitWatchList(const QStringList &names) = 0;
    virtual void insertBreakPoint(const QString &fileName, int line) = 0;
    virtual void removeBreakPoint(const QString &fileName, int line) = 0;
    virtual void createWatch(const QString &var) = 0;
    virtual void removeWatch(const QString &var) = 0;
    virtual void removeAllWatch() = 0;
    virtual void showFrame(QModelIndex index) = 0;
signals:
    void debugStarted();
    void debugStoped();
    void debugLoaded();
    void debugLog(LiteApi::DEBUG_LOG_TYPE type, const QString &log);
    void setExpand(LiteApi::DEBUG_MODEL_TYPE type, const QModelIndex &index, bool expanded);
    void setCurrentLine(const QString &fileName, int line);
    void setFrameLine(const QString &fileName, int line);
    void watchCreated(const QString &watch,const QString &name);
    void watchRemoved(const QString &watch);
};

class IDebuggerManager : public IManager
{
    Q_OBJECT
public:
    IDebuggerManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addDebugger(IDebugger *debug) = 0;
    virtual void removeDebugger(IDebugger *debug) = 0;
    virtual IDebugger *findDebugger(const QString &mimeType) = 0;
    virtual QList<IDebugger*> debuggerList() const = 0;
    virtual void setCurrentDebugger(IDebugger *debug) = 0;
    virtual IDebugger *currentDebugger() = 0;
signals:
    void currentDebuggerChanged(LiteApi::IDebugger*);
};

inline IDebuggerManager *getDebugManager(LiteApi::IApplication *app)
{
    return LiteApi::findExtensionObject<IDebuggerManager*>(app,"LiteApi.IDebuggerManager");
}

class ILiteDebug : public IObject
{
    Q_OBJECT
public:
    ILiteDebug(QObject *parent) : IObject(parent)
    {
    }
public:
    virtual IDebuggerManager *debugManager() const = 0;
    virtual void startDebug(const QString &cmd, const QString &args, const QString &work) = 0;
    virtual bool isRunning() const = 0;
public slots:
    virtual void continueRun() = 0;
    virtual void runToLine() = 0;
    virtual void stopDebug() = 0;
    virtual void stepOver() = 0;
    virtual void stepInto() = 0;
    virtual void stepOut() = 0;
    virtual void showLine() = 0;
    virtual void toggleBreakPoint() = 0;
    virtual void removeAllBreakPoints() = 0;
signals:
    void debugBefore();
    void debugEnd();
};

inline ILiteDebug *getLiteDebug(LiteApi::IApplication *app)
{
    return LiteApi::findExtensionObject<ILiteDebug*>(app,"LiteApi.ILiteDebug");
}

} //namespace LiteApi

#endif //LITEDEBUGAPI_H

