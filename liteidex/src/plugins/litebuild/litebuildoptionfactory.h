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
// Module: litebuildoptionfactory.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: litebuildoptionfactory.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef LITEBUILDOPTIONFACTORY_H
#define LITEBUILDOPTIONFACTORY_H

#include "liteapi/liteapi.h"

class LiteBuildOptionFactory : public LiteApi::IOptionFactory
{
public:
    LiteBuildOptionFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IOption *create(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;

};

#endif // LITEBUILDOPTIONFACTORY_H
