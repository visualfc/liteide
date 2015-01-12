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
// Module: commentapi.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "commentapi.h"
#include "qjson/include/QJson/Parser"
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


CommentApi::CommentApi(const QString &package)
    : m_package(package), m_bLoad(false)
{
}

QString CommentApi::package() const
{
    return m_package;
}

bool CommentApi::loadApi()
{
    if (m_bLoad) {
        return m_bLoad;
    }
    QFile f(m_apiFile);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray data = f.readAll();
    bool ok;
    QJson::Parser parser;
    QVariant json = parser.parse(data, &ok);
    if (ok) {
        QMapIterator<QString, QVariant> i(json.toMap());
        while(i.hasNext()) {
            i.next();
            QMap<QString,QVariant> m = i.value().toMap();
            LiteApi::Comment comment;
            comment.line = m.value("line").toString();
            comment.start = m.value("start").toString();
            comment.end = m.value("end").toString();
            if (!comment.line.isEmpty() ||
                    !comment.start.isEmpty() ||
                    !comment.end.isEmpty()) {
                m_commentMap.insert(i.key(),comment);
            }
        }
    }
    m_bLoad = true;
    return true;
}

LiteApi::Comment CommentApi::findComment(const QString &name) const
{
    return m_commentMap.value(name);
}

void CommentApi::setApiFile(const QString &file)
{
    m_apiFile = file;
}
