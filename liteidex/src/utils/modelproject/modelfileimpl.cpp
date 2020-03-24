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
// Module: modelfileimpl.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "modelfileimpl.h"
#include "fileutil/fileutil.h"

#include <QStandardItemModel>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


ModelFileImpl::ModelFileImpl(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IFile(parent),
      m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_bReadOnly = false;
}

bool ModelFileImpl::loadText(const QString &fileName, const QString &mimeType, QString &/*outText*/)
{
    m_mimeType = mimeType;
    if (!loadFile(fileName)) {
        return false;
    }
    m_fileName = fileName;
    m_rootPath = QFileInfo(m_fileName).absolutePath();
    return true;
}

bool ModelFileImpl::reloadText(QString &outText)
{
    if (m_fileName.isEmpty()) {
        return false;
    }
    return loadText(m_fileName,m_mimeType,outText);
}

bool ModelFileImpl::isReadOnly() const
{
    return m_bReadOnly;
}

bool ModelFileImpl::isBinary() const
{
    return false;
}

bool ModelFileImpl::saveText(const QString &/*fileName*/, const QString &/*text*/)
{
    return false;
}

QString ModelFileImpl::filePath() const
{
    return m_fileName;
}

QString ModelFileImpl::mimeType() const
{
    return m_mimeType;
}

QStandardItemModel *ModelFileImpl::model() const
{
    return m_model;
}

QString ModelFileImpl::value(const QString &key) const
{
    QStringList val = m_context.value(key);
    if (!val.isEmpty()) {
        return val.at(0);
    }
    return QString();
}

QStringList ModelFileImpl::values(const QString &key) const
{
    return m_context.value(key);
}

QStringList ModelFileImpl::fileNameList() const
{
    return m_fileNameList;
}

QStringList ModelFileImpl::filePathList() const
{
    return m_filePathList;
}

QString ModelFileImpl::fileNameToFullPath(const QString &fileName)
{
    QString fullPath = m_rootPath;
    fullPath += QLatin1Char('/');
    fullPath += fileName;
    return fullPath;
}

QString ModelFileImpl::workPath() const
{
    return QFileInfo(m_fileName).absolutePath();
}
