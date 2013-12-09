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
// Module: folderprojectfactory.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FOLDERPROJECTFACTORY_H
#define FOLDERPROJECTFACTORY_H

#include "liteapi/liteapi.h"

class FolderProjectFactory : public LiteApi::IProjectFactory
{
public:
    FolderProjectFactory(LiteApi::IApplication *app, QObject *parnet);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IProject *open(const QString &fileName, const QString &mimeType);
    virtual bool findTargetInfo(const QString &fileName, const QString &mimetype,  QMap<QString,QString>& targetInfo) const;
public:
    LiteApi::IApplication *m_liteApp;
    QStringList m_mimeTypes;
};

#endif // FOLDERPROJECTFACTORY_H
