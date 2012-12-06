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
// Module: folderprojectfactory.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-09-19
// $Id: folderprojectfactory.cpp,v 1.0 2012-09-19 visualfc Exp $

#include "folderprojectfactory.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

FolderProjectFactory::FolderProjectFactory(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IProjectFactory(parent),
    m_liteApp(app)
{
    m_mimeTypes.append("folder/x-path");
}

QStringList FolderProjectFactory::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IProject *FolderProjectFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return 0;
    }
    return m_liteApp->projectManager()->openFolder(fileName);
}

bool FolderProjectFactory::findTargetInfo(const QString &fileName, const QString &mimetype, QMap<QString,QString>& targetInfo) const
{
    return false;
}
