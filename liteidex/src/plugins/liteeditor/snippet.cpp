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
// Module: snippet.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "snippet.h"

QString Snippet::trigger() const
{
    return m_trigger;
}

void Snippet::setTrigger(const QString &trigger)
{
    m_trigger = trigger;
}

QString Snippet::content() const
{
    return m_content;
}

void Snippet::setContent(const QString &content)
{
    m_content = content;
}


SnippetList::SnippetList(const QString &mimeType) :
    m_bLoad(false), m_mimeType(mimeType)
{
}

SnippetList::~SnippetList()
{
    qDeleteAll(m_snippetList);
}

QString SnippetList::mimeType() const
{
    return m_mimeType;
}

bool SnippetList::load()
{
    if (m_bLoad) {
        return true;
    }
    m_bLoad = true;

    return true;
}

QList<LiteApi::ISnippet *> SnippetList::findSnippet(const QString &trigger, Qt::CaseSensitivity cs) const
{
    QList<LiteApi::ISnippet *> l;
    foreach(LiteApi::ISnippet *snippet, m_snippetList) {
        if (snippet->trigger().compare(trigger,cs) == 0){
            l.append(snippet);
        }
    }
    return l;
}

QList<LiteApi::ISnippet *> SnippetList::snippetList() const
{
    return m_snippetList;
}

void SnippetList::appendPath(const QString &path)
{
    m_pathList.append(path);
}
