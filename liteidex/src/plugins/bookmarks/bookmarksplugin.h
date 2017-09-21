/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
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
// Module: bookmarksplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef BOOKMARKSPLUGIN_H
#define BOOKMARKSPLUGIN_H

#include "bookmarks_global.h"
#include "liteapi/liteapi.h"

class BookmarksPlugin : public LiteApi::IPlugin
{
public:
    BookmarksPlugin();
    virtual bool load(LiteApi::IApplication *app);
};

class PluginFactory : public LiteApi::PluginFactoryT<BookmarksPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.BookmarksPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/Bookmarks");
        m_info->setVer("X33");
        m_info->setName("Bookmarks");
        m_info->setAuthor("visualfc");
        m_info->setInfo("Bookmarks");
        //m_info->setMustLoad(true);
        m_info->appendDepend("plugin/liteeditor");
    }
};


#endif // BOOKMARKSPLUGIN_H
