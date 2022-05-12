/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
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
// Module: goenvmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "goenvmanager.h"
#include "fileutil/fileutil.h"
#include <QDir>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

using namespace LiteApi;

GoEnvManager::GoEnvManager(QObject *parent)
    : IGoEnvManger(parent)
{

}

bool GoEnvManager::initWithApp(IApplication *app)
{
    if (!IGoEnvManger::initWithApp(app)) {
        return false;
    }
    m_liteApp->extension()->addObject("LiteApi.IGoEnvManger",this);
    m_envManager = LiteApi::getEnvManager(app);
    return true;
}

QString GoEnvManager::gocmd() const
{
    return m_gocmd;
}

QString GoEnvManager::gotools() const
{
    return m_gotools;
}

QString GoEnvManager::GOROOT() const
{
    return m_goroot;
}

QStringList GoEnvManager::GOPATH() const
{
    return m_gopathList;
}

QProcessEnvironment GoEnvManager::environment() const
{
    return LiteApi::getGoEnvironment(m_liteApp);
}

QProcessEnvironment GoEnvManager::customEnvironment(const QString &buildFilePath, QString *pCustomBuildPath) const
{
    return LiteApi::getCustomGoEnvironment(m_liteApp,buildFilePath,pCustomBuildPath);
}

QStringList GoEnvManager::customGOPATH(const QString &buildPath, QString *pCustomBuildPath) const
{
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    QProcessEnvironment env = this->customEnvironment(buildPath,pCustomBuildPath);
    return env.value("GOPATH").split(sep);
}

QString GoEnvManager::findRealCustomBuildPath(const QString &buildPath) const
{
    if (buildPath.isEmpty()) {
        return QString();
    }
    QString customKey = "litebuild-custom/"+buildPath;
    QString customBuildPath = buildPath;
    bool use_custom_gopath = m_liteApp->settings()->value(customKey+"#use_custom_gopath",false).toBool();
    if (!use_custom_gopath) {
        QString srcRoot = LiteApi::lookupSrcRoot(buildPath);
        if (!srcRoot.isEmpty()) {
            customKey = LiteApi::lookupParentHasCustom(m_liteApp,buildPath,srcRoot, &customBuildPath);
            if (!customKey.isEmpty()) {
                use_custom_gopath = true;
                return customBuildPath;
            }
        }
    }
    return QString();
}

bool GoEnvManager::hasCustomGOPATH(const QString &buildPath) const
{
    return !findRealCustomBuildPath(buildPath).isEmpty();
}

void GoEnvManager::updateGoEnv()
{
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    m_gotools = LiteApi::getGotools(m_liteApp);
    m_gocmd = FileUtil::lookupGoBin("go",m_liteApp,env,false);
    m_goroot = env.value("GOROOT");
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    QStringList pathList;
    QString goroot = QDir::toNativeSeparators(env.value("GOROOT"));
    foreach (QString path, env.value("GOPATH").split(sep,qtSkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeAll(goroot);
    pathList.removeDuplicates();

    if (m_gopathList != pathList) {
        m_gopathList = pathList;
        emit globalGOPATHChanged();
    }
}

void GoEnvManager::updateCustomGOPATH(const QString &buildPath)
{
    emit customGOPATHChanged(buildPath);
}
