/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: colorstyle.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "colorstyle.h"
#include <QFile>
#include <QXmlStreamReader>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

ColorStyleScheme::ColorStyleScheme(QObject *parent)
    : QObject(parent)
{
}

ColorStyleScheme::~ColorStyleScheme()
{
    clear();
}

void ColorStyleScheme::clear()
{
    qDeleteAll(m_nameStyleMap);
    m_nameStyleMap.clear();
    m_name.clear();
}

const ColorStyle *ColorStyleScheme::findStyle(const QString &name) const
{
    return m_nameStyleMap.value(name,0);
}

bool ColorStyleScheme::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return false;
    }
    clear();
    return load(&file,fileName);
}

bool ColorStyleScheme::load(QIODevice *dev, const QString &/*fileName*/)
{
    QXmlStreamReader reader(dev);
    QXmlStreamAttributes attrs;
    ColorStyle *style = 0;
    bool bread = false;
    while (!reader.atEnd()) {
        switch (reader.readNext()) {
        case QXmlStreamReader::StartElement:
            attrs = reader.attributes();
            if (reader.name() == "style-scheme") {
                m_name = attrs.value("name").toString();
                if (!m_name.isEmpty()) {
                    bread = true;
                }
            } else if (reader.name() == "style" && style == 0 && bread) {
                QString tmp = attrs.value("name").toString();
                if (tmp.isEmpty()) {
                    break;
                }
                style = new ColorStyle;
                style->setName(tmp);
                tmp = attrs.value("foreground").toString();
                if (!tmp.isEmpty()) {
                    style->setForegound(tmp);
                }
                tmp = attrs.value("background").toString();
                if (!tmp.isEmpty()) {
                   style->setBackground(tmp);
                }
                tmp = attrs.value("bold").toString().toLower();
                if (tmp == "true") {
                    style->setBold(true);
                }
                tmp = attrs.value("italic").toString().toLower();
                if (tmp == "true") {
                    style->setItalic(true);
                }
            }
            break;
        case QXmlStreamReader::EndElement:
            if (reader.name() == "style") {
                if (style) {
                    if (!style->name().isEmpty()) {
                        m_nameStyleMap.insert(style->name(),style);
                    } else {
                        delete style;
                    }
                }
                style = 0;
            }
            break;
        default:
            break;
        }
    }
    return bread;
}
