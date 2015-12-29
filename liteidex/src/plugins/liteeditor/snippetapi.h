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
// Module: snippetapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITESNIPPETAPI_H
#define LITESNIPPETAPI_H

#include "liteeditorapi/liteeditorapi.h"

class SnippetApi : public LiteApi::ISnippetApi
{
public:
    SnippetApi(const QString &package);
    virtual ~SnippetApi();
    virtual QString package() const;
    virtual QStringList apiFiles() const;
    virtual bool loadApi();
    virtual QList<LiteApi::Snippet*> snippetList() const;

    void appendApiFile(const QString &file);
    void setApiFiles(const QStringList &files);
    bool isEmpty() const;
protected:
    QString m_package;
    QStringList m_apiFiles;
    QList<LiteApi::Snippet*> m_snippetList;
    bool m_bLoad;
};
#endif // LITESNIPPETAPI_H
