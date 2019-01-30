/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: editorapimanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "editorapimanager.h"
#include "wordapi.h"
#include "snippetapi.h"

#include <QDir>
#include <QFileInfo>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


EditorApiManager::EditorApiManager(QObject *parent)
    : IEditorApiManager(parent)
{
}

EditorApiManager::~EditorApiManager()
{
    qDeleteAll(m_wordApiList);
    qDeleteAll(m_snippetApiList);
}

void EditorApiManager::addWordApi(IWordApi *api)
{
    m_wordApiList.append(api);
}

void EditorApiManager::removeWordApi(IWordApi *api)
{
    m_wordApiList.removeAll(api);
}

IWordApi *EditorApiManager::findWordApi(const QString &mimeType)
{
    QString package = LiteApi::findPackageByMimeType(m_liteApp,mimeType);
    if (package.isEmpty()) {
        return 0;
    }
    foreach (IWordApi *wordApi, m_wordApiList) {
        if (wordApi->package() == package) {
            return wordApi;
        }
    }
    return 0;
}

QList<IWordApi*> EditorApiManager::wordApiList() const
{
    return m_wordApiList;
}

void EditorApiManager::addSnippetApi(ISnippetApi *api)
{
    m_snippetApiList.append(api);
}

void EditorApiManager::removeSnippetApi(ISnippetApi *api)
{
    m_snippetApiList.removeAll(api);
}

ISnippetApi *EditorApiManager::findSnippetApi(const QString &mimeType)
{
    QString package = LiteApi::findPackageByMimeType(m_liteApp,mimeType);
    if (package.isEmpty()) {
        return 0;
    }
    foreach (ISnippetApi *api, m_snippetApiList) {
        if (api->package() == package) {
            return api;
        }
    }
    return 0;

}

QList<ISnippetApi *> EditorApiManager::snippetApiList() const
{
    return m_snippetApiList;
}

void EditorApiManager::load(const QString &path)
{
    QDir dir = path;
    m_liteApp->appendLog("WordApiManager","Loading "+path);
    QStringList nameFilter;
    nameFilter << "*.api" << "*.snippet.json";
    foreach(QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot)) {
        QStringList wordFiles;
        QStringList snippetFiles;
        if (info.isDir()) {
            foreach (QFileInfo i, QDir(info.absoluteFilePath()).entryInfoList(nameFilter)) {
                if (i.fileName().endsWith(".api")) {
                    wordFiles.append(i.filePath());
                } else if (i.fileName().endsWith(".snippet.json")) {
                    snippetFiles.append(i.filePath());
                }
            }
        }
        if (!wordFiles.isEmpty()) {
            WordApi *api = new WordApi(info.fileName());
            api->setApiFiles(wordFiles);
            this->addWordApi(api);
            m_liteApp->appendLog("load word api",wordFiles.join(","));
        }
        if (!snippetFiles.isEmpty()) {
            SnippetApi *api = new SnippetApi(info.fileName());
            api->setApiFiles(snippetFiles);
            this->addSnippetApi(api);
            m_liteApp->appendLog("load snippet api",snippetFiles.join(","));
        }        
    }
}
