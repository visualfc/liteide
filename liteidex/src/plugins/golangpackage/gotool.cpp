/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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

#include "gotool.h"
#include "liteenvapi/liteenvapi.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
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
    m_process = new Process(this);
    connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readError()));
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readOutput()));
    connect(m_process,SIGNAL(error(QProcess::ProcessError)),this,SIGNAL(error(QProcess::ProcessError)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SIGNAL(finished(int,QProcess::ExitStatus)));
}

GoTool::~GoTool()
{    
    kill();
}

QStringList GoTool::liteGopath() const
{
    QStringList pathList;
    QStringList list =  m_liteApp->settings()->value("liteide/gopath").toStringList();
    foreach(QString path, list) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    return pathList;
}

void GoTool::setLiteGopath(const QStringList &pathList)
{
    m_liteApp->settings()->setValue("liteide/gopath",pathList);
}

QStringList GoTool::sysGopath() const
{
    QProcessEnvironment env = LiteApi::getSysEnvironment(m_liteApp);
//    QString goroot = env.value("GOROOT");
    QStringList pathList;
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    foreach (QString path, env.value("GOPATH").split(sep,QString::SkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    return pathList;
}

void GoTool::kill()
{
    m_process->stopAndWait(100,200);
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

void GoTool::start_list_json()
{
    this->kill();

    m_stdOutput.clear();
    m_stdError.clear();

    QString cmd = LiteApi::getGotools(m_liteApp);
    QStringList args;
    args << "pkgs" << "-list" << "-json";

    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    m_process->setEnvironment(env.toStringList());
    m_process->start(cmd,args);
}

void GoTool::readError()
{
    m_stdError.append(m_process->readAllStandardError());
}

void GoTool::readOutput()
{
    m_stdOutput.append(m_process->readAllStandardOutput());
}
