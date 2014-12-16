/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: envmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "envmanager.h"
#include "liteenv_global.h"
#include "fileutil/fileutil.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QToolBar>
#include <QMenu>
#include <QComboBox>
#include <QLabel>
#include <QAction>
#include <QSysInfo>
#include <QProcess>
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

Env::Env(QObject *parent) :
    LiteApi::IEnv(parent)
{
    m_env = QProcessEnvironment::systemEnvironment();
    m_process = 0;
}

QString Env::id() const
{
    return m_id;
}

QString Env::filePath() const
{
    return m_filePath;
}

QProcessEnvironment& Env::environment()
{
    return m_env;
}

QStringList Env::orgEnvLines() const
{
    return m_orgEnvLines;
}

QMap<QString, QString> Env::goEnvMap() const
{
    return m_goEnvMap;
}

void Env::reload()
{
    if (m_filePath.isEmpty()) {
        return;
    }
    QFile f(m_filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    loadEnvFile(&f);
    f.close();
    loadGoEnv();
}

void Env::loadGoEnv()
{
    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdout()));
        connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readStderr()));
    }
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(100);
    }
    m_goEnvMap.clear();
    QString gocmd = FileUtil::lookPath("go",m_env,false);
    if (gocmd.isEmpty()) {
        QString goroot = m_env.value("GOROOT");
        if (goroot.isEmpty()) {
            goroot = LiteApi::getDefaultGOROOT();
        }
        gocmd = FileUtil::lookPathInDir("go",goroot+"/bin");
        if (gocmd.isEmpty()) {
            emit goenvError(m_id,"cannot find go in PATH");
            return;
        }
    }
    m_process->setProcessEnvironment(m_env);
    m_process->start(gocmd,QStringList() << "env");
}

void Env::loadEnvFile(QIODevice *dev)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_orgEnvLines.clear();
#ifdef Q_OS_WIN
    QRegExp rx("\\%([\\w]+)\\%");
#else
    QRegExp rx("\\$([\\w]+)");
#endif
    while (!dev->atEnd()) {
        QString line = QString::fromUtf8(dev->readLine().trimmed());
        if (line.indexOf("#") == 0) {
            continue;
        }
        int pos = line.indexOf("=");
        if (pos == -1) {
            continue;
        }
        m_orgEnvLines.append(line);
        QString key = line.left(pos).trimmed();
        QString value = line.right(line.length()-pos-1).trimmed();
        QStringList cap0;
        QStringList cap1;
        pos = 0;
        while ((pos = rx.indexIn(value, pos)) != -1) {
             cap0 << rx.cap(0);
             cap1 << rx.cap(1);
             pos += rx.matchedLength();
        }
        for (int i = 0; i < cap0.size(); i++) {
            if (env.contains(cap1.at(i))) {
                value.replace(cap0.at(i),env.value(cap1.at(i)));
            }
        }
        env.insert(key,value);
    }
    m_env = env;
}

void Env::loadEnv(EnvManager *manager, const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }

    Env *env = new Env(manager);
    env->m_filePath = filePath;
    env->m_id = QFileInfo(filePath).baseName();
    env->loadEnvFile(&f);
    f.close();
    manager->addEnv(env);
}

void Env::readStdout()
{
    QByteArray data = m_process->readAllStandardOutput();
// set GOARCH=amd64
// GOARCH="amd64"
    foreach (QByteArray line, data.split('\n')) {
        QString info = QString::fromUtf8(line).trimmed();
        if (info.startsWith("set ")) {
            info = info.mid(4);
        }
        int index = info.indexOf("=");
        if (index > 0) {
            QString key = info.left(index);
            QString value = info.right(info.length()-index-1);
            if (value.startsWith("\"") && value.endsWith("\"")) {
                value = value.mid(1,value.length()-2);
            }
            m_goEnvMap[key] = value;
        }
    }
}

void Env::readStderr()
{
    QByteArray data = m_process->readAllStandardError();
    emit goenvError(m_id,QString::fromUtf8(data));
}

EnvManager::EnvManager(QObject *parent)
    : LiteApi::IEnvManager(parent),
      m_curEnv(0),m_toolBar(0)
{
}

EnvManager::~EnvManager()
{
    if (m_toolBar) {
        m_liteApp->actionManager()->removeToolBar(m_toolBar);
    }
    if (m_curEnv) {
        m_liteApp->settings()->setValue(LITEENV_CURRENTENV,m_curEnv->id());
    }
}

void EnvManager::addEnv(LiteApi::IEnv *env)
{
    m_envList.append(env);
    connect(env,SIGNAL(goenvError(QString,QString)),this,SLOT(goenvError(QString,QString)));
}

void EnvManager::removeEnv(LiteApi::IEnv *env)
{
    if (env) {
        disconnect(env,0);
    }
    m_envList.removeAll(env);
}

LiteApi::IEnv *EnvManager::findEnv(const QString &id) const
{
    foreach (LiteApi::IEnv *env, m_envList) {
        if (env->id() == id) {
            return env;
        }
    }
    if (!m_envList.empty()) {
        return m_envList.first();
    }
    return NULL;
}

QList<LiteApi::IEnv*> EnvManager::envList() const
{
    return m_envList;
}

