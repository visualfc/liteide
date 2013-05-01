/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: markdownplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MARKDOWNPLUGIN_H
#define MARKDOWNPLUGIN_H

#include "markdown_global.h"
#include "liteapi/liteapi.h"

class MarkdownPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    MarkdownPlugin();
    virtual bool load(LiteApi::IApplication *app);
protected slots:
    void editorCreated(LiteApi::IEditor*);
protected:
    LiteApi::IApplication *m_liteApp;
};

class PluginFactory : public LiteApi::PluginFactoryT<MarkdownPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
public:
    PluginFactory() {
        m_info->setId("plugin/Markdown");
        m_info->setVer("x18");
        m_info->setName("Markdown");
        m_info->setAnchor("visualfc");
        m_info->setInfo("Markdown Editor");
        m_info->appendDepend("plugin/liteeditor");
    }
};


#endif // MARKDOWNPLUGIN_H
