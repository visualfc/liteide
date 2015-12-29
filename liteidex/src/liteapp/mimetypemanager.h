/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: mimetypemanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MIMETYPEMANAGER_H
#define MIMETYPEMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class MimeTypeManager : public IMimeTypeManager
{
public:
    ~MimeTypeManager();
    virtual bool addMimeType(IMimeType *mimeType);
    virtual void removeMimeType(IMimeType *mimeType);
    virtual QList<IMimeType*> mimeTypeList() const;
    virtual IMimeType *findMimeType(const QString &type) const;
    virtual QString findPackageByMimeType(const QString &type) const;
    virtual QString findMimeTypeByFile(const QString &fileName) const;
    virtual QString findMimeTypeBySuffix(const QString &suffix) const;
    virtual QString findMimeTypeByScheme(const QString &scheme) const;
    virtual QStringList findAllFilesByMimeType(const QString &dir, const QString &type, int deep = 0) const;
    void loadMimeTypes(const QString &path);
protected:
    QList<IMimeType*>   m_mimeTypeList;
};

#endif // MIMETYPEMANAGER_H
