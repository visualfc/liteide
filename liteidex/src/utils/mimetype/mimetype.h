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
// Module: mimetype.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEAPI_MIMETYPE_H
#define LITEAPI_MIMETYPE_H

#include "liteapi/liteapi.h"

class MimeType : public LiteApi::IMimeType
{
public:
    virtual QString type() const;
    virtual QString scheme() const;
    virtual QString comment() const;
    virtual QString codec() const;
    virtual QStringList globPatterns() const;
    virtual QStringList subClassesOf() const;
    virtual void merge(const IMimeType *mimeType);

    void setType(const QString &type);
    void setScheme(const QString &scheme);
    void setCodec(const QString &codec);
    void setComment(const QString &comment);
    void appendGlobPatterns(const QString &globPattern);
    void appendSubClassesOf(const QString &subClassOf);
    void appendLocalComment(const QString &local, const QString &commnet);
    bool isEmpty() const;
public:
    static bool loadMimeTypes(LiteApi::IMimeTypeManager *manager, const QString &fileName);
    static bool loadMimeTypes(LiteApi::IMimeTypeManager *manager, QIODevice *dev, const QString &fileName);
protected:
    QString m_type;
    QString m_scheme;
    QString m_codec;
    QStringList m_comment;
    QStringList m_globPatterns;
    QStringList m_subClassesOf;
    QMap<QString,QString> m_localCommentMap;
};
#endif // LITEAPI_MIMETYPE_H
