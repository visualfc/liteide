/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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


WordApi::WordApi()
    : m_bLoad(false)
{
}


QString WordApi::mimeType() const
{
    return m_mimeType;
}

QStringList WordApi::apiFiles() const
{
    return m_globApiFiles;
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
    foreach (QString file, m_globApiFiles) {
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

void WordApi::setType(const QString &type)
{
    m_mimeType = type;
}

void WordApi::appendApiFiles(const QString &globApiFile)
{
    if (globApiFile.isEmpty()) {
        return;
    }
    m_globApiFiles.append(globApiFile);
}

bool WordApi::isEmpty() const
{
    return m_mimeType.isEmpty() || m_globApiFiles.isEmpty();
}

bool WordApi::loadWordApi(LiteApi::IWordApiManager *manager, const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return false;
    }
    return WordApi::loadWordApi2(manager,&file,fileName);
}

bool WordApi::loadWordApi2(LiteApi::IWordApiManager *manager, QIODevice *dev, const QString &fileName)
{
    QXmlStreamReader reader(dev);
    QXmlStreamAttributes attrs;
    WordApi *wordApi = 0;
    while (!reader.atEnd()) {
        switch (reader.readNext()) {
        case QXmlStreamReader::StartElement:
            attrs = reader.attributes();
            if (reader.name() == "mime-type" && wordApi == 0) {
                wordApi = new WordApi;
                wordApi->setType(attrs.value("type").toString());
            } else if (reader.name() == "glob" && wordApi) {
                QString apiFile = attrs.value("apifile").toString();
                wordApi->appendApiFiles(QFileInfo(QFileInfo(fileName).absoluteDir(),apiFile).canonicalFilePath());
            }
            break;
        case QXmlStreamReader::EndElement:
            if (reader.name() == "mime-type") {
                if (wordApi && !wordApi->isEmpty()) {
                    manager->addWordApi(wordApi);
                }
                wordApi = 0;
            }
            break;
        default:
            break;
        }
    }
    return true;
}
