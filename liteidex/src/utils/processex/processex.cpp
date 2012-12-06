/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: processex.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "processex.h"
#include <QMap>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end



static QString exitStatusText(QProcess::ExitStatus code)
{
    static QString text;
    switch (code) {
    case QProcess::NormalExit:
        text = "process exited normally";
        break;
    case QProcess::CrashExit:
        text = "process crashed";
        break;
    default:
        text = "process exited unknown";
    }
    return text;
}

static QString processErrorText(QProcess::ProcessError code)
{
    static QString text;
    switch (code) {
    case QProcess::FailedToStart:
        text = "process failed to start";
        break;
    case QProcess::Crashed:
        text = "process crashed some time after starting successfully";
        break;
    case QProcess::Timedout:
        text = "last waitFor...() function timed out";
        break;
    case QProcess::ReadError:
        text = "error occurred when attempting to read from the process";
        break;
    case QProcess::WriteError:
        text = "error occurred when attempting to write to the process";
        break;
    case QProcess::UnknownError:
    default:
        text = "unknown error occurred";
    }
    return text;
}

ProcessEx::ProcessEx(QObject *parent)
    : QProcess(parent)
{
    connect(this,SIGNAL(readyReadStandardOutput()),this,SLOT(slotReadOutput()));
    connect(this,SIGNAL(readyReadStandardError()),this,SLOT(slotReadError()));
    connect(this,SIGNAL(error(QProcess::ProcessError)),this,SLOT(slotError(QProcess::ProcessError)));
    connect(this,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(slotFinished(int,QProcess::ExitStatus)));
}

ProcessEx::~ProcessEx()
{
    if (isRuning()) {
        this->kill();
    }
}

bool ProcessEx::isRuning() const
{
    return this->state() == QProcess::Running;
}

void ProcessEx::setUserData(int id, const QVariant &data)
{
    m_idVarMap.insert(id,data);
}

QVariant ProcessEx::userData(int id) const
{
    return m_idVarMap.value(id);
}


void ProcessEx::slotError(QProcess::ProcessError error)
{
    emit extFinish(true,-1,processErrorText(error));
}

void ProcessEx::slotFinished(int code,QProcess::ExitStatus status)
{
    emit extFinish(false,code,exitStatusText(status));
}

void ProcessEx::slotReadOutput()
{
    emit extOutput(this->readAllStandardOutput(),false);
}

void ProcessEx::slotReadError()
{
    emit extOutput(this->readAllStandardError(),true);
}

void ProcessEx::startEx(const QString &cmd, const QString &args)
{
#ifdef Q_OS_WIN
    this->setNativeArguments(args);
    if (cmd.indexOf(" ")) {
        this->start("\""+cmd+"\"");
    } else {
        this->start(cmd);
    }
#else
    this->start(cmd+" "+args);
#endif
}
