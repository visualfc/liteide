/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: quickopen_global.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef QUICKOPEN_GLOBAL_H
#define QUICKOPEN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QUICKOPEN_LIBRARY)
#  define QUICKOPENSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUICKOPENSHARED_EXPORT Q_DECL_IMPORT
#endif

#define OPTION_QUICKOPEN "option/quickopen"
#define QUICKOPEN_FILES_MAXCOUNT "quickopen/filesmaxcount"
#define QUICKOPNE_FILES_MATCHCASE "quickopen/filesmatchscase"
#define QUICKOPEN_FOLDER_MAXCOUNT "quickopen/foldermaxcount"
#define QUICKOPNE_FOLDER_MATCHCASE "quickopen/foldermatchscase"
#define QUICKOPNE_EDITOR_MATCHCASE "quickopen/editormatchcase"

#endif // QUICKOPEN_GLOBAL_H
