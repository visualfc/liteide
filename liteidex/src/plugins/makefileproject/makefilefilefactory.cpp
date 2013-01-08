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
// Module: makefilefilefactory.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "makefilefilefactory.h"
#include "makefileproject.h"
#include "makefilefile.h"
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


MakefileFileFactory::MakefileFileFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IProjectFactory(parent),
      m_liteApp(app)
{
    m_mimeTypes.append("text/x-makefile");
    m_mimeTypes.append("text/x-gomake");
}

QStringList MakefileFileFactory::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IProject *MakefileFileFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return 0;
    }
    MakefileProject *project = new MakefileProject(m_liteApp);
    project->setModelFile(new MakefileFile(m_liteApp,project));
    if (!project->open(fileName,mimeType)) {
        delete project;
        return 0;
    }
    return project;
}

bool MakefileFileFactory::findTargetInfo(const QString &fileName, const QString &mimeType, QMap<QString,QString>& targetInfo) const
{
    if (!m_mimeTypes.contains(mimeType)) {
        return false;
    }
    MakefileFile *file = new MakefileFile(m_liteApp,0);
    bool b = file->open(fileName,mimeType);
    if (b) {
        targetInfo = file->targetInfo();
    }
    delete file;
    return b;
}
