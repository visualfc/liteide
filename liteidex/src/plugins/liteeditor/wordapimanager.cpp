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
// Module: wordapimanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: wordapimanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "wordapimanager.h"
#include "wordapi/wordapi.h"

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


WordApiManager::WordApiManager(QObject *parent)
    : IWordApiManager(parent)
{
}

WordApiManager::~WordApiManager()
{
    qDeleteAll(m_wordApiList);
}

void WordApiManager::addWordApi(IWordApi *wordApi)
{
    m_wordApiList.append(wordApi);
}

void WordApiManager::removeWordApi(IWordApi *wordApi)
{
    m_wordApiList.removeAll(wordApi);
}

IWordApi *WordApiManager::findWordApi(const QString &mimeType)
{
    foreach (IWordApi *wordApi, m_wordApiList) {
        if (wordApi->mimeType() == mimeType) {
            return wordApi;
        }
    }
    return 0;
}

QList<IWordApi*> WordApiManager::wordApiList() const
{
    return m_wordApiList;
}

void WordApiManager::load(const QString &path)
{
    QDir dir = path;
    m_liteApp->appendLog("WordApiManager","load "+path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.xml"));
    foreach (QString fileName, dir.entryList()) {
        WordApi::loadWordApi(this,QFileInfo(dir,fileName).absoluteFilePath());
    }
}
