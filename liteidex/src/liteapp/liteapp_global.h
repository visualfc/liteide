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
// Module: liteapp_global.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEAPP_GLOBAL_H
#define LITEAPP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LITEAPP_LIBRARY)
#  define LITEIDESHARED_EXPORT Q_DECL_EXPORT
#else
#  define LITEIDESHARED_EXPORT Q_DECL_IMPORT
#endif

#define LITEIDE_STORELOCAL "LiteIDE/StoreLocal"

#define OPTION_LITEAPP "option/liteapp"
#define OPTION_LITEOUTPUT  "option/liteoutput"

#define LITEAPP_MAXRECENTFILES "LiteApp/MaxRecentFile"
#define LITEAPP_AUTOCLOSEPROEJCTFILES "LiteApp/AutoCloseProjectEditors"
#define LITEAPP_AUTOLOADLASTSESSION "LiteApp/AutoLoadLastSession"
#define LITEAPP_AUTOIDLESAVEDOCUMENTS "LiteApp/AutoIdleSaveDocuments"
#define LITEAPP_AUTOIDLESAVEDOCUMENTS_TIME  "LiteApp/AutoIdelSaveDocumentsTime"
#define LITEAPP_LANGUAGE "General/Language"
#define LITEAPP_STYLE "LiteApp/WindowStyle"
#define LITEAPP_SPLASHVISIBLE "LiteApp/SplashVisible"
#define LITEAPP_WELCOMEPAGEVISIBLE "General/WelcomePageVisible"
#define LITEAPP_TOOLBARICONSIZE "General/ToolBarIconSize"
#define LITEAPP_EDITTABSCLOSABLE "LiteApp/EditTabsClosable"
#define LITEAPP_EDITTABSENABLEWHELL "LiteApp/EditTabEnableWhell"
#define LITEAPP_SHOWEDITTOOLBAR "LiteApp/ShowEditToolbar"
#define LITEAPP_QSS "LitApp/Qss"
#define LITEAPP_FULLSCREEN "LiteApp/FullScreen"
#define LITEAPP_WINSTATE   "LiteApp/WinState"
#define LITEAPP_SHORTCUTS "keybord_shortcuts/"
#define LITEAPP_OPTNFOLDERINNEWWINDOW "LiteApp/OpenFolderInNewWindow"
#define LITEAPP_FOLDERSHOWHIDENFILES "LiteApp/FolderShowHidenFiles"
#define LITEAPP_FOLDERSHOWDETAILS "LiteApp/FolderShowDetails"
#define LITEAPP_STARTUPRELOADFILES   "LiteApp/StartupReloadFiles"
#define LITEAPP_STARTUPRELOADFOLDERS   "LiteApp/StartupReloadFolders"
#define LITEAPP_FILEWATCHERAUTORELOAD "LiteApp/FileWatcherAutoReload"
#define LITEIDE_CUSTOMEICONPATH    "LiteApp/CustomeIconPath"
#define LITEIDE_CUSTOMEICON "LiteApp/CustomeIcon"

#define OUTPUT_FAMILY "output/family"
#define OUTPUT_FONTSIZE "output/fontsize"
#define OUTPUT_FONTZOOM "output/fontzoom"
#define OUTPUT_ANTIALIAS "output/antialias"
#define OUTPUT_MAXLINES  "output/maxlines"
#define OUTPUT_USECOLORSCHEME "output/colorscheme"

#define LITEAPP_FILESFILTER_MAXCOUNT "LiteApp/FilesFilterMaxCount"

#define LITEAPP_SESSIONLIST "LiteApp/SessionList"


enum TOOLBAR_ICONSIZE {
    TOOLBAR_ICONSIZE_16 = 0,
    TOOLBAR_ICONSIZE_18,
    TOOLBAR_ICONSIZE_20,
    TOOLBAR_ICONSIZE_22,
    TOOLBAR_ICONSIZE_24
};

#endif // LITEAPP_GLOBAL_H
