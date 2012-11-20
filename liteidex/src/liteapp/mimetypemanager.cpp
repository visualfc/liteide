/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: mimetypemanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: mimetypemanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "mimetypemanager.h"
#include "mimetype/mimetype.h"
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


MimeTypeManager::~MimeTypeManager()
{
    qDeleteAll(m_mimeTypeList);
}

bool MimeTypeManager::addMimeType(IMimeType *mimeType)
{
    foreach (IMimeType *m, m_mimeTypeList) {
        if (m->type() == mimeType->type()) {
            m->merge(mimeType);
            return false;
        }
    }
    m_mimeTypeList.append(mimeType);
    return true;
}

void MimeTypeManager::removeMimeType(IMimeType *mimeType)
{
    m_mimeTypeList.removeOne(mimeType);
}

QList<IMimeType*> MimeTypeManager::mimeTypeList() const
{
    return m_mimeTypeList;
}

IMimeType *MimeTypeManager::findMimeType(const QString &type) const
{
    foreach(IMimeType *mimeType, m_mimeTypeList) {
        if (mimeType->type() == type) {
            return mimeType;
        }
    }
    return 0;
}

QString MimeTypeManager::findMimeTypeByFile(const QString &fileName) const
{
    QString find = QFileInfo(fileName).suffix();
    if (find.isEmpty()) {
        find = QFileInfo(fileName).fileName();
    } else {
        find = "*."+find;
    }
    foreach (IMimeType *mimeType, m_mimeTypeList) {
        foreach (QString pattern, mimeType->globPatterns()) {
            if (find.compare(pattern,Qt::CaseInsensitive) == 0) {
               return mimeType->type();
            }
        }
    }
    return  QString();
}

QString MimeTypeManager::findMimeTypeByScheme(const QString &scheme) const
{
    foreach (IMimeType *mimeType, m_mimeTypeList) {
        QString type = mimeType->scheme();
        if (type.isEmpty()) {
            type = "file";
        }
        if (scheme == type) {
            return mimeType->type();
        }
    }
    return QString();
}

void MimeTypeManager::loadMimeTypes(const QString &path)
{
    QDir dir = path;
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.xml"));
    foreach (QString fileName, dir.entryList()) {
        bool b = MimeType::loadMimeTypes(this,QFileInfo(dir,fileName).absoluteFilePath());
        m_liteApp->appendLog("LiteApp",QString("LoadMimeType %1 %2").arg(fileName).arg(b?"true":"fallse"));
    }
}

QStringList MimeTypeManager::findAllFilesByMimeType(const QString &dirPath, const QString &type, int deep) const
{
    LiteApi::IMimeType *mimeType = findMimeType(type);
    if (mimeType) {
        QDir dir(dirPath);
        for (int i = 0; i <= deep; i++) {
            QStringList files = dir.entryList(QStringList() << mimeType->globPatterns(),QDir::Files);
            if (!files.isEmpty()) {
                return files;
            }
            dir.cdUp();
        }
    }
    return QStringList();
}
