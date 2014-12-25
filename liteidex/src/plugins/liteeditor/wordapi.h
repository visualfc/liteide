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
// Module: wordapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEWORDAPI_H
#define LITEWORDAPI_H

#include "liteeditorapi/liteeditorapi.h"

class WordApi : public LiteApi::IWordApi
{
public:
    WordApi(const QString &package);
    virtual QString package() const;
    virtual QStringList apiFiles() const;
    virtual bool loadApi();
    virtual QStringList wordList() const;
    virtual QStringList expList() const;
    virtual void appendExp(const QStringList &list);

    void appendApiFile(const QString &file);
    void setApiFiles(const QStringList &files);
    bool isEmpty() const;
protected:
    QString m_package;
    QStringList m_apiFiles;
    QStringList m_wordList;
    QStringList m_expList;
    bool m_bLoad;
};
#endif // LITEWORDAPI_H
