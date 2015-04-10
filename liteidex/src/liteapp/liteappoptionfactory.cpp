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
// Module: liteappoptionfactory.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteappoption.h"
#include "outputoption.h"
#include "liteappoptionfactory.h"
#include "liteapp_global.h"
#include "memory.h"

LiteAppOptionFactory::LiteAppOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent),
      m_liteApp(app)
{
}

QStringList LiteAppOptionFactory::mimeTypes() const
{
    return QStringList() << OPTION_LITEAPP << OPTION_OUTPUT;
}

LiteApi::IOption *LiteAppOptionFactory::create(const QString &mimeType)
{
    if (mimeType == OPTION_LITEAPP) {
        return new LiteAppOption(m_liteApp,this);
    } else if (mimeType == OPTION_OUTPUT) {
        return new OutputOption(m_liteApp,this);
    }
    return 0;
}
