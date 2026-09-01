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
// Module: envmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "envmanager.h"
#include "goenvmanager.h"
#include "liteenv_global.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "liteapi/liteids.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QToolBar>
#include <QMenu>
#include <QComboBox>
#include <QLabel>
#include <QAction>
#include <QActionGroup>
#include <QSysInfo>
#include <QProcess>
#include <QStringList>
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

static QString updateValue(const QString &value,const QProcessEnvironment &env)
{
    QString v = value;
    QRegExp rx("\\$\\((\\w+)\\)");
    int pos = 0;
    QStringList list;
    while ((pos = rx.indexIn(v, pos)) != -1) {
         list << rx.cap(1);
         pos += rx.matchedLength();
    }
    foreach (QString str, list) {
         if (env.contains(str)) {
            v.replace("$("+str+")",env.value(str));
        }
    }
    return v;
}

Env::Env(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IEnv(parent), m_liteApp(app)
{
    m_ideEnvMap.insert("LITEIDE_ROOT_PATH",app->rootPath());
    m_ideEnvMap.insert("LITEIDE_APP_PATH",app->applicationPath());
    m_ideEnvMap.insert("LITEIDE_TOOL_PATH",app->toolPath());
    m_ideEnvMap.insert("LITEIDE_RES_PATH",app->resourcePath());
    m_ideEnvMap.insert("LITEIDE_PLUGIN_PATH",app->pluginPath());

    m_env = QProcessEnvironment::systemEnvironment();
    updateIdeEnv(m_env);

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
    if (!m_filePath.isEmpty()) {
        QFile f(m_filePath);
        if (f.open(QIODevice::ReadOnly)) {
            loadEnvFile(&f);
            f.close();
            loadGoEnv();
        }
    } else {
        emit goenvChanged(m_id);
    }
}

void Env::loadGoEnv()
{
    if (!m_process) {
        m_process = new Process(this);
        connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdout()));
        connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readStderr()));
        connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
        connect(m_process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
    }
    m_process->stopAndWait(100,200);
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
    updateIdeEnv(env);

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
        if (value.contains("$")) {
            value = updateValue(value,env);
        }
        env.insert(key,value);
    }
    m_env = env;
}

Env *Env::loadEnv(EnvManager *manager, const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return 0;
    }

    Env *env = new Env(manager->application(),manager);
    env->m_filePath = filePath;
    env->m_id = QFileInfo(filePath).completeBaseName();
    env->loadEnvFile(&f);
    f.close();
    manager->addEnv(env);
    return env;
}

static QStringList envFilter = QString("GOROOT;GOPATH;GOEXE;GOOS;GOARCH;GOBIN;GOVERSION;GO111MODULE;GOTOOLCHAIN").split(";");

void Env::readStdout()
{
    QByteArray data = m_process->readAllStandardOutput();
// set GOARCH=amd64
// GOARCH="amd64"
    m_liteApp->appendLog("LiteEnv","go env\n"+QString::fromUtf8(data).trimmed(),false);
    foreach (QByteArray line, data.split('\n')) {
        QString info = QString::fromUtf8(line).trimmed();
        if (info.startsWith("set ")) {
            info = info.mid(4);
        }
        int index = info.indexOf("=");
        if (index > 0) {
            QString key = info.left(index);
            if (!envFilter.contains(key)) {
                continue;
            }
            QString value = info.right(info.length()-index-1);
            if (value.startsWith("\"") && value.endsWith("\"")) {
                value = value.mid(1,value.length()-2);
            } else if (value.startsWith("\'") && value.endsWith("\'")) {
                value = value.mid(1,value.length()-2);
            }
            m_goEnvMap[key] = value;
        }
    }
//    emit goenvChanged(m_id);
}

void Env::readStderr()
{
    QByteArray data = m_process->readAllStandardError();
    m_liteApp->appendLog("LiteEnv","go env\n"+QString::fromUtf8(data).trimmed(),true);
//    emit goenvError(m_id,QString::fromUtf8(data));
}

void Env::finished(int code, QProcess::ExitStatus /*status*/)
{
    if (code == 0) {
        emit goenvChanged(m_id);
    } else {
        emit goenvError(m_id,QString("go env exit code %1").arg(code));
    }
}

void Env::error(QProcess::ProcessError error)
{
    emit goenvError(m_id, ProcessEx::processErrorText(error));
}