void EnvManager::setCurrentEnv(LiteApi::IEnv *env)
{
    if (m_curEnv == env) {
        return;
    }
    m_curEnv = env;
    if (m_curEnv) {
        m_curEnv->reload();
        m_liteApp->settings()->setValue(LITEENV_CURRENTENV,m_curEnv->id());
        m_liteApp->appendLog("LiteEnv",QString("load environment %1").arg(m_curEnv->id()),false);
    }
    emit currentEnvChanged(m_curEnv);
}

LiteApi::IEnv *EnvManager::currentEnv() const
{
    return m_curEnv;
}

QProcessEnvironment EnvManager::currentEnvironment() const
{
    if (m_curEnv) {
        return m_curEnv->environment();
    }
    return QProcessEnvironment::systemEnvironment();
}

void EnvManager::loadEnvFiles(const QString &path)
{
    QDir dir = path;
    m_liteApp->appendLog("LiteEnv","Loaded environment files from "+path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.env"));
    foreach (QString fileName, dir.entryList()) {
        Env::loadEnv(this,QFileInfo(dir,fileName).absoluteFilePath());
    }
}

static QString defaultEnvid()
{
#ifdef Q_OS_WIN
    if (QSysInfo::WordSize == 32) {
        return "win32";
    } else {
        return "win64";
    }
#endif
#ifdef Q_OS_LINUX
    if (QSysInfo::WordSize == 32) {
        return "linux32";
    } else {
        return "linux64";
    }
#endif
#ifdef Q_OS_DARWIN
    if (QSysInfo::WordSize == 32) {
        return "darwin32";
    } else {
        return "darwin64";
    }
#endif
#ifdef Q_OS_FREEBSD
    if (QSysInfo::WordSize == 32) {
        return "freebsd32";
    } else {
        return "freebsd64";
    }
#endif
}

bool EnvManager::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IEnvManager::initWithApp(app)) {
        return false;
    }
    loadEnvFiles(m_liteApp->resourcePath()+"/liteenv");

    m_toolBar = m_liteApp->actionManager()->insertToolBar("toolbar/env",tr("Environment Toolbar"),"toolbar/tabs");
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuToolBarPos,m_toolBar->toggleViewAction());

    m_envCmb = new QComboBox;
    m_envCmb->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_envCmb->setMinimumContentsLength(6);
    m_envCmb->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
    m_envCmb->setToolTip(tr("Switching current environment"));

    m_toolBar->addWidget(m_envCmb);
    QAction *editAct = new QAction(QIcon("icon:liteenv/images/setenv.png"),tr("Edit current environment"),this);
    QAction *reloadAct = new QAction(QIcon("icon:liteenv/images/reload.png"),tr("Reload current environment"),this);
    m_toolBar->addAction(reloadAct);
    m_toolBar->addAction(editAct);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuLastPos,reloadAct);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuLastPos,editAct);

    foreach (LiteApi::IEnv *env, m_envList) {
        m_envCmb->addItem(env->id());
    }

    m_liteApp->extension()->addObject("LiteApi.IEnvManager",this);

    QString id = m_liteApp->settings()->value(LITEENV_CURRENTENV,defaultEnvid()).toString();
    if (!id.isEmpty()) {
        this->setCurrentEnvId(id);
    }

    connect(m_envCmb,SIGNAL(activated(QString)),this,SLOT(envActivated(QString)));
    connect(editAct,SIGNAL(triggered()),this,SLOT(editCurrentEnv()));
    connect(reloadAct,SIGNAL(triggered()),this,SLOT(reloadCurrentEnv()));
    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(m_liteApp,SIGNAL(broadcast(QString,QString,QString)),this,SLOT(broadcast(QString,QString,QString)));
    return true;
}

void EnvManager::setCurrentEnvId(const QString &id)
{
    LiteApi::IEnv *env = findEnv(id);
    if (!env) {
        return;
    }
    for (int i = 0; i < m_envCmb->count(); i++) {
        if (m_envCmb->itemText(i) == id) {
            m_envCmb->setCurrentIndex(i);
            break;
        }
    }
    setCurrentEnv(env);
}


void EnvManager::envActivated(QString id)
{
    LiteApi::IEnv *env = findEnv(id);
    setCurrentEnv(env);
}

void EnvManager::editCurrentEnv()
{
    if (!m_curEnv) {
        return;
    }
    m_liteApp->fileManager()->openEditor(m_curEnv->filePath(),true);
}

void EnvManager::reloadCurrentEnv()
{
    if (!m_curEnv) {
        return;
    }
    m_curEnv->reload();
    m_liteApp->appendLog("LiteEnv",QString("reload environment %1").arg(m_curEnv->id()),false);
    currentEnvChanged(m_curEnv);
}

void EnvManager::broadcast(QString module,QString id,QString)
{
    if (module == "golangpackage" && id == "reloadgopath") {
        reloadCurrentEnv();
    }
}

void EnvManager::editorSaved(LiteApi::IEditor *editor)
{
    LiteApi::ITextEditor *ed = LiteApi::getTextEditor(editor);
    if (!ed) {
        return;
    }
    if (m_curEnv && m_curEnv->filePath() == ed->filePath()) {
        m_curEnv->reload();
        currentEnvChanged(m_curEnv);
    }
}

void EnvManager::goenvError(const QString &id, const QString &msg)
{
    m_liteApp->appendLog(QString("%1: go env error").arg(id),msg,true);
}
