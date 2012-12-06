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
// Module: packageprojectfactory.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-24
// $Id: packageprojectfactory.cpp,v 1.0 2012-4-24 visualfc Exp $

#include "packageprojectfactory.h"
#include <packageproject.h>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

PackageProjectFactory::PackageProjectFactory(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IProjectFactory(parent),
    m_liteApp(app)
{
    m_mimeTypes.append("text/x-gopackage");
    m_mimeTypes.append("text/x-gocommand");
}

QStringList PackageProjectFactory::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IProject *PackageProjectFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return 0;
    }
    PackageProject *project = new PackageProject(m_liteApp);
    project->setPath(fileName);
    project->reload();

    return project;
}

bool PackageProjectFactory::findTargetInfo(const QString &fileName, const QString &mimetype, QMap<QString,QString>& targetInfo) const
{
    return false;
}
