/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: golangasticon.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangasticon.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


/*
tools/goastview/packageview.go
const (
    tag_package      = "p"
    tag_imports_folder	 = "+m"
    tag_import		 = "mm"
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

GolangAstIconPublic::GolangAstIconPublic() :
    iconPackage(QIcon("icon:golangast/images/package.png")),
    iconImport(QIcon("icon:golangast/images/import.png")),
    iconImports(QIcon("icon:golangast/images/imports.png")),
    iconType(QIcon("icon:golangast/images/type.png")),
    iconStruct(QIcon("icon:golangast/images/struct.png")),
    iconInterface(QIcon("icon:golangast/images/interface.png")),
    iconFunc(QIcon("icon:golangast/images/func.png")),
    iconFuncs(QIcon("icon:golangast/images/funcs.png")),
    iconVar(QIcon("icon:golangast/images/var.png")),
    iconVars(QIcon("icon:golangast/images/vars.png")),
    iconConst(QIcon("icon:golangast/images/const.png")),
    iconConsts(QIcon("icon:golangast/images/consts.png")),
    iconTypeMethod(QIcon("icon:golangast/images/type_method.png")),
    iconTypeFactor(QIcon("icon:golangast/images/type_factor.png")),
    iconTypeVar(QIcon("icon:golangast/images/type_var.png")),
    iconGofile(QIcon("icon:golangast/images/gofile.png")),
    iconMakefile(QIcon("icon:golangast/images/makefile.png")),
    iconProfile(QIcon("icon:golangast/images/project.png"))
{

}

QIcon GolangAstIconPublic::iconFromTag(const QString &tag) const
{
    if (tag == "p")
        return iconPackage;
    else if (tag == "+m")
        return iconImports;
    else if (tag == "mm")
        return iconImport;
    else if (tag == "t")
        return iconType;
    else if (tag == "i")
        return iconInterface;
    else if (tag == "s")
        return iconStruct;
    else if (tag == "v")
        return iconVar;
    else if (tag == "c")
        return iconConst;
    else if (tag == "f")
        return iconFunc;
    else if (tag == "+v")
        return iconVars;
    else if (tag == "+c")
        return iconConsts;
    else if (tag == "+f")
        return iconFuncs;
    else if (tag == "tm")
        return iconTypeMethod;
    else if (tag == "tf")
        return iconTypeFactor;
    else if (tag == "tv")
        return iconTypeVar;
    return QIcon();
}

QIcon GolangAstIconPublic::iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool) const
{
    if (tag == LiteApi::TagPackage)
        return iconPackage;
    else if (tag == LiteApi::TagImportFolder)
        return iconImports;
    else if (tag == LiteApi::TagImport)
        return iconImport;
    else if (tag == LiteApi::TagType)
        return iconType;
    else if (tag == LiteApi::TagInterface)
        return iconInterface;
    else if (tag == LiteApi::TagStruct)
        return iconStruct;
    else if (tag == LiteApi::TagValue)
        return iconVar;
    else if (tag == LiteApi::TagConst)
        return iconConst;
    else if (tag == LiteApi::TagFunc)
        return iconFunc;
    else if (tag == LiteApi::TagValueFolder)
        return iconVars;
    else if (tag == LiteApi::TagConstFolder)
        return iconConsts;
    else if (tag == LiteApi::TagFuncFolder)
        return iconFuncs;
    else if (tag == LiteApi::TagTypeMethod)
        return iconTypeMethod;
    else if (tag == LiteApi::TagTypeFactor)
        return iconTypeFactor;
    else if (tag == LiteApi::TagTypeValue)
        return iconTypeVar;
    return QIcon();
}

GolangAstIconPrivate::GolangAstIconPrivate() :
    iconPackage(QIcon("icon:golangast/images/package.png")),
    iconImport(QIcon("icon:golangast/images/import.png")),
    iconImports(QIcon("icon:golangast/images/imports.png")),
    iconType(QIcon("icon:golangast/images/type_p.png")),
    iconStruct(QIcon("icon:golangast/images/struct_p.png")),
    iconInterface(QIcon("icon:golangast/images/interface_p.png")),
    iconFunc(QIcon("icon:golangast/images/func_p.png")),
    iconFuncs(QIcon("icon:golangast/images/funcs.png")),
    iconVar(QIcon("icon:golangast/images/var_p.png")),
    iconVars(QIcon("icon:golangast/images/vars.png")),
    iconConst(QIcon("icon:golangast/images/const_p.png")),
    iconConsts(QIcon("icon:golangast/images/consts.png")),
    iconTypeMethod(QIcon("icon:golangast/images/type_method_p.png")),
    iconTypeFactor(QIcon("icon:golangast/images/type_factor_p.png")),
    iconTypeVar(QIcon("icon:golangast/images/type_var_p.png"))
{

}

QIcon GolangAstIconPrivate::iconFromTag(const QString &tag) const
{
    if (tag == "p")
        return iconPackage;
    else if (tag == "+m")
        return iconImports;
    else if (tag == "mm")
        return iconImport;
    else if (tag == "t")
        return iconType;
    else if (tag == "i")
        return iconInterface;
    else if (tag == "s")
        return iconStruct;
    else if (tag == "v")
        return iconVar;
    else if (tag == "c")
        return iconConst;
    else if (tag == "f")
        return iconFunc;
    else if (tag == "+v")
        return iconVars;
    else if (tag == "+c")
        return iconConsts;
    else if (tag == "+f")
        return iconFuncs;
    else if (tag == "tm")
        return iconTypeMethod;
    else if (tag == "tf")
        return iconTypeFactor;
    else if (tag == "tv")
        return iconTypeVar;
    return QIcon();
}

QIcon GolangAstIconPrivate::iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool) const
{
    if (tag == LiteApi::TagPackage)
        return iconPackage;
    else if (tag == LiteApi::TagImportFolder)
        return iconImports;
    else if (tag == LiteApi::TagImport)
        return iconImport;
    else if (tag == LiteApi::TagType)
        return iconType;
    else if (tag == LiteApi::TagInterface)
        return iconInterface;
    else if (tag == LiteApi::TagStruct)
        return iconStruct;
    else if (tag == LiteApi::TagValue)
        return iconVar;
    else if (tag == LiteApi::TagConst)
        return iconConst;
    else if (tag == LiteApi::TagFunc)
        return iconFunc;
    else if (tag == LiteApi::TagValueFolder)
        return iconVars;
    else if (tag == LiteApi::TagConstFolder)
        return iconConsts;
    else if (tag == LiteApi::TagFuncFolder)
        return iconFuncs;
    else if (tag == LiteApi::TagTypeMethod)
        return iconTypeMethod;
    else if (tag == LiteApi::TagTypeFactor)
        return iconTypeFactor;
    else if (tag == LiteApi::TagTypeValue)
        return iconTypeVar;
    return QIcon();
}

GolangAstIcon::GolangAstIcon()
{
}

GolangAstIcon *GolangAstIcon::instance()
{
    static GolangAstIcon icons;
    return &icons;
}

QIcon GolangAstIcon::iconFromTag(const QString &tag, bool pub) const
{
    if (pub) {
        return icons.iconFromTag(tag);
    }
    return icons_p.iconFromTag(tag);
}

QIcon GolangAstIcon::iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool pub) const
{
    if (pub) {
        return icons.iconFromTagEnum(tag);
    }
    return icons_p.iconFromTagEnum(tag);
}
