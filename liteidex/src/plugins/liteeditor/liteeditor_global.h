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
// Module: liteeditor_global.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditor_global.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef LITEEDITOR_GLOBAL_H
#define LITEEDITOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LITEEDITOR_LIBRARY)
#  define LITEEDITORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LITEEDITORSHARED_EXPORT Q_DECL_IMPORT
#endif

#define OPTION_LITEEDITOR "option/liteeditor"
#define EDITOR_STYLE "editor/style"
#define EDITOR_FAMILY "editor/family"
#define EDITOR_FONTSIZE "editor/fontsize"
#define EDITOR_ANTIALIAS "editor/antialias"
#define EDITOR_NOPRINTCHECK "editor/noprintcheck"
#define EDITOR_AUTOINDENT "editor/autoindent"
#define EDITOR_AUTOBRACE0 "editor/autobraces0"
#define EDITOR_AUTOBRACE1 "editor/autobraces1"
#define EDITOR_AUTOBRACE2 "editor/autobraces2"
#define EDITOR_AUTOBRACE3 "editor/autobraces3"
#define EDITOR_AUTOBRACE4 "editor/autobraces4"
#define EDITOR_AUTOBRACE5 "editor/autobraces5"
#define EDITOR_COMPLETER_CASESENSITIVE "editor/ComplererCaseSensitive"
#define EDITOR_LINENUMBERVISIBLE "editor/linenumbervisible"
#define EDITOR_PREFIXLENGTH "editor/prefixlength"
#define EDITOR_RIGHTLINEVISIBLE "editor/rightlinevisible"
#define EDITOR_RIGHTLINEWIDTH "editor/rightlinewidth"

#endif // LITEEDITOR_GLOBAL_H
