/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE. All rights reserved.
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
// Module: golanglint.cpp
// Creator: Hai Thanh Nguyen <phaikawl@gmail.com>

#ifndef GOLANGLINT_GLOBAL_H
#define GOLANGLINT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOLANGLINT_LIBRARY)
#  define GOLANGLINTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GOLANGLINTSHARED_EXPORT Q_DECL_IMPORT
#endif

#define GOLANGLINT_CONFIDENCE "golanglint/confidence"
#define GOLANGLINT_AUTOLINT "golanglint/confidence"
#define GOLANGLINT_SYNCTIMEOUT "golanglint/synctimeout"
#define GOLANGLINT_TAG "golanglint/navigatetag"

#endif // GOLANGLINT_GLOBAL_H
