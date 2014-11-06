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
// Module: filebrowserplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILEBROWSERPLUGIN_H
#define FILEBROWSERPLUGIN_H

#include "filebrowser_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class FileBrowser;
class FileBrowserPlugin : public LiteApi::IPlugin
{
public:
    FileBrowserPlugin();
    virtual bool load(LiteApi::IApplication *app);
protected:
    FileBrowser *m_browser;
};

class PluginFactory : public LiteApi::PluginFactoryT<FileBrowserPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.FileBrowserPlugin")
#endif
public:
    PluginFactory()
    {
        m_info->setId("plugin/filebrowser");
        m_info->setName("FileBrowser");
        m_info->setAuthor("visualfc");
        m_info->setVer("x24.3");
        m_info->setInfo("File System Browser");
    }
};

#endif // FILEBROWSERPLUGIN_H
