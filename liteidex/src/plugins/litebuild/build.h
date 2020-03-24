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
// Module: build.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef BUILD_H
#define BUILD_H

#include "litebuildapi/litebuildapi.h"
#include <QProcessEnvironment>

using namespace LiteApi;
class Build : public LiteApi::IBuild
{
    Q_OBJECT
public:
    Build(QObject *parent = 0);
    virtual ~Build();
    virtual QString mimeType() const;
    virtual QString id() const;
    virtual QString work() const;
    virtual QString lock() const;
    virtual QList<BuildAction*> actionList() const;
    virtual QList<BuildLookup*> lookupList() const;
    virtual QList<BuildConfig*> configList() const;
    virtual QList<BuildCustom*> customList() const;
    virtual QList<BuildTarget*>  targetList() const;
    virtual BuildAction *findAction(const QString &id);
    virtual QList<QAction*> actions();
protected:
    void make();
    QAction *makeAction(BuildAction *ba);
protected slots:
    void slotBuildAction();
public:
    void setType(const QString &mimeType);
    void setId(const QString &id);
    void setWork(const QString &work);
    void setLock(const QString &lock);
    void appendAction(BuildAction *act);
    void appendLookup(BuildLookup *lookup);
    void appendConfig(BuildConfig *config);
    void appendCustom(BuildCustom *custom);
    void appendDebug(BuildTarget *debug);
public:
    static bool loadBuild(LiteApi::IBuildManager *manager, const QString &fileName);
    static bool loadBuild(LiteApi::IBuildManager *manager, QIODevice *dev, const QString &fileName);
protected:
    QString m_mimeType;
    QString m_id;
    QString m_lock;
    QString m_work;
    QMap<QString,QMenu*> m_idMenuMap;
    QList<QAction*> m_actions;
    QList<BuildAction*> m_actionList;
    QList<BuildLookup*> m_lookupList;
    QList<BuildConfig*> m_configList;
    QList<BuildCustom*> m_customList;
    QList<BuildTarget*> m_targetList;
    QMap<BuildAction*,QString> m_actionCmdMap;
    QMap<BuildAction*,QString> m_actionArgMap;
};

#endif // BUILD_H
