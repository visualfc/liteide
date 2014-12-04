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
// Module: filesystemmodelex.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filesystemmodelex.h"
#include <QDateTime>
#include <QLocale>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QString FileSystemModelEx::fileSize(qint64 bytes)
{
    // According to the Si standard KB is 1000 bytes, KiB is 1024
    // but on windows sizes are calculated by dividing by 1024 so we do what they do.
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    const qint64 tb = 1024 * gb;
    if (bytes >= tb)
        return QFileSystemModel::tr("%1 TB").arg(QLocale().toString(qreal(bytes) / tb, 'f', 3));
    if (bytes >= gb)
        return QFileSystemModel::tr("%1 GB").arg(QLocale().toString(qreal(bytes) / gb, 'f', 2));
    if (bytes >= mb)
        return QFileSystemModel::tr("%1 MB").arg(QLocale().toString(qreal(bytes) / mb, 'f', 1));
    if (bytes >= kb)
        return QFileSystemModel::tr("%1 KB").arg(QLocale().toString(bytes / kb));
    return QFileSystemModel::tr("%1 bytes").arg(QLocale().toString(bytes));
}

FileSystemModelEx::FileSystemModelEx(QObject *parent) :
    QFileSystemModel(parent)
{
}

int FileSystemModelEx::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}
QVariant FileSystemModelEx::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole) {
        if (this->isDir(index)) {
            return QDir::toNativeSeparators(this->filePath(index));
        } else {
            QFileInfo info = this->fileInfo(index);
            return QString("%1\n%2\n%3")
                    .arg(QDir::toNativeSeparators(info.filePath()))
                    .arg(fileSize(info.size()))
                    .arg(info.lastModified().toString(Qt::SystemLocaleDate));
        }
    }
    return QFileSystemModel::data(index,role);
}
