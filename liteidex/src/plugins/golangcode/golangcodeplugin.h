/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
    void editorComment();
    void editorCreated(LiteApi::IEditor*);
    void currentEditorChanged(LiteApi::IEditor*);
protected:
    LiteApi::IApplication *m_liteApp;
    QAction    *m_commentAct;
    GolangCode *m_code;
};

class PluginFactory : public LiteApi::PluginFactoryT<GolangCodePlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
public:
    PluginFactory() {
        m_info->setId("plugin/golangcode");
        m_info->appendDepend("plugin/golangast");
        m_info->setName("GolangCode");
        m_info->setAnchor("visualfc");
        m_info->setVer("x15");
        m_info->setInfo("Golang Gocode Util");
    }
};

#endif // GOLANGCODEPLUGIN_H
