/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE. All rights reserved.
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
// Module: makefilefile.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MAKEFILEFILE_H
#define MAKEFILEFILE_H

#include "modelproject/modelfileimpl.h"

class MakefileFile : public ModelFileImpl
{
public:
    MakefileFile(LiteApi::IApplication *app, QObject *parent);
protected:
    virtual bool loadFile(const QString &filePath);
    virtual void updateModel();
public:
    virtual QString target() const;
    virtual QString targetPath() const;
    virtual QMap<QString,QString> targetInfo() const;
};

#endif // MAKEFILEFILE_H
