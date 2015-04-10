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
// Module: folderprojectfactory.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "folderprojectfactory.h"
#include "memory.h"

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

bool FolderProjectFactory::findTargetInfo(const QString&, const QString&, QMap<QString,QString>&) const
{
    return false;
}
