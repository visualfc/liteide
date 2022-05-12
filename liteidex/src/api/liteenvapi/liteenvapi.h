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
// Module: liteenvapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEENVAPI_H
#define LITEENVAPI_H

#include "liteapi/liteapi.h"
#include <QProcessEnvironment>
#include <QDir>
#include <QDebug>

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
    virtual QMap<QString,QString> goEnvMap() const = 0;
    virtual QProcessEnvironment& environment() = 0;
    virtual void reload() = 0;
signals:
    void goenvError(QString,QString);
    void goenvChanged(QString);
};

class IEnvManager : public IManager
{
    Q_OBJECT
public:
    IEnvManager(QObject *parent = 0) : IManager(parent) {}
    virtual QList<IEnv*> envList() const = 0;
    virtual IEnv *findEnv(const QString &id, const QString &backup = "system") const = 0;
    virtual void setCurrentEnvId(const QString &id) = 0;
    virtual IEnv *currentEnv() const = 0;
    virtual QProcessEnvironment currentEnvironment() const = 0;
    virtual void reloadCurrentEnv() = 0;
signals:
    void currentEnvChanged(LiteApi::IEnv*);
};

class IGoEnvManger: public IManager
{
    Q_OBJECT
public:
    IGoEnvManger(QObject *parent = 0) : IManager(parent) {}
    virtual QString gocmd() const = 0;
    virtual QString gotools() const = 0;
    virtual QString GOROOT() const = 0;
    virtual QStringList GOPATH() const = 0;
    virtual QProcessEnvironment environment() const = 0;
    virtual QProcessEnvironment customEnvironment(const QString &buildFilePath, QString *pCustomBuildPath = 0) const = 0;
    virtual QStringList customGOPATH(const QString &buildPath, QString *pCustomBuildPath = 0) const = 0;
    virtual QString findRealCustomBuildPath(const QString &buildPath) const = 0;
    virtual bool hasCustomGOPATH(const QString &buildPath) const = 0;
    virtual void updateGoEnv() = 0;
    virtual void updateCustomGOPATH(const QString &buildPath) = 0;
signals:
    void globalGOPATHChanged();
    void customGOPATHChanged(const QString &buildPath);
};

inline IEnvManager *getEnvManager(LiteApi::IApplication* app)
{
    return LiteApi::findExtensionObject<IEnvManager*>(app,"LiteApi.IEnvManager");
}

