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
// Module: golangdocplugin.h
// Creator: visualfc <visualfc@gmail.com>



#ifndef GOLANGDOCPLUGIN_H
#define GOLANGDOCPLUGIN_H

#include "golangdoc_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class GolangDoc;
class GolangDocPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    GolangDocPlugin();
    ~GolangDocPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
protected slots:
    void currentEditorChanged(LiteApi::IEditor*);
protected:
    GolangDoc *m_golangDoc;
};

class PluginFactory : public LiteApi::PluginFactoryT<GolangDocPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
};

#endif // GOLANGDOCPLUGIN_H
