/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: wordapi.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "wordapi.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QFileInfo>
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


WordApi::WordApi(const QString &package)
    : m_package(package), m_bLoad(false)
{

}

QString WordApi::package() const
{
    return m_package;
}

QStringList WordApi::apiFiles() const
{
    return m_apiFiles;
}

QStringList WordApi::wordList() const
{
    return m_wordList;
}

QStringList WordApi::expList() const
{
    return m_expList;
}

void WordApi::appendExp(const QStringList &list)
{
    m_expList.append(list);
}

bool WordApi::loadApi()
{
    if (m_bLoad) {
        return true;
    }
    m_bLoad = true;
    m_wordList.clear();
    foreach (QString file, m_apiFiles) {
        QFile f(file);
        if (!f.open(QIODevice::ReadOnly)) {
            continue;
        }
        while (!f.atEnd()) {
            QString line = f.readLine().trimmed();
            if (!line.isEmpty()) {
                m_wordList.append(line);
            }
        }
    }
    return !m_wordList.isEmpty();
}

void WordApi::appendApiFile(const QString &file)
{
    m_apiFiles.append(file);
}

void WordApi::setApiFiles(const QStringList &files)
{
    m_apiFiles = files;
}

bool WordApi::isEmpty() const
{
    return m_apiFiles.isEmpty();
}
