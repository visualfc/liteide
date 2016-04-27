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
// Module: fakevimedit_global.h
// Creator: jsuppe <jon.suppe@gmail.com>

#ifndef FAKEVIMEDIT_GLOBAL_H
#define FAKEVIMEDIT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FAKEVIMEDIT_LIBRARY)
#  define FAKEVIMEDITSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FAKEVIMEDITSHARED_EXPORT Q_DECL_IMPORT
#endif

#define OPTION_FAKEVIMEDIT   "option/fakevimedit"
#define FAKEVIMEDIT_USEFAKEVIM "fakevimedit/usefakevim"

#endif // FAKEVIMEDIT_GLOBAL_H
