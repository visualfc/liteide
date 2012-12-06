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
// Module: wordapimanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: wordapimanager.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef WORDAPIMANAGER_H
#define WORDAPIMANAGER_H

#include "liteeditorapi/liteeditorapi.h"

using namespace LiteApi;

class WordApiManager : public IWordApiManager
{
public:
    WordApiManager(QObject *parent = 0);
    ~WordApiManager();
    virtual void addWordApi(IWordApi *wordApi);
    virtual void removeWordApi(IWordApi *wordApi);
    virtual IWordApi *findWordApi(const QString &mimeType);
    virtual QList<IWordApi*> wordApiList() const;
public:
    void load(const QString &path);
protected:
    QList<IWordApi*>    m_wordApiList;
};

#endif // WORDAPIMANAGER_H
