/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: macsupport.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MACSUPPORT_H
#define MACSUPPORT_H

#include <QMainWindow>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QOperatingSystemVersion>
#else
#include <QSysInfo>
#endif

class MacSupport
{
public:
    static void setFullScreen(QMainWindow *window);
    static bool isLionOrHigh()
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        return QOperatingSystemVersion::current()
                >= QOperatingSystemVersion(QOperatingSystemVersion::MacOS, 10, 7);
#else
        return QSysInfo::MacintoshVersion > QSysInfo::MV_10_6;
#endif
    }
};

#endif // MACSUPPORT_H
