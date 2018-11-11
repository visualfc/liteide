/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#include "uncommentselection.h"
#include <QtCore/QtGlobal>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextDocument>

using namespace Utils;

CommentDefinition::CommentDefinition() :
    m_afterWhiteSpaces(true),
    m_afterWhiteSpacesAddSpace(true),
    m_singleLine(QLatin1String("//")),
    m_multiLineStart(QLatin1String("/*")),
    m_multiLineEnd(QLatin1String("*/"))
{}

CommentDefinition &CommentDefinition::setAfterWhiteSpaces(const bool afterWhiteSpaces)
{
    m_afterWhiteSpaces = afterWhiteSpaces;
    return *this;
}

CommentDefinition &CommentDefinition::setAfterWhiteSpacesAddSpace(const bool afterWhiteSpacesAddSpace)
{
    m_afterWhiteSpacesAddSpace = afterWhiteSpacesAddSpace;
    return *this;
}

CommentDefinition &CommentDefinition::setSingleLine(const QString &singleLine)
{
    m_singleLine = singleLine;
    return *this;
}

CommentDefinition &CommentDefinition::setMultiLineStart(const QString &multiLineStart)
{
    m_multiLineStart = multiLineStart;
    return *this;
}

CommentDefinition &CommentDefinition::setMultiLineEnd(const QString &multiLineEnd)
{
    m_multiLineEnd = multiLineEnd;
    return *this;
}

bool CommentDefinition::isAfterWhiteSpaces() const
{ return m_afterWhiteSpaces; }

bool CommentDefinition::isAfterWhiteSpacesAddSpace() const
{   return m_afterWhiteSpacesAddSpace; }

const QString &CommentDefinition::singleLine() const
{ return m_singleLine; }

const QString &CommentDefinition::multiLineStart() const
{ return m_multiLineStart; }

const QString &CommentDefinition::multiLineEnd() const
{ return m_multiLineEnd; }

bool CommentDefinition::hasSingleLineStyle() const
{ return !m_singleLine.isEmpty(); }

bool CommentDefinition::hasMultiLineStyle() const
{ return !m_multiLineStart.isEmpty() && !m_multiLineEnd.isEmpty(); }

void CommentDefinition::clearCommentStyles()
{
    m_singleLine.clear();
    m_multiLineStart.clear();
    m_multiLineEnd.clear();
}

namespace {

bool isComment(const QString &text,
               int index,
               const CommentDefinition &definition,
               const QString & (CommentDefinition::* comment) () const)
{
    const QString &commentType = ((definition).*(comment))();
    const int length = commentType.length();

    Q_ASSERT(text.length() - index >= length);

    int i = 0;
    while (i < length) {
        if (text.at(index + i) != commentType.at(i))
            return false;
        ++i;
    }
    return true;
}

} // namespace anynomous

