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

#include "tabsettings.h"

#include <utils/settingsutils.h>

#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>

static const char *spacesForTabsKey = "SpacesForTabs";
static const char *autoSpacesForTabsKey = "AutoSpacesForTabs";
static const char *smartBackspaceKey = "SmartBackspace";
static const char *autoIndentKey = "AutoIndent";
static const char *tabSizeKey = "TabSize";
static const char *indentSizeKey = "IndentSize";
static const char *indentBracesKey = "IndentBraces";
static const char *doubleIndentBlocksKey = "DoubleIndentBlocks";
static const char *tabKeyBehaviorKey = "TabKeyBehavior";
static const char *groupPostfix = "TabSettings";
static const char *paddingModeKey = "PaddingMode";

namespace TextEditor {

TabSettings::TabSettings() :
    m_spacesForTabs(true),
    m_autoSpacesForTabs(false),
    m_autoIndent(true),
    m_smartBackspace(false),
    m_tabSize(8),
    m_indentSize(4),
    m_indentBraces(false),
    m_doubleIndentBlocks(false),
    m_tabKeyBehavior(TabNeverIndents),
    m_continuationAlignBehavior(ContinuationAlignWithSpaces)
{
}

void TabSettings::toSettings(const QString &category, QSettings *s) const
{
    Utils::toSettings(QLatin1String(groupPostfix), category, s, this);
}

void TabSettings::fromSettings(const QString &category, const QSettings *s)
{
    *this = TabSettings(); // Assign defaults
    Utils::fromSettings(QLatin1String(groupPostfix), category, s, this);
}

void TabSettings::toMap(const QString &prefix, QVariantMap *map) const
{
    map->insert(prefix + QLatin1String(spacesForTabsKey), m_spacesForTabs);
    map->insert(prefix + QLatin1String(autoSpacesForTabsKey), m_autoSpacesForTabs);
    map->insert(prefix + QLatin1String(autoIndentKey), m_autoIndent);
    map->insert(prefix + QLatin1String(smartBackspaceKey), m_smartBackspace);
    map->insert(prefix + QLatin1String(tabSizeKey), m_tabSize);
    map->insert(prefix + QLatin1String(indentSizeKey), m_indentSize);
    map->insert(prefix + QLatin1String(indentBracesKey), m_indentBraces);
    map->insert(prefix + QLatin1String(doubleIndentBlocksKey), m_doubleIndentBlocks);
    map->insert(prefix + QLatin1String(tabKeyBehaviorKey), m_tabKeyBehavior);
    map->insert(prefix + QLatin1String(paddingModeKey), m_continuationAlignBehavior);
}

void TabSettings::fromMap(const QString &prefix, const QVariantMap &map)
{
    m_spacesForTabs =
        map.value(prefix + QLatin1String(spacesForTabsKey), m_spacesForTabs).toBool();
    m_autoSpacesForTabs =
        map.value(prefix + QLatin1String(autoSpacesForTabsKey), m_autoSpacesForTabs).toBool();
    m_autoIndent = map.value(prefix + QLatin1String(autoIndentKey), m_autoIndent).toBool();
    m_smartBackspace =
            map.value(prefix + QLatin1String(smartBackspaceKey), m_smartBackspace).toBool();
    m_tabSize = map.value(prefix + QLatin1String(tabSizeKey), m_tabSize).toInt();
    m_indentSize = map.value(prefix + QLatin1String(indentSizeKey), m_indentSize).toInt();
    m_indentBraces = map.value(prefix + QLatin1String(indentBracesKey), m_indentBraces).toBool();
    m_doubleIndentBlocks =
        map.value(prefix + QLatin1String(doubleIndentBlocksKey), m_doubleIndentBlocks).toBool();
    m_tabKeyBehavior = (TabKeyBehavior)
        map.value(prefix + QLatin1String(tabKeyBehaviorKey), m_tabKeyBehavior).toInt();
    m_continuationAlignBehavior = (ContinuationAlignBehavior)
        map.value(prefix + QLatin1String(paddingModeKey), m_continuationAlignBehavior).toInt();
}

bool TabSettings::cursorIsAtBeginningOfLine(const QTextCursor &cursor) const
{
    QString text = cursor.block().text();
    int fns = firstNonSpace(text);
    return (cursor.position() - cursor.block().position() <= fns);
}

int TabSettings::lineIndentPosition(const QString &text) const
{
    int i = 0;
    while (i < text.size()) {
        if (!text.at(i).isSpace())
            break;
        ++i;
    }
    int column = columnAt(text, i);
    return i - (column % m_indentSize);
}

int TabSettings::firstNonSpace(const QString &text) const
{
    int i = 0;
    while (i < text.size()) {
        if (!text.at(i).isSpace())
            return i;
        ++i;
    }
    return i;
}

QString TabSettings::indentationString(const QString &text) const
{
    return text.left(firstNonSpace(text));
}


int TabSettings::indentationColumn(const QString &text) const
{
    return columnAt(text, firstNonSpace(text));
}

int TabSettings::maximumPadding(const QString &text) const
{
    int fns = firstNonSpace(text);
    int i = fns;
    while (i > 0) {
        if (text.at(i-1) != QLatin1Char(' '))
            break;
        --i;
    }
    return fns - i;
}


int TabSettings::trailingWhitespaces(const QString &text) const
{
    int i = 0;
    while (i < text.size()) {
        if (!text.at(text.size()-1-i).isSpace())
            return i;
        ++i;
    }
    return i;
}

bool TabSettings::isIndentationClean(const QTextBlock &block) const
{
    int i = 0;
    int spaceCount = 0;
    QString text = block.text();
    bool spacesForTabs = guessSpacesForTabs(block);
    while (i < text.size()) {
        QChar c = text.at(i);
        if (!c.isSpace())
            return true;

        if (c == QLatin1Char(' ')) {
            ++spaceCount;
            if (!spacesForTabs && spaceCount == m_tabSize)
                return false;
        } else if (c == QLatin1Char('\t')) {
            if (spacesForTabs || spaceCount != m_indentSize)
                return false;
            spaceCount = 0;
        }
        ++i;
    }
    return true;
}

bool TabSettings::tabShouldIndent(const QTextDocument *document, QTextCursor cursor, int *suggestedPosition) const
{
    if (m_tabKeyBehavior == TabNeverIndents)
        return false;
    QTextCursor tc = cursor;
    if (suggestedPosition)
        *suggestedPosition = tc.position(); // At least suggest original position
    tc.movePosition(QTextCursor::StartOfLine);
    if (tc.atBlockEnd()) // cursor was on a blank line
        return true;
    if (document->characterAt(tc.position()).isSpace()) {
        tc.movePosition(QTextCursor::WordRight);
        if (tc.positionInBlock() >= cursor.positionInBlock()) {
            if (suggestedPosition)
                *suggestedPosition = tc.position(); // Suggest position after whitespace
            if (m_tabKeyBehavior == TabLeadingWhitespaceIndents)
                return true;
        }
    }
    return (m_tabKeyBehavior == TabAlwaysIndents);
}

int TabSettings::columnAt(const QString &text, int position) const
{
    int column = 0;
    for (int i = 0; i < position; ++i) {
        if (text.at(i) == QLatin1Char('\t'))
            column = column - (column % m_tabSize) + m_tabSize;
        else
            ++column;
    }
    return column;
}

int TabSettings::positionAtColumn(const QString &text, int column, int *offset) const
{
    int col = 0;
    int i = 0;
    while (i < text.size() && col < column) {
        if (text.at(i) == QLatin1Char('\t'))
            col = col - (col % m_tabSize) + m_tabSize;
        else
            ++col;
        ++i;
    }
    if (offset)
        *offset = column - col;
    return i;
}

int TabSettings::spacesLeftFromPosition(const QString &text, int position) const
{
    int i = position;
    while (i > 0) {
        if (!text.at(i-1).isSpace())
            break;
        --i;
    }
    return position - i;
}

int TabSettings::indentedColumn(int column, bool doIndent) const
{
    int aligned = (column / m_indentSize) * m_indentSize;
    if (doIndent)
        return aligned + m_indentSize;
    if (aligned < column)
        return aligned;
    return qMax(0, aligned - m_indentSize);
}

bool TabSettings::guessSpacesForTabs(const QTextBlock &_block) const
{
    if (m_spacesForTabs && m_autoSpacesForTabs && _block.isValid()) {
        const QTextDocument *doc = _block.document();
        QVector<QTextBlock> currentBlocks(2, _block); // [0] looks back; [1] looks forward
        int maxLookAround = 100;
        while (maxLookAround-- > 0) {
            if (currentBlocks.at(0).isValid())
                currentBlocks[0] = currentBlocks.at(0).previous();
            if (currentBlocks.at(1).isValid())
                currentBlocks[1] = currentBlocks.at(1).next();
            bool done = true;
            foreach (const QTextBlock &block, currentBlocks) {
                if (block.isValid())
                    done = false;
                if (!block.isValid() || block.length() == 0)
                    continue;
                const QChar firstChar = doc->characterAt(block.position());
                if (firstChar == QLatin1Char(' ')) {
                    return true;
                } else if (firstChar == QLatin1Char('\t')) {
                    return false;
                }
            }
            if (done)
                break;
        }
    }
    return m_spacesForTabs;
}

QString TabSettings::indentationString(int startColumn, int targetColumn, const QTextBlock &block) const
{
    targetColumn = qMax(startColumn, targetColumn);
    if (guessSpacesForTabs(block))
        return QString(targetColumn - startColumn, QLatin1Char(' '));

    QString s;
    int alignedStart = startColumn - (startColumn % m_tabSize) + m_tabSize;
    if (alignedStart > startColumn && alignedStart <= targetColumn) {
        s += QLatin1Char('\t');
        startColumn = alignedStart;
    }
    if (int columns = targetColumn - startColumn) {
        int tabs = columns / m_tabSize;
        s += QString(tabs, QLatin1Char('\t'));
        s += QString(columns - tabs * m_tabSize, QLatin1Char(' '));
    }
    return s;
}

void TabSettings::indentLine(QTextBlock block, int newIndent, int padding) const
{
    const QString text = block.text();
    const int oldBlockLength = text.size();

    if (m_continuationAlignBehavior == NoContinuationAlign) {
        newIndent -= padding;
        padding = 0;
    } else if (m_continuationAlignBehavior == ContinuationAlignWithIndent) {
        padding = 0;
    }

    // Quickly check whether indenting is required.
    if (indentationColumn(text) == newIndent)
        return;

    QString indentString;

    if (!m_spacesForTabs) {
        // user likes tabs for spaces and uses tabs for indentation, preserve padding
        indentString = indentationString(0, newIndent - padding, block);
        indentString += QString(padding, QLatin1Char(' '));
    } else {
        indentString = indentationString(0, newIndent, block);
    }

    if (oldBlockLength == indentString.length() && text == indentString)
        return;

    QTextCursor cursor(block);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, firstNonSpace(text));
    cursor.removeSelectedText();
    cursor.insertText(indentString);
    cursor.endEditBlock();
}

