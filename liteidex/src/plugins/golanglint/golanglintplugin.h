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
// Module: golanglint.cpp
// Creator: Hai Thanh Nguyen <phaikawl@gmail.com>

#ifndef GOLANGLINTPLUGIN_H
#define GOLANGLINTPLUGIN_H

#include "golanglint_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class GolangLint;
class QToolButton;
class GolangLintPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
public:
    GolangLintPlugin();
    virtual bool load(LiteApi::IApplication *app);
    virtual QStringList dependPluginList() const;
protected slots:
    void editorCreated(LiteApi::IEditor*);
    void appLoaded();
    void goplayLint();
protected:
    LiteApi::IApplication *m_liteApp;
    QAction   *m_golintAct;
    QAction   *m_goplayAct;
    LiteApi::IEditor *m_playEditor;
    QToolButton *m_lintBtn;
    GolangLint *m_lint;
};

class PluginFactory : public LiteApi::PluginFactoryT<GolangLintPlugin>
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPluginFactory)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "liteidex.GolangLintPlugin")
#endif
public:
    PluginFactory() {
        m_info->setId("plugin/golanglint");
        m_info->setName("GolangLint");
        m_info->setAuthor("Hai Thanh Nguyen");
        m_info->setVer("0.1");
        m_info->setInfo("Golang Golint Util");
    }
};


#endif // GOLANGLINTPLUGIN_H