void Env::updateIdeEnv(QProcessEnvironment &env)
{
    QMapIterator<QString,QString> i(m_ideEnvMap);
    while(i.hasNext()) {
        i.next();
        env.insert(i.key(),i.value());
    }
}

EnvManager::EnvManager(QObject *parent)
    : LiteApi::IEnvManager(parent),
      m_curEnv(0),m_toolBar(0), m_envCmb(0),
      m_appLoaded(false)
{
    m_goEnvManager = new GoEnvManager(this);
}

EnvManager::~EnvManager()
{
    if (m_toolBar) {
        m_liteApp->actionManager()->removeToolBar(m_toolBar);
    }
    if (m_curEnv) {
        m_liteApp->settings()->setValue(LITEENV_CURRENTENV,m_curEnv->id());
    }
    delete m_selectMenu;
}

void EnvManager::addEnv(LiteApi::IEnv *env)
{
    m_envList.append(env);
    connect(env,SIGNAL(goenvError(QString,QString)),this,SLOT(goenvError(QString,QString)));
    connect(env,SIGNAL(goenvChanged(QString)),this,SLOT(goenvChanged(QString)));
}

void EnvManager::removeEnv(LiteApi::IEnv *env)
{
    if (env) {
        disconnect(env,0);
    }
    m_envList.removeAll(env);
}

