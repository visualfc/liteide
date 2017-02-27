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
// Module: golangastitem.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGASTITEM_H
#define GOLANGASTITEM_H

#include <QStandardItem>
#include "golangastapi/golangastapi.h"

struct AstItemPos {
    QString fileName;
    int     line;
    int     column;
    int     endLine;
    int     endColumn;
};

class GolangAstItem : public QStandardItem
{
public:
    QList<AstItemPos>   m_posList;
    QString             m_tagName;
    QString             m_tipInfo;
    LiteApi::ASTTAG_ENUM m_tagFlag;
public:
    bool hasChildItem() const {
        switch (m_tagFlag) {
        case LiteApi::TagPackage:
        case LiteApi::TagConstFolder:
        case LiteApi::TagValueFolder:
        case LiteApi::TagImportFolder:
        case LiteApi::TagFuncFolder:
        case LiteApi::TagStruct:
        case LiteApi::TagInterface:
        case LiteApi::TagType:
            return true;
        default:
            return false;
        }
        return false;
    }
    bool isFolder() const {
        switch (m_tagFlag) {
        case LiteApi::TagConstFolder:
        case LiteApi::TagValueFolder:
        case LiteApi::TagImportFolder:
        case LiteApi::TagFuncFolder:
            return true;
        default:
            return false;
        }
        return false;
    }
};

#endif // GOLANGASTITEM_H
