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
// Module: golangcodeplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGCODEPLUGIN_H
#define GOLANGCODEPLUGIN_H

#include "golangcode_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class GolangCode;
class GolangCodePlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    GolangCodePlugin();
    virtual bool load(LiteApi::IApplication *app);
    virtual QStringList dependPluginList() const;
protected slots:
    void appLoaded();
    void editorCreated(LiteApi::IEditor*);
    void currentEditorChanged(LiteApi::IEditor*);
protected:
    LiteApi::IApplication *m_liteApp;
    GolangCode *m_code;
};

class PluginFactory : public LiteApi::PluginFactoryT<GolangCodePlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.GolangCodePlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/golangcode");
        m_info->appendDepend("plugin/golangast");
        m_info->setName("GolangCode");
        m_info->setAuthor("visualfc");
        m_info->setVer("X27.2");
        m_info->setInfo("Golang Gocode Support");
    }
};

#endif // GOLANGCODEPLUGIN_H
