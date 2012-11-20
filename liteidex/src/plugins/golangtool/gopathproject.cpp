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
// Module: gopathproject.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-2-27
// $Id: gopathproject.cpp,v 1.0 2012-2-28 visualfc Exp $

#include "gopathproject.h"
#include "gopathmodel.h"
#include "liteapi/litefindobj.h"
#include "gopathbrowser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
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

GopathProject::GopathProject(GopathBrowser *browser) : m_browser(browser)
{
    connect(m_browser,SIGNAL(startPathChanged(QString)),this,SLOT(startPathChanged(QString)));
    m_path = m_browser->startPath();
}

GopathProject::~GopathProject()
{
    disconnect(m_browser);
    m_browser->widget()->hide();
}

GopathBrowser *GopathProject::browser()
{
    return m_browser;
}

void GopathProject::startPathChanged(const QString &path)
{
    m_path = path;
    emit reloaded();
}

QWidget *GopathProject::widget()
{
    return m_browser->widget();
}

QString GopathProject::name() const
{
    return m_path;
}


QString GopathProject::filePath() const
{
    return m_path;
}

QString GopathProject::mimeType() const
{
    return "text/x-gopath";
}

QStringList GopathProject::fileNameList() const
{
    QDir dir(m_path);
    QStringList list;
    foreach(QFileInfo info,dir.entryInfoList(QDir::Files)) {
        list.append(info.fileName());
    }
    return list;
}

QStringList GopathProject::filePathList() const
{
    QDir dir(m_path);
    QStringList list;
    foreach(QFileInfo info, dir.entryInfoList(QStringList() <<"*.go",QDir::Files)) {
        list.append(info.filePath());
    }
    return list;
}

QString GopathProject::fileNameToFullPath(const QString &filePath)
{
    return QFileInfo(m_path,filePath).filePath();
}

QMap<QString,QString> GopathProject::projectInfo() const
{
    QMap<QString,QString> m;
    if (m_path.isEmpty()) {
        return m;
    }
    QFileInfo info(m_path);
    m.insert("PROJECTNAME",info.fileName());
    m.insert("PROJECTPATH",info.filePath());
    m.insert("PROJECTDIR",info.filePath());
    return m;
}

QMap<QString,QString> GopathProject::targetInfo() const
{
    QMap<QString,QString> m;
    if (m_path.isEmpty()) {
        return m;
    }
    QFileInfo info(m_path);
    QString target = info.fileName();
#ifdef Q_OS_WIN
    target += ".exe";
#endif
    m.insert("TARGETNAME",target);
    m.insert("TARGETPATH",QFileInfo(QDir(m_path),target).filePath());
    m.insert("TARGETDIR",info.filePath());
    m.insert("WORKDIR",info.filePath());
    return m;
}


void GopathProject::load()
{
    m_browser->widget()->show();
}
