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
// Module: goenvmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOENVMANAGER_H
#define GOENVMANAGER_H

#include "liteenvapi/liteenvapi.h"

class GoEnvManager : public LiteApi::IGoEnvManger
{
    Q_OBJECT
public:
    GoEnvManager(QObject *parent);
    virtual bool initWithApp(LiteApi::IApplication *app);
    virtual QString gocmd() const;
    virtual QString gotools() const;
    virtual QString GOROOT() const;
    virtual QStringList GOPATH() const;
    virtual QProcessEnvironment environment() const;
    virtual QProcessEnvironment customEnvironment(const QString &buildFilePath, QString *pCustomBuildPath) const;
    virtual QStringList customGOPATH(const QString &buildPath, QString *pCustomBuildPath) const;
    virtual QString findRealCustomBuildPath(const QString &buildPath) const;
    virtual bool hasCustomGOPATH(const QString &buildPath) const;
    virtual void updateGoEnv();
    virtual void updateCustomGOPATH(const QString &buildPath);
protected:
    QString m_gocmd;
    QString m_gotools;
    QString m_goroot;
    QStringList m_gopathList;
    LiteApi::IEnvManager *m_envManager;
};

#endif // GOENVMANAGER_H