void Utils::unCommentSelection(QPlainTextEdit *edit, CommentFlag flag, const CommentDefinition &definition)
{
    if (!definition.hasSingleLineStyle() && !definition.hasMultiLineStyle())
        return;

    QTextCursor cursor = edit->textCursor();
    QTextDocument *doc = cursor.document();

    if (!cursor.hasSelection() && (flag == BlockComment) ) {
        if (definition.hasMultiLineStyle()) {
            cursor.beginEditBlock();
            cursor.insertText(definition.multiLineStart());
            cursor.insertText(definition.multiLineEnd());
            cursor.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,definition.multiLineEnd().length());
            cursor.endEditBlock();
            edit->setTextCursor(cursor);
            return;
        }
    }

    cursor.beginEditBlock();

    int pos = cursor.position();
    int anchor = cursor.anchor();
    int start = qMin(anchor, pos);
    int end = qMax(anchor, pos);
    bool anchorIsStart = (anchor == start);

    QTextBlock startBlock = doc->findBlock(start);
    QTextBlock endBlock = doc->findBlock(end);

    if (end > start && endBlock.position() == end) {
        --end;
        endBlock = endBlock.previous();
    }

    bool doMultiLineStyleUncomment = false;
    bool doMultiLineStyleComment = false;
    bool doSingleLineStyleUncomment = false;

    bool hasSelection = cursor.hasSelection();
    int firstSpacesOffset = -1;

    if (hasSelection && definition.hasMultiLineStyle()) {

        QString startText = startBlock.text();
        int startPos = start - startBlock.position();
        const int multiLineStartLength = definition.multiLineStart().length();
        bool hasLeadingCharacters = !startText.left(startPos).trimmed().isEmpty();

        if (startPos >= multiLineStartLength
            && isComment(startText,
                         startPos - multiLineStartLength,
                         definition,
                         &CommentDefinition::multiLineStart)) {
            startPos -= multiLineStartLength;
            start -= multiLineStartLength;
        }

        bool hasSelStart = (startPos <= startText.length() - multiLineStartLength
                            && isComment(startText,
                                         startPos,
                                         definition,
                                         &CommentDefinition::multiLineStart));

        QString endText = endBlock.text();
        int endPos = end - endBlock.position();
        const int multiLineEndLength = definition.multiLineEnd().length();
        bool hasTrailingCharacters =
                !endText.left(endPos).remove(definition.singleLine()).trimmed().isEmpty()
                && !endText.mid(endPos).trimmed().isEmpty();

        if (endPos <= endText.length() - multiLineEndLength
            && isComment(endText, endPos, definition, &CommentDefinition::multiLineEnd)) {
            endPos += multiLineEndLength;
            end += multiLineEndLength;
        }

        bool hasSelEnd = (endPos >= multiLineEndLength
                          && isComment(endText,
                                       endPos - multiLineEndLength,
                                       definition,
                                       &CommentDefinition::multiLineEnd));

        doMultiLineStyleUncomment = hasSelStart && hasSelEnd;
        doMultiLineStyleComment = !doMultiLineStyleUncomment
                                  && (hasLeadingCharacters
                                      || hasTrailingCharacters
                                      || !definition.hasSingleLineStyle()
                                      || (flag == BlockComment));
    } else if (!hasSelection && !definition.hasSingleLineStyle()) {

        QString text = startBlock.text().trimmed();
        doMultiLineStyleUncomment = text.startsWith(definition.multiLineStart())
                                    && text.endsWith(definition.multiLineEnd());
        doMultiLineStyleComment = !doMultiLineStyleUncomment && !text.isEmpty();

        start = startBlock.position();
        end = endBlock.position() + endBlock.length() - 1;

        if (doMultiLineStyleUncomment) {
            int offset = 0;
            text = startBlock.text();
            const int length = text.length();
            while (offset < length && text.at(offset).isSpace())
                ++offset;
            start += offset;
        }
    }

    if (flag == SingleComment) {
        if (doMultiLineStyleComment) {
            doMultiLineStyleComment = false;
        }
    }

    if (doMultiLineStyleUncomment) {
        cursor.setPosition(end);
        cursor.movePosition(QTextCursor::PreviousCharacter,
                            QTextCursor::KeepAnchor,
                            definition.multiLineEnd().length());
        cursor.removeSelectedText();
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::NextCharacter,
                            QTextCursor::KeepAnchor,
                            definition.multiLineStart().length());
        cursor.removeSelectedText();
    } else if (doMultiLineStyleComment) {
        cursor.setPosition(end);
        cursor.insertText(definition.multiLineEnd());
        cursor.setPosition(start);
        cursor.insertText(definition.multiLineStart());
    } else {
        endBlock = endBlock.next();
        doSingleLineStyleUncomment = true;
        for (QTextBlock block = startBlock; block != endBlock; block = block.next()) {
            QString text = block.text().trimmed();
            if (!text.isEmpty() && !text.startsWith(definition.singleLine())) {
                doSingleLineStyleUncomment = false;
                break;
            }
        }
        if (!hasSelection && cursor.block().text().isEmpty()) {
            doSingleLineStyleUncomment = false;
        }
        const int singleLineLength = definition.singleLine().length();
        for (QTextBlock block = startBlock; block != endBlock; block = block.next()) {
            if (doSingleLineStyleUncomment) {
                QString text = block.text();
                int i = 0;
                while (i <= text.size() - singleLineLength) {
                    if (isComment(text, i, definition, &CommentDefinition::singleLine)) {
                        cursor.setPosition(block.position() + i);
                        cursor.movePosition(QTextCursor::NextCharacter,
                                            QTextCursor::KeepAnchor,
                                            singleLineLength);
                        if (definition.isAfterWhiteSpacesAddSpace()) {
                            if (i < text.size()-singleLineLength) {
                                if (text.at(i+singleLineLength) == 0x0020) {
                                    cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,1);
                                }
                            }
                        }
                        cursor.removeSelectedText();
                        break;
                    }
                    if (!text.at(i).isSpace())
                        break;
                    ++i;
                }
            } else {
                QString text = block.text();
                foreach(QChar c, text) {
                    if (!c.isSpace()) {
                        if (definition.isAfterWhiteSpaces()) {
                            int offset = text.indexOf(c);
                            if (firstSpacesOffset != -1 && offset > firstSpacesOffset) {
                                offset = firstSpacesOffset;
                            }
                            cursor.setPosition(block.position() + offset);
                        } else {
                            cursor.setPosition(block.position());
                        }
                        if (firstSpacesOffset == -1) {
                            firstSpacesOffset = cursor.position()-cursor.block().position();
                        }
                        if (definition.isAfterWhiteSpaces() && definition.isAfterWhiteSpacesAddSpace()) {
                            cursor.insertText(definition.singleLine()+" ");
                        } else {
                            cursor.insertText(definition.singleLine());
                        }
                        break;
                    }
                }
            }
        }
    }

    // adjust selection when commenting out
    if (hasSelection && !doMultiLineStyleUncomment && !doSingleLineStyleUncomment) {
        cursor = edit->textCursor();
        if (!doMultiLineStyleComment)
            start = startBlock.position(); // move the comment into the selection
        int lastSelPos = anchorIsStart ? cursor.position() : cursor.anchor();
        if (anchorIsStart) {
            cursor.setPosition(start);
            cursor.setPosition(lastSelPos, QTextCursor::KeepAnchor);
        } else {
            cursor.setPosition(lastSelPos);
            cursor.setPosition(start, QTextCursor::KeepAnchor);
        }
        edit->setTextCursor(cursor);
    }

    cursor.endEditBlock();
}
