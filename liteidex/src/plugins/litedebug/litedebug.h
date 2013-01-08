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
// Module: litedebug.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEDEBUG_H
#define LITEDEBUG_H

#include "liteapi/liteapi.h"
#include "litedebugapi/litedebugapi.h"
#include "liteenvapi/liteenvapi.h"
#include "litebuildapi/litebuildapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "textoutput/textoutput.h"

class DebugManager;
class DebugWidget;
class QLabel;

struct CurrentLine
{
    CurrentLine() : line(0)
    {
    }
    QString  fileName;
    int      line;
};

class LiteDebug : public LiteApi::ILiteDebug
{
    Q_OBJECT
public:
    explicit LiteDebug(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~LiteDebug();
    QWidget *widget();
    bool canDebug(LiteApi::IEditor *editor) const;
    virtual LiteApi::IDebuggerManager *debugManager() const;
    virtual void startDebug(const QString &cmd, const QString &args, const QString &work);
    virtual bool isRunning() const;
signals:
    void debugVisible(bool);
public slots:
    void appLoaded();
    void editorCreated(LiteApi::IEditor*);
    void editorAboutToClose(LiteApi::IEditor*);
    void currentEditorChanged(LiteApi::IEditor*);
    void startDebugExternal();
    virtual void startDebug();
    virtual void continueRun();
    virtual void runToLine();
    virtual void stopDebug();
    virtual void stepOver();
    virtual void stepInto();
    virtual void stepOut();
    virtual void showLine();
    virtual void toggleBreakPoint();
    virtual void removeAllBreakPoints();
    virtual void enterAppInputText(QString);
protected slots:
    void setDebugger(LiteApi::IDebugger*);
    void debugLoaded();
    void debugLog(LiteApi::DEBUG_LOG_TYPE type, const QString &log);
    void debugStarted();
    void debugStoped();
    void setCurrentLine(const QString &fileName, int line);
protected:
    void clearLastLine();
    LiteApi::IApplication *m_liteApp;
    LiteApi::IEnvManager *m_envManager;
    LiteApi::ILiteBuild *m_liteBuild;
    LiteApi::IDebugger *m_debugger;
    DebugManager *m_manager;
    QWidget      *m_widget;
    DebugWidget  *m_dbgWidget;
    QMenu        *m_debugMenu;
    TextOutput   *m_output;
    QAction      *m_outputAct;
    QAction *m_startDebugExternal;
    QAction *m_startDebugAct;
    QAction *m_stopDebugAct;
    QAction *m_showLineAct;
    QAction *m_stepIntoAct;
    QAction *m_stepOverAct;
    QAction *m_stepOutAct;
    QAction *m_continueAct;
    QAction *m_runToLineAct;
    QAction *m_switchBreakAct;
    QAction *m_removeAllBreakAct;
    QStringList m_debugMimeTypes;
    QString  m_debugInfoId;
    CurrentLine m_lastLine;
    QMultiMap<QString,int> m_fileBpMap;
};

#endif // LITEDEBUG_H
