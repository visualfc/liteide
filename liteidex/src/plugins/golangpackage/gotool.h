/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
#include "liteenvapi/liteenvapi.h"
#include <QProcess>

inline QStringList liteGopath(LiteApi::IApplication *app)
{
    QStringList pathList;
    QStringList list =  app->settings()->value("liteide/gopath").toStringList();
    foreach(QString path, list) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    return pathList;
}

inline void setLiteGopath(LiteApi::IApplication *app, const QStringList &pathList)
{
    app->settings()->setValue("liteide/gopath",pathList);
}

inline QStringList sysGopath(LiteApi::IApplication *app)
{
    QProcessEnvironment env = LiteApi::getSysEnvironment(app);
//    QString goroot = env.value("GOROOT");
    QStringList pathList;
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    foreach (QString path, env.value("GOPATH").split(sep,qtSkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    return pathList;
}


class Process;
class GoTool : public QObject
{
    Q_OBJECT
public:
    explicit GoTool(LiteApi::IApplication *app,QObject *parent = 0);
    virtual ~GoTool();
    void setProcessEnvironment(const QProcessEnvironment &environment);
    void setLiteGopath(const QStringList &pathList);
    void kill();
    void setWorkDir(const QString &dir);
    QString workDir() const;
    void start_list_json();
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
    Process *m_process;
    QByteArray m_stdOutput;
    QByteArray m_stdError;
};

#endif // GOTOOL_H