LiteApi::IEnv *EnvManager::findEnv(const QString &id, const QString &backup) const
{
    foreach (LiteApi::IEnv *env, m_envList) {
        if (env->id() == id) {
            return env;
        }
    }
    if (!backup.isEmpty()) {
        foreach (LiteApi::IEnv *env, m_envList) {
            if (env->id() == backup) {
                return env;
            }
        }
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

void EnvManager::appLoaded()
{
    m_appLoaded = true;
    m_liteApp->appendLog("EnvManager","init load environment");
    //emitEnvChanged();
    QString id = m_liteApp->settings()->value(LITEENV_CURRENTENV,"system").toString();

    //commandline
    //liteide --select-env system
    QString flagSelectEnv = "--select-env";
    QString selectEnv = m_liteApp->globalCookie().value(flagSelectEnv).toString();

    if (!selectEnv.isEmpty()) {
        id = selectEnv;
        m_liteApp->globalCookie().remove(flagSelectEnv);
    }
    if (id.isEmpty()) {
        id = "system";
    }

    this->setCurrentEnvId(id);
}

void EnvManager::loadEnvFiles(const QString &path, bool userPath)
{
    QDir dir = path;
    m_liteApp->appendLog("LiteEnv","Loaded environment files from "+path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.env"));
    foreach (QString fileName, dir.entryList()) {
        Env *env = Env::loadEnv(this,QFileInfo(dir,fileName).absoluteFilePath());
        if (userPath && env) {
            for (int i = 0; i < m_envList.size() - 1; ++i) {
                if (m_envList.at(i)->id() == env->id()) {
                    LiteApi::IEnv *old = m_envList.takeAt(i);
                    delete old;
                    break;
                }
            }
        }
    }
}

void EnvManager::addUserEnvFile(const QString &filePath)
{
    Env *env = Env::loadEnv(this, filePath);
    if (!env) {
        return;
    }
    for (int i = 0; i < m_envList.size() - 1; ++i) {
        if (m_envList.at(i)->id() == env->id()) {
            LiteApi::IEnv *old = m_envList.takeAt(i);
            if (old == m_curEnv) {
                m_curEnv = env;
            }
            delete old;
            break;
        }
    }
    if (!m_envCmb) {
        return;
    }
    for (int i = 0; i < m_envCmb->count(); ++i) {
        if (m_envCmb->itemText(i) == env->id()) {
            m_envCmb->removeItem(i);
            break;
        }
    }
    foreach (QAction *oldAct, m_selectActionGroup->actions()) {
        if (oldAct->text() == env->id()) {
            m_selectActionGroup->removeAction(oldAct);
            m_selectMenu->removeAction(oldAct);
            oldAct->deleteLater();
            break;
        }
    }
    m_envCmb->addItem(env->id());
    QAction *act = new QAction(env->id(), this);
    act->setCheckable(true);
    m_selectActionGroup->addAction(act);
    m_selectMenu->addAction(act);
}

void EnvManager::renameUserEnvFile(const QString &oldPath, const QString &newPath)
{
    LiteApi::IEnv *oldEnv = 0;
    bool wasCurrent = false;
    foreach (LiteApi::IEnv *env, m_envList) {
        if (env->filePath() == oldPath) {
            oldEnv = env;
            break;
        }
    }
    if (oldEnv) {
        const QString oldId = oldEnv->id();
        if (oldEnv == m_curEnv) {
            wasCurrent = true;
            m_curEnv = 0;
        }
        m_envList.removeAll(oldEnv);
        foreach (QAction *act, m_selectActionGroup ? m_selectActionGroup->actions() : QList<QAction*>()) {
            if (act->text() == oldId) {
                m_selectActionGroup->removeAction(act);
                if (m_selectMenu) m_selectMenu->removeAction(act);
                act->deleteLater();
                break;
            }
        }
        for (int i = 0; i < m_envCmb->count(); ++i) {
            if (m_envCmb->itemText(i) == oldId) {
                m_envCmb->removeItem(i);
                break;
            }
        }
        delete oldEnv;
    }
    addUserEnvFile(newPath);
    if (wasCurrent) {
        setCurrentEnvId(QFileInfo(newPath).completeBaseName());
    }
}

void EnvManager::removeUserEnvFile(const QString &filePath)
{
    LiteApi::IEnv *removed = 0;
    foreach (LiteApi::IEnv *env, m_envList) {
        if (env->filePath() == filePath) {
            removed = env;
            break;
        }
    }
    if (!removed) {
        return;
    }
    const QString id = removed->id();
    const bool wasCurrent = (removed == m_curEnv);
    if (wasCurrent) {
        m_curEnv = 0;
    }
    m_envList.removeAll(removed);
    delete removed;
    for (int i = 0; i < m_envCmb->count(); ++i) {
        if (m_envCmb->itemText(i) == id) {
            m_envCmb->removeItem(i);
            break;
        }
    }
    foreach (QAction *act, m_selectActionGroup->actions()) {
        if (act->text() == id) {
            m_selectActionGroup->removeAction(act);
            m_selectMenu->removeAction(act);
            act->deleteLater();
            break;
        }
    }
    const QString templatePath = m_liteApp->resourcePath() + "/liteenv/" + id + ".env";
    if (QFileInfo::exists(templatePath)) {
        addUserEnvFile(templatePath);
    }
    if (wasCurrent) {
        // A removed environment can still have queued go-env signals. Select
        // the replacement before those signals are delivered.
        setCurrentEnvId("system");
    }
}

QString EnvManager::userEnvPath(const QString &id) const
{
    return QDir(m_liteApp->storagePath() + "/liteenv").absoluteFilePath(id + ".env");
}

bool EnvManager::materializeCurrentEnv()
{
    if (!m_curEnv || m_curEnv->filePath().isEmpty()) {
        return false;
    }
    const QString templatePath = m_liteApp->resourcePath() + "/liteenv/" + m_curEnv->id() + ".env";
    if (QFileInfo(m_curEnv->filePath()).canonicalFilePath() != QFileInfo(templatePath).canonicalFilePath()) {
        return true;
    }
    const QString path = userEnvPath(m_curEnv->id());
    QDir dir = QFileInfo(path).dir();
    if (!dir.exists() && !QDir().mkpath(dir.absolutePath())) {
        return false;
    }
    if (!QFileInfo::exists(path) && !QFile::copy(templatePath, path)) {
        return false;
    }
    Env *userEnv = Env::loadEnv(this, path);
    if (!userEnv) {
        return false;
    }
    for (int i = 0; i < m_envList.size() - 1; ++i) {
        if (m_envList.at(i)->id() == userEnv->id()) {
            LiteApi::IEnv *old = m_envList.takeAt(i);
            if (old == m_curEnv) {
                m_curEnv = userEnv;
            }
            delete old;
            break;
        }
    }
    userEnv->reload();
    return true;
}

void EnvManager::emitEnvChanged()
{
    if (!m_appLoaded) {
        return;
    }    

    m_goEnvManager->updateGoEnv();
    emit currentEnvChanged(m_curEnv);
}

//static QString defaultEnvid()
//{
//#ifdef Q_OS_WIN
//    if (QSysInfo::WordSize == 32) {
//        return "win32";
//    } else {
//        return "win64";
//    }
//#endif
//#ifdef Q_OS_LINUX
//    if (QSysInfo::WordSize == 32) {
//        return "linux32";
//    } else {
//        return "linux64";
//    }
//#endif
//#ifdef Q_OS_DARWIN
//    if (QSysInfo::WordSize == 32) {
//        return "darwin32";
//    } else {
//        return "darwin64";
//    }
//#endif
//#ifdef Q_OS_FREEBSD
//    if (QSysInfo::WordSize == 32) {
//        return "freebsd32";
//    } else {
//        return "freebsd64";
//    }
//#endif
//#ifdef Q_OS_OPENBSD
//    if (QSysInfo::WordSize == 32) {
//        return "openbsd32";
//    } else {
//        return "openbsd64";
//    }
//#endif
//}

bool EnvManager::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IEnvManager::initWithApp(app)) {
        return false;
    }
    loadEnvFiles(m_liteApp->resourcePath()+"/liteenv");
    loadEnvFiles(m_liteApp->storagePath()+"/liteenv", true);

    m_toolBar = m_liteApp->actionManager()->insertToolBar(ID_TOOLBAR_ENV,tr("Environment Toolbar"));
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

    m_selectMenu = new QMenu(tr("Select Environment"));

    m_liteApp->actionManager()->insertMenuActions(ID_MENU_TOOLS,"sep/env",true,
                                                  QList<QAction*>() << reloadAct << editAct << m_selectMenu->menuAction());

    m_selectActionGroup = new QActionGroup(this);

    foreach (LiteApi::IEnv *env, m_envList) {
        m_envCmb->addItem(env->id());
        QAction *act = new QAction(env->id(),this);
        act->setCheckable(true);
        m_selectActionGroup->addAction(act);
    }
    m_selectMenu->addActions(m_selectActionGroup->actions());

    m_liteApp->extension()->addObject("LiteApi.IEnvManager",this);

    connect(m_envCmb,SIGNAL(textActivated(QString)),this,SLOT(envActivated(QString)));
    connect(m_selectActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(selectEnvAction(QAction*)));
    connect(editAct,SIGNAL(triggered()),this,SLOT(editCurrentEnv()));
    connect(reloadAct,SIGNAL(triggered()),this,SLOT(reloadCurrentEnv()));
    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(m_liteApp,SIGNAL(broadcast(QString,QString,QVariant)),this,SLOT(broadcast(QString,QString,QVariant)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));

    m_goEnvManager->initWithApp(app);

    return true;
}

void EnvManager::setCurrentEnvId(const QString &id)
{
    LiteApi::IEnv *env = findEnv(id);
    if (!env) {
        return;
    }
    for (int i = 0; i < m_envCmb->count(); i++) {
        if (m_envCmb->itemText(i) == env->id()) {
            m_envCmb->setCurrentIndex(i);
            QAction *act = m_selectActionGroup->actions().at(i);
            if (act) {
                act->setChecked(true);
            }
            break;
        }
    }
    setCurrentEnv(env);
}

void EnvManager::envActivated(QString id)
{
    setCurrentEnvId(id);
}

void EnvManager::editCurrentEnv()
{
    if (!m_curEnv) {
        return;
    }
    if (!materializeCurrentEnv()) {
        m_liteApp->appendLog("LiteEnv",tr("Cannot create user environment file for %1").arg(m_curEnv->id()),true);
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
    //emitEnvChanged();
}

void EnvManager::broadcast(QString /*module*/,QString /*id*/,QVariant)
{
//    if (module == "golangpackage" && id == "reloadgopath") {
//        reloadCurrentEnv();
//    }
}

void EnvManager::editorSaved(LiteApi::IEditor *editor)
{
    LiteApi::ITextEditor *ed = LiteApi::getTextEditor(editor);
    if (!ed) {
        return;
    }
    if (m_curEnv && m_curEnv->filePath() == ed->filePath()) {
        m_curEnv->reload();
    } else if (m_curEnv && userEnvPath(m_curEnv->id()) == ed->filePath()) {
        materializeCurrentEnv();
    }
}

void EnvManager::goenvError(const QString &id, const QString &msg)
{
    m_liteApp->appendLog(QString("%1: go env error").arg(id),msg,true);
    emitEnvChanged();
}

void EnvManager::goenvChanged(const QString &id)
{
    if (m_curEnv && id == m_curEnv->id()) {
        m_liteApp->appendLog("LiteEnv",QString("reset %1 environment for \"go env\"").arg(id),false);
        emitEnvChanged();
    }
}

void EnvManager::selectEnvAction(QAction *act)
{
    QString id = act->text();
    setCurrentEnvId(id);
}
