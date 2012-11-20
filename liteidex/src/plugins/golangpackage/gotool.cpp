/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: gotool.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-19
// $Id: gotool.cpp,v 1.0 2012-4-19 visualfc Exp $

#include "gotool.h"
#include "liteenvapi/liteenvapi.h"
#include "fileutil/fileutil.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GoTool::GoTool(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_process = new QProcess(this);
    connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readError()));
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readOutput()));
    connect(m_process,SIGNAL(error(QProcess::ProcessError)),this,SIGNAL(error(QProcess::ProcessError)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SIGNAL(finished(int,QProcess::ExitStatus)));
}

GoTool::~GoTool()
{
    kill();
    delete m_process;
}

QStringList GoTool::liteGopath() const
{
    return m_liteApp->settings()->value("liteide/gopath").toStringList();
}

void GoTool::setLiteGopath(const QStringList &pathList)
{
    m_liteApp->settings()->setValue("liteide/gopath",pathList);
}

QStringList GoTool::sysGopath() const
{
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    QString goroot = env.value("GOROOT");
    QStringList pathList;
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    pathList.append(goroot);
    foreach (QString path, env.value("GOPATH").split(sep,QString::SkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    return pathList;
}

bool GoTool::exists()
{
    return QFileInfo(m_gotool).exists();
}

void GoTool::reloadEnv()
{
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    QString goroot = env.value("GOROOT");
    QString gobin = env.value("GOBIN");
    if (!goroot.isEmpty() && gobin.isEmpty()) {
        gobin = goroot+"/bin";
    }
    QString gotool = FileUtil::findExecute(gobin+"/go");
    if (gotool.isEmpty()) {
        gotool = FileUtil::lookPath("go",env,true);
    }
    m_process->setProcessEnvironment(env);
    m_gotool = gotool;
}

bool GoTool::isRuning() const
{
    return m_process->state() == QProcess::Running;
}

void GoTool::kill()
{
    if (m_process->state() != QProcess::NotRunning) {
        if (!m_process->waitForFinished(200)) {
            m_process->kill();
        }
    }
}

void  GoTool::setProcessEnvironment(const QProcessEnvironment &environment)
{
    m_process->setProcessEnvironment(environment);
}

void GoTool::setWorkDir(const QString &dir)
{
    m_process->setWorkingDirectory(dir);
}

QString GoTool::workDir() const
{
    return m_process->workingDirectory();
}

void GoTool::start(const QStringList &args)
{
    this->kill();
    m_stdOutput.clear();
    m_stdError.clear();
    m_process->start(m_gotool,args);
}

void GoTool::readError()
{
    m_stdError.append(m_process->readAllStandardError());
}

void GoTool::readOutput()
{
    m_stdOutput.append(m_process->readAllStandardOutput());
}
