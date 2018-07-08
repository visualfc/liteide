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
// Module: processex.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEAPI_PROCESSEX_H
#define LITEAPI_PROCESSEX_H

#include <QProcess>
#include <QVariant>
#include "liteapi/liteapi.h"

class Process : public QProcess
{
    Q_OBJECT
public:
    Process(QObject *parent);
    virtual ~Process();
    bool isRunning() const;
    bool isStop() const;
    void stop(int ms);
    void stopAndWait(int termMs, int finishMs);
    void startEx(const QString &cmd, const QString &args);
    static bool startDetachedExAndHide(const QString& cmd, const QStringList &args);
    static bool startDetachedEx(const QString &cmd, const QStringList &args, const QString &workDir);
public:
    void setUserData(int id, const QVariant &data);
    QVariant userData(int id) const;
protected:
    QMap<int,QVariant> m_idVarMap;
};

class ProcessEx : public Process
{
    Q_OBJECT
public:
    ProcessEx(QObject *parent);
signals:
    void extOutput(const QByteArray &data,bool bError);
    void extFinish(bool error,int code, QString msg);
protected slots:
    void slotStateChanged(QProcess::ProcessState);
    void slotError(QProcess::ProcessError);
    void slotFinished(int,QProcess::ExitStatus);
    void slotReadOutput();
    void slotReadError();
public:
    static QString exitStatusText(int code,QProcess::ExitStatus status);
    static QString processErrorText(QProcess::ProcessError code);
private:
    bool m_suppressFinish;
};

void SendProcessCtrlC(QProcess *);

class LiteProcess : public QProcess
{
public:
    LiteProcess(LiteApi::IApplication *app, QObject *parent);
    void setUseCtrlC(bool use);
    void startEx(const QString &cmd, const QString &args);
    void interrupt();
    void terminate();
    bool isStop() const;
protected:
    LiteApi::IApplication *m_liteApp;
    bool m_useCtrlC;
};

#endif // LITEAPI_PROCESSEX_H