void TabSettings::reindentLine(QTextBlock block, int delta) const
{
    const QString text = block.text();
    const int oldBlockLength = text.size();

    int oldIndent = indentationColumn(text);
    int newIndent = qMax(oldIndent + delta, 0);

    if (oldIndent == newIndent)
        return;

    QString indentString;
    if (!m_spacesForTabs && m_tabSize == m_indentSize) {
        // user likes tabs for spaces and uses tabs for indentation, preserve padding
        int padding = qMin(maximumPadding(text), newIndent);
        indentString = indentationString(0, newIndent - padding, block);
        indentString += QString(padding, QLatin1Char(' '));
    } else {
        indentString = indentationString(0, newIndent, block);
    }

    if (oldBlockLength == indentString.length() && text == indentString)
        return;

    QTextCursor cursor(block);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, firstNonSpace(text));
    cursor.removeSelectedText();
    cursor.insertText(indentString);
    cursor.endEditBlock();
}

bool TabSettings::equals(const TabSettings &ts) const
{
    return m_spacesForTabs == ts.m_spacesForTabs
        && m_autoSpacesForTabs == ts.m_autoSpacesForTabs
        && m_autoIndent == ts.m_autoIndent
        && m_smartBackspace == ts.m_smartBackspace
        && m_tabSize == ts.m_tabSize
        && m_indentSize == ts.m_indentSize
        && m_indentBraces == ts.m_indentBraces
	&& m_doubleIndentBlocks == ts.m_doubleIndentBlocks
        && m_tabKeyBehavior == ts.m_tabKeyBehavior
        && m_continuationAlignBehavior == ts.m_continuationAlignBehavior;
}

} // namespace TextEditor
