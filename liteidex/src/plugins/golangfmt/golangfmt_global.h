/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: golangfmt_global.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGFMT_GLOBAL_H
#define GOLANGFMT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOLANGFMT_LIBRARY)
#  define GOLANGFMTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GOLANGFMTSHARED_EXPORT Q_DECL_IMPORT
#endif

#define GOLANGFMT_FIXIMPORTS "golangfmt/goimportsstyle"
#define GOLANGFMT_SORTIMPORTS "golangfmt/sortimports"
#define GOLANGFMT_AUTOFMT "golangfmt/autofmt"
#define GOLANGFMT_USESYNCFMT "golangfmt/syncfmt"
#define GOLANGFMT_SYNCTIMEOUT "golangfmt/synctimeout"
#define GOLANGFMT_TAG "golangfmt/navtag"

#endif // GOLANGFMT_GLOBAL_H
