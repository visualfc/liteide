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
// Module: colorstyle.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef COLORSTYLE_H
#define COLORSTYLE_H

#include <QObject>
#include <QColor>
#include <QMap>

class ColorStyle
{
public:
    ColorStyle() : m_bold(false),m_italic(false)
    {
    }
    void setName(const QString &name) {
        m_name = name;
    }
    QString name() const {
        return m_name;
    }
    void setForegound(const QColor &forgound) {
        m_foregound = forgound;
    }
    QColor foregound() const {
        return m_foregound;
    }
    void setBackground(const QColor &backgound) {
        m_background = backgound;
    }
    QColor background() const {
        return m_background;
    }
    void setBold(bool bold) {
        m_bold = bold;
    }
    bool bold() const {
        return m_bold;
    }
    void setItalic(bool italic) {
        m_italic = italic;
    }
    bool italic() const {
        return m_italic;
    }
protected:
    QString m_name;
    QColor  m_foregound;
    QColor  m_background;
    bool    m_bold;
    bool    m_italic;
};

class ColorStyleScheme : QObject
{
public:
    ColorStyleScheme(QObject *parent);
    virtual ~ColorStyleScheme();
public:
    QString name() const { return m_name; }
    const ColorStyle *findStyle(const QString &name) const;
    bool load(const QString &fileName);
    bool load(QIODevice *dev, const QString &fileName);
    void clear();
protected:
    QMap<QString,ColorStyle*> m_nameStyleMap;
    QString m_name;
};

#endif // COLORSTYLE_H
