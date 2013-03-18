/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: snippet.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SNIPPET_H
#define SNIPPET_H

#include "liteeditorapi/liteeditorapi.h"

class Snippet : public LiteApi::ISnippet
{
public:
    QString trigger() const;
    void setTrigger(const QString &trigger);
    QString content() const;
    void setContent(const QString &content);
protected:
    QString m_trigger;
    QString m_content;
};

class SnippetList : public LiteApi::ISnippetList
{
public:
    SnippetList(const QString &mimeType);
    virtual ~SnippetList();
    virtual QString mimeType() const;    
    virtual bool load();
    virtual QList<LiteApi::ISnippet*> findSnippet(const QString &trigger, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;
    virtual QList<LiteApi::ISnippet*> snippetList() const;
    void appendPath(const QString &path);
protected:
    bool    m_bLoad;
    QString m_mimeType;
    QStringList m_pathList;
    QList<LiteApi::ISnippet*> m_snippetList;
};

#endif // SNIPPET_H
