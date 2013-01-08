/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: makefilefile.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "makefilefile.h"
#include <QStandardItem>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include "fileutil/fileutil.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


MakefileFile::MakefileFile(LiteApi::IApplication *app,QObject *parent)
    : ModelFileImpl(app,parent)
{
}

bool MakefileFile::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return false;
    }
    QMap<QString,QStringList> context = FileUtil::readFileContext(&file);
    file.close();
    if (context.isEmpty()) {
        //return false;
    }
    m_context = context;
    if (m_context.contains("GOFILES") ||
            m_context.contains("CGOFILES")) {
        m_mimeType = "text/x-gomake";
    }
    return true;
}

void MakefileFile::updateModel()
{
    m_model->clear();
    m_fileNameList.clear();
    m_filePathList.clear();

    QStandardItem *item = new QStandardItem(QFileInfo(m_fileName).fileName());
    item->setData(ItemProFile);
    item->setIcon(QIcon("icon:images/projectitem.png"));
    m_model->appendRow(item);
    m_filePathList.append(m_fileName);

    QMap<QString,QString> fileMap;
    fileMap.insert("GOFILES",tr("GOFILES"));
    fileMap.insert("CGOFILES",tr("CGOFILES"));
    fileMap.insert("HEADERS",tr("Headers"));
    fileMap.insert("SOURCES",tr("Sources"));

    QMapIterator<QString,QString> i(fileMap);
    while(i.hasNext()) {
        i.next();
        QStringList files = this->values(i.key());
        if (!files.isEmpty()) {
            QStandardItem *folder = new QStandardItem(i.value());
            folder->setData(ItemFolder);
            folder->setIcon(QIcon("icon:images/folderitem.png"));
            m_model->appendRow(folder);
            foreach(QString file, files) {
                QStandardItem *fileItem = new QStandardItem(file);
                fileItem->setData(ItemFile);
                fileItem->setIcon(QIcon("icon:images/fileitem.png"));
                folder->appendRow(fileItem);
                m_fileNameList.append(file);
                m_filePathList.append(fileNameToFullPath(file));
            }
        }
    }
}

QString MakefileFile::target() const
{
    QString target = QFileInfo(m_fileName).fileName();
    QString val = value("TARG");
    if (!val.isEmpty()) {
        target = val;
    }
    return target;
}

QMap<QString,QString> MakefileFile::targetInfo() const
{
    QMap<QString,QString> m;
    if (m_fileName.isEmpty()) {
        return m;
    }
    QFileInfo info(m_fileName);
    QString target = info.fileName();
    QString val = this->value("TARG");
    if (!val.isEmpty()) {
        target = val;
    }
    m.insert("TARGETNAME",target);
    m.insert("TARGETPATH",QFileInfo(QDir(info.path()),target).filePath());
    m.insert("TARGETDIR",info.path());
    m.insert("WORKDIR",info.path());
    return m;
}


QString MakefileFile::targetPath() const
{
    QString target = QFileInfo(m_fileName).fileName();
    QString val = value("TARG");
    if (!val.isEmpty()) {
        target = val;
    }
    QString workPath = QFileInfo(m_fileName).absolutePath();
    if (!workPath.isEmpty()) {
        target = QFileInfo(QDir(workPath),target).absoluteFilePath();
    }
    return target;
}
