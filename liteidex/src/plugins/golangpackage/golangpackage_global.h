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
// Module: golangpackage_global.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGPACKAGE_GLOBAL_H
#define GOLANGPACKAGE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOLANGPACKAGE_LIBRARY)
#  define GOLANGPACKAGESHARED_EXPORT Q_DECL_EXPORT
#else
#  define GOLANGPACKAGESHARED_EXPORT Q_DECL_IMPORT
#endif

#define LITEIDE_USESYSGOPATH "liteide/usesysgopath"
#define LITEIDE_USELITEIDEGOPATH "liteide/uselitegopath"

#endif // GOLANGPACKAGE_GLOBAL_H
