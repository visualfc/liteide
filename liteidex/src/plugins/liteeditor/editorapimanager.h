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
// Module: editorapimanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef EDITORAPIMANAGER_H
#define EDITORAPIMANAGER_H

#include "liteeditorapi/liteeditorapi.h"

using namespace LiteApi;

class EditorApiManager : public IEditorApiManager
{
public:
    EditorApiManager(QObject *parent = 0);
    ~EditorApiManager();
    virtual void addWordApi(IWordApi *api);
    virtual void removeWordApi(IWordApi *api);
    virtual IWordApi *findWordApi(const QString &mimeType);
    virtual QList<IWordApi*> wordApiList() const;
    virtual void addSnippetApi(ISnippetApi *api);
    virtual void removeSnippetApi(ISnippetApi *api);
    virtual ISnippetApi *findSnippetApi(const QString &mimeType);
    virtual QList<ISnippetApi*> snippetApiList() const;
public:
    void load(const QString &path);
protected:
    QList<IWordApi*>    m_wordApiList;
    QList<ISnippetApi*> m_snippetApiList;
};

#endif // EDITORAPIMANAGER_H
