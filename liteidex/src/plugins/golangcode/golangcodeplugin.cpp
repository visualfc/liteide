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
// Module: golangcodeplugin.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangcodeplugin.h"
#include "liteeditorapi/liteeditorapi.h"
#include "qtc_editutil/uncommentselection.h"
#include "golangcode.h"
#include "golangcodeoptionfactory.h"
#include <QMenu>
#include <QAction>
#include <QPlainTextEdit>
#include <QFileInfo>
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

GolangCodePlugin::GolangCodePlugin()
{
}

bool GolangCodePlugin::load(LiteApi::IApplication *app)
{
    m_liteApp = app;
    m_code = new GolangCode(app,this);

    app->optionManager()->addFactory(new GolangCodeOptionFactory(app,this));
    connect(app->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(app->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));
    return true;
}

QStringList GolangCodePlugin::dependPluginList() const
{
    return QStringList() << "plugin/liteenv" << "plugin/golangast";
}

void GolangCodePlugin::appLoaded()
{
}

void GolangCodePlugin::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    if (editor->mimeType() != "text/x-gosrc") {
        return;
    }
    LiteApi::ILiteEditor *liteEdit = LiteApi::getLiteEditor(editor);
    if (liteEdit) {
        liteEdit->setSpellCheckZoneDontComplete(true);
    }
}

void GolangCodePlugin::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (editor) {
        if (editor->mimeType() == "text/x-gosrc") {
            LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
            m_code->setCompleter(completer);
            return;
        } else if (editor->mimeType() == "browser/goplay") {
            LiteApi::IEditor* editor = LiteApi::findExtensionObject<LiteApi::IEditor*>(m_liteApp->extension(),"LiteApi.Goplay.IEditor");
            if (editor && editor->mimeType() == "text/x-gosrc") {
                LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
                m_code->setCompleter(completer);
                return;
            }
        }
    }
    m_code->setCompleter(0);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PluginFactory,PluginFactory)
#endif
