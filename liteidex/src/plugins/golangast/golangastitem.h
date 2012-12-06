/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// date: 2011-8-3
// $Id: golangastitem.h,v 1.0 2011-8-3 visualfc Exp $

#ifndef GOLANGASTITEM_H
#define GOLANGASTITEM_H

#include <QStandardItem>

class GolangAstItem : public QStandardItem
{
public:
    void setTagName(const QString &tagName)
    {
        m_tagName = tagName;
    }
    QString tagName() const
    {
        return m_tagName;
    }
    void setFileName(const QString &fileName)
    {
        m_fileName = fileName;
    }
    QString fileName() const
    {
        return m_fileName;
    }
    void setLine(int line)
    {
        m_line = line;
    }
    void setCol(int col)
    {
        m_col = col;
    }
    int line() const
    {
        return m_line;
    }
    int col() const
    {
        return m_col;
    }
protected:
    QString m_tagName;
    QString m_fileName;
    int     m_line;
    int     m_col;
};


#endif // GOLANGASTITEM_H
