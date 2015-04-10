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
// Module: markdownplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "markdownplugin.h"
#include "htmlpreview.h"
#include "markdownedit.h"
#include "markdownbatchbrowser.h"
#include <QtPlugin>
#include <QDebug>
#include "memory.h"

MarkdownPlugin::MarkdownPlugin()
{
}

bool MarkdownPlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;

    QAction *act = m_liteApp->editorManager()->registerBrowser(new MarkdownBatchBrowser(m_liteApp,this));
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,act);

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

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
