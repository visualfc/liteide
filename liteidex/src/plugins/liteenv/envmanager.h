/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: envmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef ENVMANAGER_H
#define ENVMANAGER_H

#include "liteenvapi/liteenvapi.h"

class QComboBox;

class EnvManager;
class Env : public LiteApi::IEnv
{
    Q_OBJECT
public:
    Env(QObject *parent = 0);
public:
    virtual QString id() const;
    virtual QString filePath() const;
    virtual QProcessEnvironment& environment();
    virtual QStringList orgEnvLines() const;
    virtual QMap<QString,QString> goEnvMap() const;
    virtual void reload();
    void loadGoEnv();
public:
    void loadEnvFile(QIODevice *dev);
    static void loadEnv(EnvManager *manager, const QString &filePath);
protected slots:
    void readStdout();
    void readStderr();
protected:
    QString m_filePath;
    QStringList m_orgEnvLines;
    QString m_id;
    QProcessEnvironment m_env;
    QMap<QString,QString> m_goEnvMap;
    QProcess *m_process;
};

class EnvManager : public LiteApi::IEnvManager
{
    Q_OBJECT
public:
    EnvManager(QObject *parent = 0);   
    ~EnvManager();
public:
    virtual bool initWithApp(LiteApi::IApplication *app);
    virtual QList<LiteApi::IEnv*> envList() const;
    virtual LiteApi::IEnv *findEnv(const QString &id) const;
    virtual void setCurrentEnvId(const QString &id);
    virtual LiteApi::IEnv *currentEnv() const;
    virtual QProcessEnvironment currentEnvironment() const;
protected slots:
    void envActivated(QString);
    void editCurrentEnv();
    void reloadCurrentEnv();
    void editorSaved(LiteApi::IEditor*);
    void goenvError(const QString &id, const QString &msg);
    void goenvChanged(const QString &id);
public:
    void setCurrentEnv(LiteApi::IEnv *env);
    void addEnv(LiteApi::IEnv *build);
    void removeEnv(LiteApi::IEnv *build);
    void loadEnvFiles(const QString &path);
public slots:
    void broadcast(QString module, QString id, QString);
protected:
    QList<LiteApi::IEnv*>    m_envList;
    LiteApi::IEnv           *m_curEnv;
    QToolBar        *m_toolBar;
    QComboBox       *m_envCmb;
};

#endif // ENVMANAGER_H
