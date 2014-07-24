/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "autocompleter.h"
#include "basetextdocumentlayout.h"
#include "tabsettings.h"

#include <QDebug>
#include <QTextCursor>

using namespace TextEditor;

AutoCompleter::AutoCompleter() :
    m_allowSkippingOfBlockEnd(false),
    m_surroundWithEnabled(true),
    m_autoParenthesesEnabled(true)
{}

AutoCompleter::~AutoCompleter()
{}

void AutoCompleter::setAutoParenthesesEnabled(bool b)
{
    m_autoParenthesesEnabled = b;
}

bool AutoCompleter::isAutoParenthesesEnabled() const
{
    return m_autoParenthesesEnabled;
}

void AutoCompleter::setSurroundWithEnabled(bool b)
{
    m_surroundWithEnabled = b;
}

bool AutoCompleter::isSurroundWithEnabled() const
{
    return m_surroundWithEnabled;
}

void AutoCompleter::countBracket(QChar open, QChar close, QChar c, int *errors, int *stillopen)
{
    if (c == open)
        ++*stillopen;
    else if (c == close)
        --*stillopen;

    if (*stillopen < 0) {
        *errors += -1 * (*stillopen);
        *stillopen = 0;
    }
}

void AutoCompleter::countBrackets(QTextCursor cursor,
                                  int from,
                                  int end,
                                  QChar open,
                                  QChar close,
                                  int *errors,
                                  int *stillopen)
{
    cursor.setPosition(from);
    QTextBlock block = cursor.block();
    while (block.isValid() && block.position() < end) {
        TextEditor::Parentheses parenList = TextEditor::BaseTextDocumentLayout::parentheses(block);
        if (!parenList.isEmpty() && !TextEditor::BaseTextDocumentLayout::ifdefedOut(block)) {
            for (int i = 0; i < parenList.count(); ++i) {
                TextEditor::Parenthesis paren = parenList.at(i);
                int position = block.position() + paren.pos;
                if (position < from || position >= end)
                    continue;
                countBracket(open, close, paren.chr, errors, stillopen);
            }
        }
        block = block.next();
    }
}

