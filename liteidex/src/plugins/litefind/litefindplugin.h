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
// Module: litefindplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEFINDPLUGIN_H
#define LITEFINDPLUGIN_H

#include "litefind_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class FindEditor;
class FindEditor;
class FileSearch;
class LiteFindPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    LiteFindPlugin();
    virtual ~LiteFindPlugin();
    virtual bool load(LiteApi::IApplication *app);
public slots:
    void find();
    void replace();
    void hideFind();
protected:
    LiteApi::IApplication *m_liteApp;
    FindEditor *m_findEditor;
    QAction *m_findAct;
    QAction *m_findNextAct;
    QAction *m_findPrevAct;
    QAction *m_replaceAct;
    QAction *m_fileSearchAct;
};

class PluginFactory : public LiteApi::PluginFactoryT<LiteFindPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.LiteFindPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/LiteFind");
        m_info->setName("LiteFind");
        m_info->setAuthor("visualfc");
        m_info->setVer("X25");
        m_info->setInfo("Core Find/Replace");
        m_info->setMustLoad(true);
    }
};

#endif // LITEFINDPLUGIN_H
