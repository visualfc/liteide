/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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

#include "packageprojectfactory.h"
#include "packageproject.h"
#include "memory.h"

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

bool PackageProjectFactory::findTargetInfo(const QString&, const QString&, QMap<QString,QString>&) const
{
    return false;
}