QString AutoCompleter::autoComplete(QTextCursor &cursor, const QString &textToInsert) const
{
    const bool checkBlockEnd = m_allowSkippingOfBlockEnd;
    m_allowSkippingOfBlockEnd = false; // consume blockEnd.

    if (m_surroundWithEnabled && cursor.hasSelection()) {
        if (textToInsert == QLatin1String("("))
            return cursor.selectedText() + QLatin1String(")");
        if (textToInsert == QLatin1String("{")) {
            //If the text span multiple lines, insert on different lines
            QString str = cursor.selectedText();
            if (str.contains(QChar::ParagraphSeparator)) {
                //Also, try to simulate auto-indent
                str = (str.startsWith(QChar::ParagraphSeparator) ? QString() : QString(QChar::ParagraphSeparator)) +
                      str;
                if (str.endsWith(QChar::ParagraphSeparator))
                    str += QLatin1String("}") + QString(QChar::ParagraphSeparator);
                else
                    str += QString(QChar::ParagraphSeparator) + QLatin1String("}");
            } else {
                str += QLatin1String("}");
            }
            return str;
        }
        if (textToInsert == QLatin1String("["))
            return cursor.selectedText() + QLatin1String("]");
        if (textToInsert == QLatin1String("\""))
            return cursor.selectedText() + QLatin1String("\"");
        if (textToInsert == QLatin1String("'"))
            return cursor.selectedText() + QLatin1String("'");
    }

    if (!m_autoParenthesesEnabled)
        return QString();

    if (!contextAllowsAutoParentheses(cursor, textToInsert))
        return QString();

    QTextDocument *doc = cursor.document();
    const QString text = textToInsert;
    const QChar lookAhead = doc->characterAt(cursor.selectionEnd());

    const QChar character = textToInsert.at(0);
    const QString parentheses = QLatin1String("()");
    const QString brackets = QLatin1String("[]");
    if (parentheses.contains(character) || brackets.contains(character)) {
        QTextCursor tmp= cursor;
        bool foundBlockStart = TextEditor::TextBlockUserData::findPreviousBlockOpenParenthesis(&tmp);
        int blockStart = foundBlockStart ? tmp.position() : 0;
        tmp = cursor;
        bool foundBlockEnd = TextEditor::TextBlockUserData::findNextBlockClosingParenthesis(&tmp);
        int blockEnd = foundBlockEnd ? tmp.position() : (cursor.document()->characterCount() - 1);
        const QChar openChar = parentheses.contains(character) ? QLatin1Char('(') : QLatin1Char('[');
        const QChar closeChar = parentheses.contains(character) ? QLatin1Char(')') : QLatin1Char(']');

        int errors = 0;
        int stillopen = 0;
        countBrackets(cursor, blockStart, blockEnd, openChar, closeChar, &errors, &stillopen);
        int errorsBeforeInsertion = errors + stillopen;
        errors = 0;
        stillopen = 0;
        countBrackets(cursor, blockStart, cursor.position(), openChar, closeChar, &errors, &stillopen);
        countBracket(openChar, closeChar, character, &errors, &stillopen);
        countBrackets(cursor, cursor.position(), blockEnd, openChar, closeChar, &errors, &stillopen);
        int errorsAfterInsertion = errors + stillopen;
        if (errorsAfterInsertion < errorsBeforeInsertion)
            return QString(); // insertion fixes parentheses or bracket errors, do not auto complete
    }

    int skippedChars = 0;
    const QString autoText = insertMatchingBrace(cursor, text, lookAhead, &skippedChars);

    if (checkBlockEnd && textToInsert.at(0) == QLatin1Char('}')) {
        if (textToInsert.length() > 1)
            qWarning() << "*** handle event compression";

        int startPos = cursor.selectionEnd(), pos = startPos;
        while (doc->characterAt(pos).isSpace())
            ++pos;

        if (doc->characterAt(pos) == QLatin1Char('}'))
            skippedChars += (pos - startPos) + 1;
    }

    if (skippedChars) {
        const int pos = cursor.position();
        cursor.setPosition(pos + skippedChars);
        cursor.setPosition(pos, QTextCursor::KeepAnchor);
    }

    return autoText;
}

bool AutoCompleter::autoBackspace(QTextCursor &cursor)
{
    m_allowSkippingOfBlockEnd = false;

    if (!m_autoParenthesesEnabled)
        return false;

    int pos = cursor.position();
    if (pos == 0)
        return false;
    QTextCursor c = cursor;
    c.setPosition(pos - 1);

    QTextDocument *doc = cursor.document();
    const QChar lookAhead = doc->characterAt(pos);
    const QChar lookBehind = doc->characterAt(pos - 1);
    const QChar lookFurtherBehind = doc->characterAt(pos - 2);

    const QChar character = lookBehind;
    if (character == QLatin1Char('(') || character == QLatin1Char('[')) {
        QTextCursor tmp = cursor;
        TextEditor::TextBlockUserData::findPreviousBlockOpenParenthesis(&tmp);
        int blockStart = tmp.isNull() ? 0 : tmp.position();
        tmp = cursor;
        TextEditor::TextBlockUserData::findNextBlockClosingParenthesis(&tmp);
        int blockEnd = tmp.isNull() ? (cursor.document()->characterCount()-1) : tmp.position();
        QChar openChar = character;
        QChar closeChar = (character == QLatin1Char('(')) ? QLatin1Char(')') : QLatin1Char(']');

        int errors = 0;
        int stillopen = 0;
        countBrackets(cursor, blockStart, blockEnd, openChar, closeChar, &errors, &stillopen);
        int errorsBeforeDeletion = errors + stillopen;
        errors = 0;
        stillopen = 0;
        countBrackets(cursor, blockStart, pos - 1, openChar, closeChar, &errors, &stillopen);
        countBrackets(cursor, pos, blockEnd, openChar, closeChar, &errors, &stillopen);
        int errorsAfterDeletion = errors + stillopen;

        if (errorsAfterDeletion < errorsBeforeDeletion)
            return false; // insertion fixes parentheses or bracket errors, do not auto complete
    }

    // ### this code needs to be generalized
    if    ((lookBehind == QLatin1Char('(') && lookAhead == QLatin1Char(')'))
        || (lookBehind == QLatin1Char('[') && lookAhead == QLatin1Char(']'))
        || (lookBehind == QLatin1Char('"') && lookAhead == QLatin1Char('"')
            && lookFurtherBehind != QLatin1Char('\\'))
        || (lookBehind == QLatin1Char('\'') && lookAhead == QLatin1Char('\'')
            && lookFurtherBehind != QLatin1Char('\\'))) {
        if (! isInComment(c)) {
            cursor.beginEditBlock();
            cursor.deleteChar();
            cursor.deletePreviousChar();
            cursor.endEditBlock();
            return true;
        }
    }
    return false;
}

