/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: buildmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include "litebuildapi/litebuildapi.h"

using namespace LiteApi;
class BuildManager : public LiteApi::IBuildManager
{
    Q_OBJECT
public:
    BuildManager(QObject *parent);
    ~BuildManager();
    virtual void addBuild(IBuild *build);
    virtual void removeBuild(IBuild *build);
    virtual IBuild *findBuild(const QString &mimeType);
    virtual QList<IBuild*> buildList() const;
    virtual void setCurrentBuild(LiteApi::IBuild *build);
    virtual IBuild *currentBuild() const;
public:
    void load(const QString &path);
protected:
    QList<IBuild*>  m_buildList;
    LiteApi::IBuild *m_build;
};

#endif // BUILDMANAGER_H
