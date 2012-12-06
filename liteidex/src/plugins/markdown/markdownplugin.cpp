/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: markdownplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-21
// $Id: markdownplugin.cpp,v 1.0 2012-11-21 visualfc Exp $

#include "markdownplugin.h"
#include "htmlpreview.h"
#include "markdownedit.h"
#include <QtPlugin>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

MarkdownPlugin::MarkdownPlugin()
{
    m_info->setId("plugin/Markdown");
    m_info->setVer("x15.0");
    m_info->setName("Markdown");
    m_info->setAnchor("visualfc");
    m_info->setInfo("Markdown Plugin");
}

bool MarkdownPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));

    new HtmlPreview(m_liteApp,this);

    return true;
}

void MarkdownPlugin::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor || editor->mimeType() != "text/x-markdown") {
        return;
    }
    new MarkdownEdit(m_liteApp,editor,this);
}

Q_EXPORT_PLUGIN(PluginFactory)
