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
// Module: commentapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef COMMENTAPI_H
#define COMMENTAPI_H

#include "liteeditorapi/liteeditorapi.h"

class CommentApi : public LiteApi::ICommentApi
{
public:
    CommentApi(const QString &package);
    virtual QString package() const;
    virtual bool loadApi();
    virtual LiteApi::Comment findComment(const QString &name) const;
    void setApiFile(const QString &file);
protected:
    QString m_package;
    bool    m_bLoad;
    QString m_apiFile;
    QMap<QString,LiteApi::Comment> m_commentMap;
};

#endif // COMMENTAPI_H
