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
// Module: liteeditorplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditorplugin.h"
#include "liteeditorfilefactory.h"
#include "liteeditoroptionfactory.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteEditorPlugin::LiteEditorPlugin()
{
    m_info->setId("plugin/liteeditor");
    m_info->setName("LiteEditor");
    m_info->setAnchor("visualfc");
    m_info->setVer("x14.0");
    m_info->setInfo("LiteIDE Editor Plugin");
}

bool LiteEditorPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    LiteEditorFileFactory *factory = new LiteEditorFileFactory(app,this);
    app->editorManager()->addFactory(factory);

    app->optionManager()->addFactory(new LiteEditorOptionFactory(app,this));

    return true;
}

QStringList LiteEditorPlugin::dependPluginList() const
{
    return QStringList() << "plugin/litebuild";
}

Q_EXPORT_PLUGIN(PluginFactory)
