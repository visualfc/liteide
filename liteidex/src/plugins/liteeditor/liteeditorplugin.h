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
// Module: liteeditorplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORPLUGIN_H
#define LITEEDITORPLUGIN_H

#include "liteeditor_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class LiteEditorPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    LiteEditorPlugin();
    virtual bool load(LiteApi::IApplication *app);
public slots:
    void editorToolBarVisibleChanged(bool b);
    void editorNavigateVisibleChanged(bool b);
protected:
    LiteApi::IApplication *m_liteApp;
    QAction *m_toolBarAct;
    QAction *m_navBarAct;
};

class PluginFactory : public LiteApi::PluginFactoryT<LiteEditorPlugin>
{
    Q_OBJECT    
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.LiteEditorPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/liteeditor");
        m_info->setName("LiteEditor");
        m_info->setAuthor("visualfc");
        m_info->setVer("X38.1");
        m_info->setInfo("Core Editor");
        m_info->setMustLoad(true);
    }
};

#endif // LITEEDITORPLUGIN_H
