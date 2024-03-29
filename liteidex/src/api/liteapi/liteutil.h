/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
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
// Module: liteutil.h
// Creator: visualfc <visualfc@gmail.com>
#ifndef LITEUTIL_H
#define LITEUTIL_H

#include "liteapi.h"

namespace LiteApi {

inline void updateSetting(QSettings *setting, const QString &key, const QVariant &value, const QVariant &def)
{
    if (value == def) {
        setting->remove(key);
    } else {
        setting->setValue(key,value);
    }
}

inline void updateAppSetting(LiteApi::IApplication *app, const QString &key, const QVariant &value, const QVariant &def)
{
    updateSetting(app->settings(),key,value,def);
}

}


#endif // LITEUTIL_H
