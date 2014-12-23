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
// Module: snippetmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SNIPPETMANAGER_H
#define SNIPPETMANAGER_H

#include "liteeditorapi/liteeditorapi.h"

using namespace LiteApi;

class SnippetsManager : public LiteApi::ISnippetsManager
{
public:
    SnippetsManager(QObject *parent);
    virtual ~SnippetsManager();
    virtual void addSnippetList(ISnippetList *snippets);
    virtual void removeSnippetList(ISnippetList *snippets);
    virtual ISnippetList *findSnippetList(const QString &mimeType);
    virtual QList<ISnippetList*> allSnippetList() const;
public:
    void load(const QString &path);
protected:
    QList<ISnippetList*> m_snippetsList;
};

#endif // SNIPPETMANAGER_H
