/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: liteapp_global.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-12-4
// $Id: liteapp_global.h,v 1.0 2012-12-4 visualfc Exp $

#ifndef LITEAPP_GLOBAL_H
#define LITEAPP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LITEAPP_LIBRARY)
#  define LITEIDESHARED_EXPORT Q_DECL_EXPORT
#else
#  define LITEIDESHARED_EXPORT Q_DECL_IMPORT
#endif

#define OPTION_LITEAPP "option/liteapp"

#define LITEAPP_MAXRECENTFILES "LiteApp/MaxRecentFiles"
#define LITEAPP_AUTOCLOSEPROEJCTFILES "LiteApp/AutoCloseProjectEditors"
#define LITEAPP_AUTOLOADLASTSESSION "LiteApp/AutoLoadLastSession"
#define LITEAPP_LANGUAGE "General/Language"
#define LITEAPP_SPLASHVISIBLE "LiteApp/SplashVisible"
#define LITEAPP_WELCOMEPAGEVISIBLE "General/WelcomePageVisible"

#endif // LITEAPP_GLOBAL_H
