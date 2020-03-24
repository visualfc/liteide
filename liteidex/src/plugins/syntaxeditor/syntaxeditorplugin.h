/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE. All rights reserved.
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
// Module: syntaxeditorplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SYNTAXEDITORPLUGIN_H
#define SYNTAXEDITORPLUGIN_H

#include "syntaxeditor_global.h"
#include "../../liteapi/liteapi.h"

#include <QtPlugin>

class SyntaxEditorPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    SyntaxEditorPlugin();
    virtual void install(LiteApi::IApplication *app);
    virtual const LiteApi::PluginInfo & info() const;
};

#endif // SYNTAXEDITORPLUGIN_H
