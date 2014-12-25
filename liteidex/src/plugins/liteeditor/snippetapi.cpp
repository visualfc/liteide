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
// Module: snippet.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "snippetapi.h"
#include <QFile>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


SnippetApi::SnippetApi(const QString &package)
    : m_package(package), m_bLoad(false)
{

}

QString SnippetApi::package() const
{
    return m_package;
}

QStringList SnippetApi::apiFiles() const
{
    return m_apiFiles;
}

QStringList SnippetApi::snippetList() const
{
    return m_snippetList;
}

bool SnippetApi::loadApi()
{
    if (m_bLoad) {
        return true;
    }
    m_bLoad = true;
    m_snippetList.clear();
    foreach (QString file, m_apiFiles) {
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly)) {
            continue;
        }
        while (!f.atEnd()) {
            QString line = f.readLine().trimmed();
            if (!line.isEmpty()) {
                m_snippetList.append(line);
            }
        }
    }
    return !m_snippetList.isEmpty();
}

void SnippetApi::appendApiFile(const QString &file)
{
    m_apiFiles.append(file);
}

void SnippetApi::setApiFiles(const QStringList &files)
{
    m_apiFiles = files;
}

bool SnippetApi::isEmpty() const
{
    return m_apiFiles.isEmpty();
}
