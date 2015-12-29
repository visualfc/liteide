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
// Module: golangasticon.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGASTICON_H
#define GOLANGASTICON_H

#include <QIcon>
#include "golangastapi/golangastapi.h"

class GolangAstIconPublic
{
public:
    GolangAstIconPublic();
    QIcon iconFromTag(const QString &tag) const;
    QIcon iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool pub = true) const;
protected:
    QIcon   iconPackage;
    QIcon   iconImport;
    QIcon   iconImports;
    QIcon   iconType;
    QIcon   iconStruct;
    QIcon   iconInterface;
    QIcon   iconFunc;
    QIcon   iconFuncs;
    QIcon   iconVar;
    QIcon   iconVars;
    QIcon   iconConst;
    QIcon   iconConsts;
    QIcon   iconTypeMethod;
    QIcon   iconTypeFactor;
    QIcon   iconTypeVar;
public:
    QIcon   iconGofile;
    QIcon   iconMakefile;
    QIcon   iconProfile;
};

class GolangAstIconPrivate
{
public:
    GolangAstIconPrivate();
    QIcon iconFromTag(const QString &tag) const;
    QIcon iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool pub = true) const;
protected:
    QIcon   iconPackage;
    QIcon   iconImport;
    QIcon   iconImports;
    QIcon   iconType;
    QIcon   iconStruct;
    QIcon   iconInterface;
    QIcon   iconFunc;
    QIcon   iconFuncs;
    QIcon   iconVar;
    QIcon   iconVars;
    QIcon   iconConst;
    QIcon   iconConsts;
    QIcon   iconTypeMethod;
    QIcon   iconTypeFactor;
    QIcon   iconTypeVar;
};

class GolangAstIcon
{
public:
    static GolangAstIcon *instance();
    QIcon iconFromTag(const QString &tag, bool pub = true) const;
    QIcon iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool pub = true) const;
protected:
    GolangAstIcon();
    GolangAstIconPrivate icons_p;
    GolangAstIconPublic  icons;
};

#endif // GOLANGASTICON_H