int AutoCompleter::paragraphSeparatorAboutToBeInserted(QTextCursor &cursor,
                                                       const TabSettings &tabSettings)
{
    if (!m_autoParenthesesEnabled)
        return 0;

    QTextDocument *doc = cursor.document();
    if (doc->characterAt(cursor.position() - 1) != QLatin1Char('{'))
        return 0;

    if (!contextAllowsAutoParentheses(cursor))
        return 0;

    // verify that we indeed do have an extra opening brace in the document
    QTextBlock block = cursor.block();
    const QString textFromCusror = block.text().mid(cursor.positionInBlock()).trimmed();
    int braceDepth = BaseTextDocumentLayout::braceDepth(doc->lastBlock());

    if (braceDepth <= 0 && (textFromCusror.isEmpty() || textFromCusror.at(0) != QLatin1Char('}')))
        return 0; // braces are all balanced or worse, no need to do anything and separator inserted not between '{' and '}'

    // we have an extra brace , let's see if we should close it

    /* verify that the next block is not further intended compared to the current block.
       This covers the following case:

            if (condition) {|
                statement;
    */
    int indentation = tabSettings.indentationColumn(block.text());

    if (block.next().isValid()) { // not the last block
        block = block.next();
        //skip all empty blocks
        while (block.isValid() && tabSettings.onlySpace(block.text()))
            block = block.next();
        if (block.isValid()
                && tabSettings.indentationColumn(block.text()) > indentation)
            return 0;
    }

    const QString &textToInsert = insertParagraphSeparator(cursor);
    int pos = cursor.position();
    cursor.insertBlock();
    cursor.insertText(textToInsert);
    cursor.setPosition(pos);

    // if we actually insert a separator, allow it to be overwritten if
    // user types it
    if (!textToInsert.isEmpty())
        m_allowSkippingOfBlockEnd = true;

    return 1;
}

bool AutoCompleter::contextAllowsAutoParentheses(const QTextCursor &cursor,
                                                 const QString &textToInsert) const
{
    Q_UNUSED(cursor);
    Q_UNUSED(textToInsert);
    return true;
}

bool AutoCompleter::contextAllowsElectricCharacters(const QTextCursor &cursor) const
{
    return contextAllowsAutoParentheses(cursor);
}

bool AutoCompleter::isInComment(const QTextCursor &cursor) const
{
    Q_UNUSED(cursor);
    return false;
}

QString AutoCompleter::insertMatchingBrace(const QTextCursor &cursor,
                                           const QString &text,
                                           QChar la,
                                           int *skippedChars) const
{
    Q_UNUSED(cursor);
    Q_UNUSED(text);
    Q_UNUSED(la);
    Q_UNUSED(skippedChars);
    return QString();
}

QString AutoCompleter::insertParagraphSeparator(const QTextCursor &cursor) const
{
    Q_UNUSED(cursor);
    return QString();
}
