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
// Module: snippetmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "snippetmanager.h"
#include "snippet.h"
#include <QFileInfo>
#include <QDir>

SnippetsManager::SnippetsManager(QObject *parent) :
    LiteApi::ISnippetsManager(parent)
{
}

SnippetsManager::~SnippetsManager()
{
    qDeleteAll(m_snippetsList);
}

void SnippetsManager::addSnippetList(ISnippetList *snippets)
{
    m_snippetsList.append(snippets);
}

void SnippetsManager::removeSnippetList(ISnippetList *snippets)
{
    m_snippetsList.removeAll(snippets);
}

ISnippetList *SnippetsManager::findSnippetList(const QString &mimeType)
{
    foreach (ISnippetList *snippets, m_snippetsList) {
        if (snippets->mimeType() == mimeType) {
            return snippets;
        }
    }
    return 0;
}

QList<ISnippetList *> SnippetsManager::allSnippetList() const
{
    return m_snippetsList;
}

void SnippetsManager::load(const QString &path)
{
    QDir dir(path);
    foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs)) {
        QString mimeType = m_liteApp->mimeTypeManager()->findMimeTypeBySuffix(info.fileName());
        if (!mimeType.isEmpty()) {
            ISnippetList *find = this->findSnippetList(mimeType);
            if (!find) {
                find = new SnippetList(mimeType);
                this->addSnippetList(find);
            }
            ((SnippetList*)find)->appendPath(info.path());
        }
    }
}
