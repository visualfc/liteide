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
// Module: documentbrowserfactory.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: documentbrowserfactory.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "documentbrowserfactory.h"
#include "documentbrowser.h"
#include <QFile>
#include <QTextBrowser>
#include <QFileInfo>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

DocumentBrowserFactory::DocumentBrowserFactory(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IEditorFactory(parent),
    m_liteApp(app)
{
    m_mimeTypes << "liteide/x-browser";
}

QStringList DocumentBrowserFactory::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IEditor *DocumentBrowserFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return 0;
    }
    DocumentBrowser *browser = new DocumentBrowser(m_liteApp,0);
    if (browser->open(fileName,mimeType)) {
        return browser;
    }
    return 0;
}
