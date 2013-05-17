/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "itemdata.h"
#include "reuse.h"

using namespace TextEditor;
using namespace Internal;

ItemData::ItemData() :
    m_italic(false),
    m_italicSpecified(false),
    m_bold(false),
    m_boldSpecified(false),
    m_underlined(false),
    m_underlinedSpecified(false),
    m_strikedOut(false),
    m_strikeOutSpecified(false),
    m_spellChecking(true),
    m_spellCheckingSpecified(false),
    m_isCustomized(false)
{}

void ItemData::setStyle(const QString &style)
{ m_style = style; }

const QString &ItemData::style() const
{ return m_style; }

void ItemData::setColor(const QString &color)
{
    if (!color.isEmpty()) {
        m_color.setNamedColor(color);
        m_isCustomized = true;
    }
}

const QColor &ItemData::color() const
{ return m_color; }

void ItemData::setSelectionColor(const QString &color)
{
    if (!color.isEmpty()) {
        m_selectionColor.setNamedColor(color);
        m_isCustomized = true;
    }
}

const QColor &ItemData::selectionColor() const
{ return m_selectionColor; }

void ItemData::setItalic(const QString &italic)
{
    if (!italic.isEmpty()) {
        m_italic = toBool(italic);
        m_italicSpecified = true;
        m_isCustomized = true;
    }
}

bool ItemData::isItalic() const
{ return m_italic; }

bool ItemData::isItalicSpecified() const
{ return m_italicSpecified; }

void ItemData::setBold(const QString &bold)
{
    if (!bold.isEmpty()) {
        m_bold = toBool(bold);
        m_boldSpecified = true;
        m_isCustomized = true;
    }
}

bool ItemData::isBold() const
{ return m_bold; }

bool ItemData::isBoldSpecified() const
{ return m_boldSpecified; }

void ItemData::setUnderlined(const QString &underlined)
{
    if (!underlined.isEmpty()) {
        m_underlined = toBool(underlined);
        m_underlinedSpecified = true;
        m_isCustomized = true;
    }
}

bool ItemData::isUnderlined() const
{ return m_underlined; }

bool ItemData::isUnderlinedSpecified() const
{ return m_underlinedSpecified; }

void ItemData::setStrikeOut(const QString &strike)
{
    if (!strike.isEmpty()) {
        m_strikedOut = toBool(strike);
        m_strikeOutSpecified = true;
        m_isCustomized = true;
    }
}

bool ItemData::isStrikeOut() const
{ return m_strikedOut; }

bool ItemData::isStrikeOutSpecified() const
{ return m_strikeOutSpecified; }

void ItemData::setSpellChecking(const QString &checking)
{
    if (!checking.isEmpty()) {
        m_spellChecking = toBool(checking);
        m_spellCheckingSpecified = true;
        m_isCustomized = true;
    }
}

bool ItemData::isSpellChecking() const
{ return m_spellChecking; }

bool ItemData::isSpellCheckingSpecified() const
{ return m_spellCheckingSpecified; }

bool ItemData::isCustomized() const
{ return m_isCustomized; }