inline IGoEnvManger *getGoEnvManager(LiteApi::IApplication *app)
{
    return LiteApi::findExtensionObject<IGoEnvManger*>(app,"LiteApi.IGoEnvManger");
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
    foreach (QString path, e.value("PATH").split(sep,qtSkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    pathList.append(app->applicationPath());
    pathList.removeDuplicates();
    e.insert("PATH",pathList.join(sep));
    return e;
}

inline QString getDefaultGOOS()
{
    const char* goos = "";
#ifdef Q_OS_WIN
    goos = "windows";
#endif
#ifdef Q_OS_LINUX
    goos = "linux";
#endif
#ifdef Q_OS_DARWIN
    goos = "darwin";
#endif
#ifdef Q_OS_FREEBSD
    goos = "freebsd";
#endif
#ifdef Q_OS_OPENBSD
    goos = "openbsd";
#endif
	return goos;
}

inline QString getDefaultGOROOT()
{
#ifdef Q_OS_WIN
    return "c:\\go";
#else
    return "/usr/local/go";
#endif
}

inline bool hasGoEnv(const QProcessEnvironment &env)
{
    return env.contains("GOROOT") && env.contains("GOARCH");
}

inline QProcessEnvironment getSysEnvironment(LiteApi::IApplication *app)
{
    QProcessEnvironment env = getCurrentEnvironment(app);
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif

    IEnvManager *mgr = LiteApi::getEnvManager(app);
    if (mgr) {
        LiteApi::IEnv *ce = mgr->currentEnv();
        if (ce) {
            QMapIterator<QString,QString> i(ce->goEnvMap());
            while(i.hasNext()) {
                i.next();
                env.insert(i.key(),i.value());
            }
        }
    }

    QString goos = env.value("GOOS");
    if (goos.isEmpty()) {
        goos = getDefaultGOOS();
    }

    QString goroot = env.value("GOROOT");
    if (goroot.isEmpty()) {
        goroot = getDefaultGOROOT();
    }
    return env;
}


inline QProcessEnvironment getGoEnvironment(LiteApi::IApplication *app)
{
    QProcessEnvironment env = getCurrentEnvironment(app);
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif

    IEnvManager *mgr = LiteApi::getEnvManager(app);
    if (mgr) {
        LiteApi::IEnv *ce = mgr->currentEnv();
        if (ce) {
            QMapIterator<QString,QString> i(ce->goEnvMap());
            while(i.hasNext()) {
                i.next();
                env.insert(i.key(),i.value());
            }
        }
    }

    QString goos = env.value("GOOS");
    if (goos.isEmpty()) {
        goos = getDefaultGOOS();
    }
    if (!env.contains("GOEXE")) {
        QString goexe;
        if (goos == "windows") {
            goexe = ".exe";
        }
        env.insert("GOEXE",goexe);
    }

    QString goarch = env.value("GOARCH");
    QString goroot = env.value("GOROOT");
    if (goroot.isEmpty()) {
        goroot = getDefaultGOROOT();
    }

    if (app->settings()->value("liteide/use111gomodule",false).toBool()) {
        env.insert("GO111MODULE",app->settings()->value("liteide/go111module").toString());
    }
    if (app->settings()->value("liteide/usegoproxy",false).toBool()) {
        env.insert("GOPROXY",app->settings()->value("liteide/goproxy").toString());
    }
    if (app->settings()->value("liteide/usegoprivate",false).toBool()) {
        env.insert("GOPRIVATE",app->settings()->value("liteide/goprivate").toString());
    }
    if (app->settings()->value("liteide/usegonoproxy",false).toBool()) {
        env.insert("GONOPROXY",app->settings()->value("liteide/gonoproxy").toString());
    }
    if (app->settings()->value("liteide/usegonosumdb",false).toBool()) {
        env.insert("GONOSUMDB",app->settings()->value("liteide/gonosumdb").toString());
    }

    QStringList pathList;
    if (app->settings()->value("liteide/usesysgopath",true).toBool()) {
        foreach (QString path, env.value("GOPATH").split(sep,qtSkipEmptyParts)) {
            pathList.append(QDir::toNativeSeparators(path));
        }
    }
    if (app->settings()->value("liteide/uselitegopath",true).toBool()) {
        foreach (QString path, app->settings()->value("liteide/gopath").toStringList()) {
            pathList.append(QDir::toNativeSeparators(path));
        }
    }
    pathList.removeDuplicates();
    env.insert("GOPATH",pathList.join(sep));

    if (!goroot.isEmpty()) {
        pathList.prepend(goroot);
    }

    QStringList binList;    
    QString gobin = env.value("GOBIN");
    if (!gobin.isEmpty()) {
        binList.append(gobin);
    }
    foreach (QString path, pathList) {
        binList.append(QFileInfo(path,"bin").filePath());
        binList.append(QFileInfo(path,"bin/"+goos+"_"+goarch).filePath());
    }
    env.insert("PATH",env.value("PATH")+sep+binList.join(sep)+sep);

    return env;
}

inline QStringList getGOPATH(LiteApi::IApplication *app, bool includeGoroot)
{
    QProcessEnvironment env = getGoEnvironment(app);
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
    foreach (QString path, env.value("GOPATH").split(sep,qtSkipEmptyParts)) {
        pathList.append(QDir::toNativeSeparators(path));
    }
    if (!includeGoroot) {
        pathList.removeAll(goroot);
    }
    pathList.removeDuplicates();
    return pathList;
}

inline QString getGOROOT(LiteApi::IApplication *app)
{
    return getGoEnvironment(app).value("GOROOT");
}

inline QString lookupSrcRoot(const QString &buildFilePath)
{
    int index = buildFilePath.indexOf("/src/");
    if (index < 0) {
        return QString();
    }
    return buildFilePath.left(index+4);
}

inline QString lookupParentHasCustom(LiteApi::IApplication *app, const QString &buildFilePath, const QString &srcRoot, QString *pCustomParent = 0)
{
    QFileInfo info(buildFilePath);
    QString parent = info.path();

    if (parent == srcRoot || info.dir().isRoot()) {
        return QString();
    }
    QString customKey = "litebuild-custom/"+parent;
    bool use_custom_gopath = app->settings()->value(customKey+"#use_custom_gopath",false).toBool();
    if (use_custom_gopath) {
        if (pCustomParent) {
            *pCustomParent = parent;
        }
        return customKey;
    }
    return lookupParentHasCustom(app,parent,srcRoot);
}

inline QProcessEnvironment getCustomGoEnvironment(LiteApi::IApplication *app, const QString &buildFilePath, QString *pCustomBuildPath = 0)
{
    if (buildFilePath.isEmpty()) {
        return getGoEnvironment(app);
    }
    QString customKey = "litebuild-custom/"+buildFilePath;
    QString customBuildPath = buildFilePath;
    bool use_custom_gopath = app->settings()->value(customKey+"#use_custom_gopath",false).toBool();
    if (!use_custom_gopath) {
        QString srcRoot = lookupSrcRoot(buildFilePath);
        if (!srcRoot.isEmpty()) {
            customKey = lookupParentHasCustom(app,buildFilePath,srcRoot, &customBuildPath);
            if (!customKey.isEmpty()) {
                use_custom_gopath = true;
            }
        }
    }
    if (!use_custom_gopath) {
        return getGoEnvironment(app);
    }
    if (pCustomBuildPath) {
        *pCustomBuildPath = customBuildPath;
    }

    QProcessEnvironment env = getCurrentEnvironment(app);
#ifdef Q_OS_WIN
    QString sep = ";";
#else
    QString sep = ":";
#endif

    IEnvManager *mgr = LiteApi::getEnvManager(app);
    if (mgr) {
        LiteApi::IEnv *ce = mgr->currentEnv();
        if (ce) {
            QMapIterator<QString,QString> i(ce->goEnvMap());
            while(i.hasNext()) {
                i.next();
                env.insert(i.key(),i.value());
            }
        }
    }

    QString goos = env.value("GOOS");
    if (goos.isEmpty()) {
        goos = getDefaultGOOS();
    }
    if (!env.contains("GOEXE")) {
        QString goexe;
        if (goos == "windows") {
            goexe = ".exe";
        }
        env.insert("GOEXE",goexe);
    }

    QString goarch = env.value("GOARCH");
    QString goroot = env.value("GOROOT");
    if (goroot.isEmpty()) {
        goroot = getDefaultGOROOT();
    }

    QStringList pathList;

    bool inherit_sys_gopath = app->settings()->value(customKey+"#inherit_sys_gopath",true).toBool();
    bool inherit_lite_gopath = app->settings()->value(customKey+"#inherit_lite_gopath",true).toBool();
    bool custom_gopath = app->settings()->value(customKey+"#custom_gopath",false).toBool();

    if (inherit_sys_gopath) {
        foreach (QString path, env.value("GOPATH").split(sep,qtSkipEmptyParts)) {
            pathList.append(QDir::toNativeSeparators(path));
        }
    }
    if (inherit_lite_gopath) {
        foreach (QString path, app->settings()->value("liteide/gopath").toStringList()) {
            pathList.append(QDir::toNativeSeparators(path));
        }
    }
    if (custom_gopath) {
        foreach (QString path, app->settings()->value(customKey+"#gopath").toStringList()) {
            pathList.append(QDir::toNativeSeparators(path));
        }
    }
    pathList.removeDuplicates();
    env.insert("GOPATH",pathList.join(sep));

    if (!goroot.isEmpty()) {
        pathList.prepend(goroot);
    }

    QStringList binList;
    QString gobin = env.value("GOBIN");
    if (!gobin.isEmpty()) {
        binList.append(gobin);
    }
    foreach (QString path, pathList) {
        binList.append(QFileInfo(path,"bin").filePath());
        binList.append(QFileInfo(path,"bin/"+goos+"_"+goarch).filePath());
    }
    env.insert("PATH",env.value("PATH")+sep+binList.join(sep)+sep);
    return env;
}

inline QProcessEnvironment getCustomGoEnvironment(LiteApi::IApplication *app, LiteApi::IEditor *editor)
{
    QString buildFilePath;
    if (editor) {
        QString filePath = editor->filePath();
        if (!filePath.isEmpty()) {
            buildFilePath = QFileInfo(filePath).path();
        }
    }
    return getCustomGoEnvironment(app,buildFilePath);
}

} //namespace LiteApi


#endif //LITEENVAPI_H

