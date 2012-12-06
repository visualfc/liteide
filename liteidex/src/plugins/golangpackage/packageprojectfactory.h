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
// Module: packageprojectfactory.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-24
// $Id: packageprojectfactory.h,v 1.0 2012-4-24 visualfc Exp $

#ifndef PACKAGEPROJECTFACTORY_H
#define PACKAGEPROJECTFACTORY_H

#include "liteapi/liteapi.h"
#include "packagebrowser.h"

class PackageProjectFactory : public LiteApi::IProjectFactory
{
public:
    PackageProjectFactory(LiteApi::IApplication *app, QObject *parnet);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IProject *open(const QString &fileName, const QString &mimeType);
    virtual bool findTargetInfo(const QString &fileName, const QString &mimetype,  QMap<QString,QString>& targetInfo) const;
public:
    LiteApi::IApplication *m_liteApp;
    QStringList m_mimeTypes;
};

#endif // PACKAGEPROJECTFACTORY_H
