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
// Module: golangastapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef __GOLANGASTAPI_H__
#define __GOLANGASTAPI_H__

#include "liteapi/liteapi.h"
#include <QProcessEnvironment>

namespace LiteApi {
/*
const (
        tag_package      = "p"
        tag_type         = "t"
        tag_struct       = "s"
        tag_interface    = "i"
        tag_value        = "v"
        tag_const        = "c"
        tag_func         = "f"
        tag_value_folder = "+v"
        tag_const_folder = "+c"
        tag_func_folder  = "+f"
        tag_type_method  = "tm"
        tag_type_factor  = "tf"
        tag_type_value   = "tv"
)
*/

enum ASTTAG_ENUM {
    TagNone = 0,
    TagPackage,
    TagType,
    TagStruct,
    TagInterface,
    TagValue,
    TagConst,
    TagFunc,
    TagValueFolder,
    TagConstFolder,
    TagFuncFolder,
    TagTypeMethod,
    TagTypeFactor,
    TagTypeValue
};

class IGolangAst : public QObject
{
    Q_OBJECT
public:
    IGolangAst(QObject *parent = 0): QObject(parent) {}
    virtual ~IGolangAst() {}
    virtual QIcon iconFromTag(const QString &tag, bool pub) const = 0;
    virtual QIcon iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool pub) const = 0;
};

} //namespace LiteApi


#endif //__GOLANGASTAPI_H__

