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

#ifndef UNCOMMENTSELECTION_H
#define UNCOMMENTSELECTION_H

#include "utils_global.h"

#include <QtCore/QString>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

namespace Utils {

enum CommentFlag {
    AutoComment = 0,
    SingleComment,
    BlockComment,
};

class QTCREATOR_UTILS_EXPORT CommentDefinition
{
public:
    CommentDefinition();

    CommentDefinition &setAfterWhiteSpaces(const bool);
    CommentDefinition &setAfterWhiteSpacesAddSpace(const bool);
    CommentDefinition &setAfterMaxSpaces(int spaces);
    CommentDefinition &setSingleLine(const QString &singleLine);
    CommentDefinition &setMultiLineStart(const QString &multiLineStart);
    CommentDefinition &setMultiLineEnd(const QString &multiLineEnd);

    bool isAfterWhiteSpaces() const;
    bool isAfterWhiteSpacesAddSpace() const;
    const QString &singleLine() const;
    const QString &multiLineStart() const;
    const QString &multiLineEnd() const;

    bool hasSingleLineStyle() const;
    bool hasMultiLineStyle() const;

    void clearCommentStyles();

private:
    bool m_afterWhiteSpaces;
    bool m_afterWhiteSpacesAddSpace;
    QString m_singleLine;
    QString m_multiLineStart;
    QString m_multiLineEnd;
};

QTCREATOR_UTILS_EXPORT
void unCommentSelection(QPlainTextEdit *edit,
                        CommentFlag flag,
                        const CommentDefinition &definiton = CommentDefinition());

} // namespace Utils

#endif // UNCOMMENTSELECTION_H
