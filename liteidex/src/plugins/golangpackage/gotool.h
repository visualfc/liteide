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
// Module: gotool.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOTOOL_H
#define GOTOOL_H

#include "liteapi/liteapi.h"
#include <QProcess>

class GoTool : public QObject
{
    Q_OBJECT
public:
    explicit GoTool(LiteApi::IApplication *app,QObject *parent = 0);
    virtual ~GoTool();
    void reloadEnv();
    bool exists();
    QStringList sysGopath() const;
    QStringList liteGopath() const;
    void setProcessEnvironment(const QProcessEnvironment &environment);
    void setLiteGopath(const QStringList &pathList);
    bool isRuning() const;
    void kill();
    void setWorkDir(const QString &dir);
    QString workDir() const;
    QString gotool() const { return m_gotool; }
    void start(const QStringList &args);
    QByteArray stdOutputData() const { return m_stdOutput; }
    QString stdOutputUtf8() const {
        return QString::fromUtf8(m_stdOutput,m_stdOutput.size());
    }
    QByteArray stdError() const { return m_stdError; }
signals:
    void error(QProcess::ProcessError);
    void finished(int,QProcess::ExitStatus);
protected slots:
    void readError();
    void readOutput();
protected:
    LiteApi::IApplication *m_liteApp;
    QProcess *m_process;
    QString m_gotool;
    QByteArray m_stdOutput;
    QByteArray m_stdError;
};

#endif // GOTOOL_H
