/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2018 LiteIDE. All rights reserved.
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
// Module: findfilesthread.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "findfilesthread.h"
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

FindFilesThread::FindFilesThread(QObject *parent) : QThread(parent)
{
    m_cancel = false;
    m_maxFileCount = 10000;
    m_filesCount = 0;
    m_maxBlockSendCount = 10;
}

void FindFilesThread::setFolderList(const QStringList &folderList, const QSet<QString> &extSet, const QSet<QString> &exceptFiles, int maxCount)
{
    m_folderList = folderList;
    m_extSet = extSet;
    m_exceptFiles = exceptFiles;
    m_maxFileCount = maxCount;
    m_filesCount = 0;
    m_processFolderSet.clear();
    m_cancel = false;
}

void FindFilesThread::stop(int time)
{
    m_cancel = true;
    if (this->isRunning()) {
        if (!this->wait(time))
            this->terminate();
    }
}

void FindFilesThread::run()
{
    m_cancel = false;
    foreach (QString folder, m_folderList) {
        findFolder(folder);
    }
}

void FindFilesThread::sendResult(const QStringList &fileList)
{
    emit findResult(fileList);
}

void FindFilesThread::findFolder(QString folder)
{
    if (m_cancel) {
        return;
    }
    if (m_processFolderSet.contains(folder)) {
        return;
    }
    m_processFolderSet.insert(folder);
    QDir dir(folder);
    QList<QString> fileList;
    foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
        if (m_cancel) {
            return;
        }
        QString filePath = info.filePath();
        if (info.isDir()) {
            findFolder(filePath);
        } else if (info.isFile()) {
            if (m_extSet.contains(info.suffix()) && !m_exceptFiles.contains(filePath) ) {
                m_filesCount++;
                if (m_filesCount > m_maxFileCount) {
                    return;
                }
                fileList << filePath;
            }
        }
        if (fileList.size() >= m_maxBlockSendCount) {
            sendResult(fileList);
            fileList.clear();
        }
    }
    if (!fileList.isEmpty()) {
        sendResult(fileList);
    }
}
