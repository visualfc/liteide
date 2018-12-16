/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: mimetype.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "mimetype.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QCoreApplication>
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


QString MimeType::package() const
{
    return m_package;
}

QString MimeType::type() const
{
    return m_type;
}

QString MimeType::scheme() const
{
    return m_scheme;
}

QString MimeType::comment() const
{
    return m_comment.join("/");
}

QString MimeType::codec() const
{
    return m_codec;
}

bool MimeType::tabToSpace() const
{
    return m_tabToSpace;
}

int MimeType::tabWidth() const
{
    return m_tabWidth;
}

QStringList MimeType::globPatterns() const
{
    return m_globPatterns;
}
QStringList MimeType::subClassesOf() const
{
    return m_subClassesOf;
}

void MimeType::merge(const IMimeType *mimeType)
{
    if (m_type != mimeType->type()) {
        return;
    }
    m_subClassesOf.append(mimeType->subClassesOf());
    m_globPatterns.append(mimeType->globPatterns());    
    m_customPatterns.append(mimeType->customPatterns());
    m_comment.append(mimeType->comment());

    if (!mimeType->codec().isEmpty()) {
        m_codec = mimeType->codec();
    }
    if (!mimeType->scheme().isEmpty()) {
        m_scheme = mimeType->scheme();
    }
    if (!mimeType->package().isEmpty()) {
        m_package = mimeType->package();
    }
    if (!mimeType->tabToSpace()) {
        m_tabToSpace = mimeType->tabToSpace();
    }
    if (mimeType->tabWidth() != -1) {
        m_tabWidth = mimeType->tabWidth();
    }
    m_subClassesOf.removeDuplicates();
    m_globPatterns.removeDuplicates();
    m_customPatterns.removeDuplicates();
    m_comment.removeDuplicates();
}

void MimeType::setCustomPatterns(const QStringList &custom)
{
    m_customPatterns = custom;
}

QStringList MimeType::customPatterns() const
{
    return m_customPatterns;
}

QStringList MimeType::allPatterns() const
{
    QStringList all;
    all << m_globPatterns;
    all << m_customPatterns;
    all.removeDuplicates();
    return all;
}

void MimeType::setPackage(const QString &package)
{
    m_package = package;
}

void MimeType::setType(const QString &type)
{
    m_type = type;
}

void MimeType::setScheme(const QString &scheme)
{
    m_scheme = scheme;
}

void MimeType::setCodec(const QString &codec)
{
    m_codec = codec;
}

void MimeType::setTabToSpace(const QString &s)
{
    if (s == "false" || s == "0") {
        m_tabToSpace = false;
    }
}

void MimeType::setTabWidth(const QString &s)
{
    bool ok = false;
    int n = s.toInt(&ok);
    if (ok) {
        m_tabWidth = n;
    }
}

void MimeType::setComment(const QString &comment)
{
    m_comment.append(comment);
}

void MimeType::appendLocalComment(const QString &local, const QString &comment)
{
    if (!local.isEmpty() && !comment.isEmpty()) {
        m_localCommentMap.insert(local,comment);
    }
}

void MimeType::appendGlobPatterns(const QString &globPattern)
{
    if (globPattern.isEmpty()) {
        return;
    }
    m_globPatterns.append(globPattern);
}

void MimeType::appendSubClassesOf(const QString &subClassOf)
{
    if (subClassOf.isEmpty()) {
        return;
    }
    m_subClassesOf.append(subClassOf);
}

bool MimeType::isEmpty() const
{
    return m_type.isEmpty();
}

bool MimeType::loadMimeTypes(LiteApi::IMimeTypeManager *manager, const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return false;
    }

    return MimeType::loadMimeTypes(manager,&file,fileName);
}

bool MimeType::loadMimeTypes(LiteApi::IMimeTypeManager *manager, QIODevice *dev, const QString &/*fileName*/)
{
    QXmlStreamReader reader(dev);
    QXmlStreamAttributes attrs;
    MimeType *mimeType = 0;
    while (!reader.atEnd()) {
        switch (reader.readNext()) {
        case QXmlStreamReader::StartElement:
            attrs = reader.attributes();
            if (reader.name() == "mime-type" && mimeType == 0) {
                mimeType = new MimeType;
                mimeType->setType(attrs.value("type").toString());
                mimeType->setPackage(attrs.value("package").toString());
                mimeType->setCodec(attrs.value("codec").toString());
                mimeType->setScheme(attrs.value("scheme").toString());
                mimeType->setTabToSpace(attrs.value("tabtospace").toString());
                mimeType->setTabWidth(attrs.value("tabwidth").toString());
            } else if (reader.name() == "sub-class-of" && mimeType) {
                mimeType->appendSubClassesOf(attrs.value("type").toString());
            } else if (reader.name() == "comment" && mimeType) {
                QString locale = attrs.value("xml:lang").toString();
                const QString comment = QCoreApplication::translate("MimeType", reader.readElementText().toLatin1());
                if (locale.isEmpty()) {
                    mimeType->setComment(comment);
                } else {
                    mimeType->appendLocalComment(locale,comment);
                }
            } else if (reader.name() == "glob" && mimeType) {
                mimeType->appendGlobPatterns(attrs.value("pattern").toString());
            }
            break;
        case QXmlStreamReader::EndElement:
            if (reader.name() == "mime-type") {
                bool b = false;
                if (mimeType && !mimeType->isEmpty()) {
                    if (manager->addMimeType(mimeType)) {
                        b = true;
                    }
                }
                if (!b) {
                    delete mimeType;
                }
                mimeType = 0;
            }
            break;
        default:
            break;
        }
    }
    return true;
}
