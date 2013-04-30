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
// Module: litebuildplugin.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEBUILDPLUGIN_H
#define LITEBUILDPLUGIN_H

#include "litebuild_global.h"
#include "liteapi/liteapi.h"
#include "elidedlabel/elidedlabel.h"
#include <QtPlugin>

class LiteBuild;
class QLineEdit;
class LiteBuildPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    LiteBuildPlugin();
    virtual bool load(LiteApi::IApplication *app);
protected slots:
    void showExecute();
    void execute();
    void currentEditorChanged(LiteApi::IEditor*);
protected:
    LiteApi::IApplication *m_liteApp;
    LiteBuild *m_build;
    QWidget   *m_executeWidget;
    QLineEdit *m_executeEdit;
    ElidedLabel *m_workLabel;
};

class PluginFactory : public LiteApi::PluginFactoryT<LiteBuildPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
public:
    PluginFactory() {
        m_info->setId("plugin/litebuild");
        m_info->setName("LiteBuilder");
        m_info->setAnchor("visualfc");
        m_info->setVer("x18");
        m_info->setInfo("Core Build");
        m_info->appendDepend("plugin/liteenv");
        m_info->setMustLoad(true);
    }
};

#endif // LITEBUILDPLUGIN_H
