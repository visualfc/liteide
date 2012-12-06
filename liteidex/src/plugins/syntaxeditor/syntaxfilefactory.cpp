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
// Module: syntaxfilefactory.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "syntaxfilefactory.h"
#include "syntaxtexteditor.h"
#include "syntaxeditor.h"
#include "golanghighlighter.h"
#include "projecthighlighter.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


SyntaxFileFactory::SyntaxFileFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IFileFactory(parent),
      m_liteApp(app)
{
    m_mimeTypes.append("text/x-gosrc");
    m_mimeTypes.append("text/x-gopro");
    m_mimeTypes.append("text/x-profile");
    m_mimeTypes.append("text/liteide.default.editor");
}

QStringList SyntaxFileFactory::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IFile *SyntaxFileFactory::open(const QString &fileName, const QString &mimeType)
{
    SyntaxEditor *edit = new SyntaxEditor(this);
    if (!edit->open(fileName,mimeType)) {
        delete edit;
        return 0;
    }

    if (mimeType == "text/x-gosrc") {
        new GolangHighlighter(edit->editor()->document());
    } else if (mimeType == "text/x-gopro") {
        new ProjectHighlighter(edit->editor()->document());
    }
    edit->editor()->loadConfig(m_liteApp->settings(),mimeType);
    m_liteApp->editorManager()->openEditor(edit);
    return edit->file();
}
