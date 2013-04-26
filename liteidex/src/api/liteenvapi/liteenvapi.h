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
// Module: liteenvapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef __LITEENVAPI_H__
#define __LITEENVAPI_H__

#include "liteapi/liteapi.h"
#include <QProcessEnvironment>
#include <QDir>

namespace LiteApi {

class IEnv : public QObject
{
    Q_OBJECT
public:
    IEnv(QObject *parent = 0): QObject(parent) {}
    virtual ~IEnv() {}
    virtual QString id() const = 0;
    virtual QString filePath() const = 0;
    virtual QStringList orgEnvLines() const = 0;
    virtual QProcessEnvironment& environment() = 0;
    virtual void reload() = 0;
};

class IEnvManager : public IManager
{
    Q_OBJECT
public:
    IEnvManager(QObject *parent = 0) : IManager(parent) {}
    virtual QList<IEnv*> envList() const = 0;
    virtual IEnv *findEnv(const QString &id) const = 0;
    virtual void setCurrentEnvId(const QString &id) = 0;
    virtual IEnv *currentEnv() const = 0;
    virtual QProcessEnvironment currentEnvironment() const = 0;
signals:
    void currentEnvChanged(LiteApi::IEnv*);
};

inline IEnvManager *getEnvManager(LiteApi::IApplication* app)
{
    return LiteApi::findExtensionObject<IEnvManager*>(app,"LiteApi.IEnvManager");
}

inline QProcessEnvironment getCurrentEnvironment(LiteApi::IApplication *app)
{
    QProcessEnvironment e;
    IEnvManager *env = getEnvManager(app);
    if (env) {
        e = env->currentEnvironment();
    } else {
        e = QProcessEnvironment::systemEnvironment();
    }
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    QStringList pathList;
    foreach (QString path, e.value("PATH").split(sep,QString::SkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.append(app->applicationPath());
    pathList.removeDuplicates();
    e.insert("PATH",pathList.join(sep));
    return e;
}

inline QProcessEnvironment getGoEnvironment(LiteApi::IApplication *app)
{
    QProcessEnvironment env = getCurrentEnvironment(app);
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif

    QStringList pathList;
    foreach (QString path, env.value("GOPATH").split(sep,QString::SkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    foreach (QString path, app->settings()->value("liteide/gopath").toStringList()) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.removeDuplicates();
    env.insert("GOPATH",pathList.join(sep));
    return env;
}

inline QStringList getGopathList(LiteApi::IApplication *app, bool includeGoroot)
{
    QProcessEnvironment env = getCurrentEnvironment(app);
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif
    QStringList pathList;
    QString goroot = QDir::toNativeSeparators(env.value("GOROOT"));
    if (includeGoroot) {
        pathList.append(goroot);
    }
    foreach (QString path, env.value("GOPATH").split(sep,QString::SkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    foreach (QString path, app->settings()->value("liteide/gopath").toStringList()) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    if (!includeGoroot) {
        pathList.removeAll(goroot);
    }
    pathList.removeDuplicates();
    return pathList;
}

inline QString getGoroot(LiteApi::IApplication *app)
{
    return getCurrentEnvironment(app).value("GOROOT");
}


} //namespace LiteApi


#endif //__LITEENVAPI_H__

