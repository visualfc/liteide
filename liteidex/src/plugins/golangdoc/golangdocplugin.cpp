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
// Module: golangdocplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangdocplugin.h"
#include "golangdoc.h"
#include "golangdocoptionfactory.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangDocPlugin::GolangDocPlugin()
    : m_golangDoc(0)
{
    m_info->setId("plugin/golangdoc");
    m_info->setName("GolangDoc");
    m_info->setAnchor("visualfc");
    m_info->setVer("x14.0");
    m_info->setInfo("Golang DocBrowser Plugin");
    m_info->appendDepend("plugin/liteenv");
    m_info->appendDepend("plugin/liteeditor");
}

GolangDocPlugin::~GolangDocPlugin()
{
    if (m_golangDoc) {
        delete m_golangDoc;
    }
}

bool GolangDocPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    m_golangDoc = new GolangDoc(app,this);
    m_liteApp->optionManager()->addFactory(new GolangDocOptionFactory(m_liteApp,this));

    return true;
}

void GolangDocPlugin::currentEditorChanged(LiteApi::IEditor *editor)
{
    bool active = false;
    if (editor && editor->mimeType() == "text/x-gosrc") {
        active = true;
    }
}

Q_EXPORT_PLUGIN(PluginFactory)
