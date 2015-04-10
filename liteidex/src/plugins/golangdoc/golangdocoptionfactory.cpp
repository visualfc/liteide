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
// Module: golangdocoptionfactory.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangdocoption.h"
#include "golangdocoptionfactory.h"
#include "memory.h"

GolangDocOptionFactory::GolangDocOptionFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IOptionFactory(parent),
      m_liteApp(app)
{
}

QStringList GolangDocOptionFactory::mimeTypes() const
{
    return QStringList() << "option/golangdoc";
}

LiteApi::IOption *GolangDocOptionFactory::create(const QString &mimeType)
{
    if (mimeType == "option/golangdoc") {
        return new GolangDocOption(m_liteApp,this);
    }
    return 0;
}
