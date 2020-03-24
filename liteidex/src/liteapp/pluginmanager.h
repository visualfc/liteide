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
// Module: pluginmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class PluginManager : public QObject
{
    Q_OBJECT
public:
    PluginManager(QObject *parent = 0);
    virtual ~PluginManager();
    QList<IPluginFactory*> factoryList();
    void loadPlugins(const QString &dir);
    bool isLoaded() const;
protected:
    bool            m_bLoaded;
    QAction         *m_aboutPluginsAct;
    QList<IPluginFactory*> m_factroyList;
};

#endif // PLUGINMANAGER_H
