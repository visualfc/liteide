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

#include "colorscheme.h"

#include "texteditorconstants.h"

#include <QtCore/QFile>
#include <QtCore/QCoreApplication>
#include <QtXml/QXmlStreamWriter>

using namespace TextEditor;

static const char *trueString = "true";

// Format

Format::Format() :
    m_foreground(Qt::black),
    m_background(Qt::white),
    m_bold(false),
    m_italic(false)
{
}

void Format::setForeground(const QColor &foreground)
{
    m_foreground = foreground;
}

void Format::setBackground(const QColor &background)
{
    m_background = background;
}

void Format::setBold(bool bold)
{
    m_bold = bold;
}

void Format::setItalic(bool italic)
{
    m_italic = italic;
}

static QColor stringToColor(const QString &string)
{
    if (string == QLatin1String("invalid"))
        return QColor();
    return QColor(string);
}

bool Format::equals(const Format &f) const
{
    return m_foreground ==  f.m_foreground && m_background == f.m_background &&
           m_bold == f.m_bold && m_italic == f.m_italic;
}

bool Format::fromString(const QString &str)
{
    *this = Format();

    const QStringList lst = str.split(QLatin1Char(';'));
    if (lst.count() != 4)
        return false;

    m_foreground = stringToColor(lst.at(0));
    m_background = stringToColor(lst.at(1));
    m_bold = lst.at(2) == QLatin1String(trueString);
    m_italic = lst.at(3) == QLatin1String(trueString);
    return true;
}


// ColorScheme

ColorScheme::ColorScheme()
{
}

bool ColorScheme::contains(const QString &category) const
{
    return m_formats.contains(category);
}

Format &ColorScheme::formatFor(const QString &category)
{
    return m_formats[category];
}

Format ColorScheme::formatFor(const QString &category) const
{
    return m_formats.value(category);
}

void ColorScheme::setFormatFor(const QString &category, const Format &format)
{
    m_formats[category] = format;
}

void ColorScheme::clear()
{
    m_formats.clear();
}

bool ColorScheme::save(const QString &fileName) const
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QXmlStreamWriter w(&file);
    w.setAutoFormatting(true);
    w.setAutoFormattingIndent(2);

    w.writeStartDocument();
    w.writeStartElement(QLatin1String("style-scheme"));
    w.writeAttribute(QLatin1String("version"), QLatin1String("1.0"));
    if (!m_displayName.isEmpty())
        w.writeAttribute(QLatin1String("name"), m_displayName);

    QMapIterator<QString, Format> i(m_formats);
    while (i.hasNext()) {
        const Format &format = i.next().value();
        w.writeStartElement(QLatin1String("style"));
        w.writeAttribute(QLatin1String("name"), i.key());
        if (format.foreground().isValid())
            w.writeAttribute(QLatin1String("foreground"), format.foreground().name().toLower());
        if (format.background().isValid())
            w.writeAttribute(QLatin1String("background"), format.background().name().toLower());
        if (format.bold())
            w.writeAttribute(QLatin1String("bold"), QLatin1String(trueString));
        if (format.italic())
            w.writeAttribute(QLatin1String("italic"), QLatin1String(trueString));
        w.writeEndElement();
    }

    w.writeEndElement();
    w.writeEndDocument();

    return true;
}

namespace {

class ColorSchemeReader : public QXmlStreamReader
{
public:
    ColorSchemeReader() :
        m_scheme(0)
    {}

    bool read(const QString &fileName, ColorScheme *scheme);
    QString readName(const QString &fileName);

private:
    bool readNextStartElement();
    void skipCurrentElement();
    void readStyleScheme();
    void readStyle();

    ColorScheme *m_scheme;
    QString m_name;
};

bool ColorSchemeReader::read(const QString &fileName, ColorScheme *scheme)
{
    m_scheme = scheme;

    if (m_scheme)
        m_scheme->clear();

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return false;

    setDevice(&file);

    if (readNextStartElement() && name() == QLatin1String("style-scheme"))
        readStyleScheme();
    else
        raiseError(QCoreApplication::translate("TextEditor::Internal::ColorScheme", "Not a color scheme file."));

    return true;
}

QString ColorSchemeReader::readName(const QString &fileName)
{
    read(fileName, 0);
    return m_name;
}

bool ColorSchemeReader::readNextStartElement()
{
    while (readNext() != Invalid) {
        if (isStartElement())
            return true;
        else if (isEndElement())
            return false;
    }
    return false;
}

void ColorSchemeReader::skipCurrentElement()
{
    while (readNextStartElement())
        skipCurrentElement();
}

void ColorSchemeReader::readStyleScheme()
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("style-scheme"));

    const QXmlStreamAttributes attr = attributes();
    m_name = attr.value(QLatin1String("name")).toString();
    if (!m_scheme)
        // We're done
        raiseError(QLatin1String("name loaded"));
    else
        m_scheme->setDisplayName(m_name);

    while (readNextStartElement()) {
        if (name() == QLatin1String("style"))
            readStyle();
        else
            skipCurrentElement();
    }
}

void ColorSchemeReader::readStyle()
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("style"));

    const QXmlStreamAttributes attr = attributes();
    QString name = attr.value(QLatin1String("name")).toString();
    QString foreground = attr.value(QLatin1String("foreground")).toString();
    QString background = attr.value(QLatin1String("background")).toString();
    bool bold = attr.value(QLatin1String("bold")) == QLatin1String(trueString);
    bool italic = attr.value(QLatin1String("italic")) == QLatin1String(trueString);

    Format format;

    if (QColor::isValidColor(foreground))
        format.setForeground(QColor(foreground));
    else
        format.setForeground(QColor());

    if (QColor::isValidColor(background))
        format.setBackground(QColor(background));
    else
        format.setBackground(QColor());

    format.setBold(bold);
    format.setItalic(italic);

    m_scheme->setFormatFor(name, format);

    skipCurrentElement();
}

} // anonymous namespace


bool ColorScheme::load(const QString &fileName)
{
    ColorSchemeReader reader;
    return reader.read(fileName, this) && !reader.hasError();
}

QString ColorScheme::readNameOfScheme(const QString &fileName)
{
    return ColorSchemeReader().readName(fileName);
}
