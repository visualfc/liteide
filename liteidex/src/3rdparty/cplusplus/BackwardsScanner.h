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
#ifndef CPLUSPLUS_BACKWARDSSCANNER_H
#define CPLUSPLUS_BACKWARDSSCANNER_H

#include "SimpleLexer.h"

#include <QTextBlock>

QT_FORWARD_DECLARE_CLASS(QTextCursor)

namespace CPlusPlus {

class BackwardsScanner
{
    enum { MAX_BLOCK_COUNT = 10 };

public:
    explicit BackwardsScanner(LanguageFeatures features,
                              const QTextCursor &cursor,
                              int maxBlockCount = MAX_BLOCK_COUNT,
                              const QString &suffix = QString(),
                              bool skipComments = true);

    int startToken() const;

    int startPosition() const;

    QString text() const;
    QString mid(int index) const;

    QString text(int index) const;
    QStringRef textRef(int index) const;
    // 1-based
    Token LA(int index) const;

    // n-la token is [startToken - n]
    Token operator[](int index) const; // ### deprecate

    QString indentationString(int index) const;

    int startOfLine(int index) const;
    int startOfMatchingBrace(int index) const;
    int startOfBlock(int index) const;

    int size() const;

    static int previousBlockState(const QTextBlock &block);

private:
    const Token &fetchToken(int tokenIndex);

private:
    QList<Token> _tokens;
    int _offset;
    int _blocksTokenized;
    QTextBlock _block;
    SimpleLexer _tokenize;
    QString _text;
    int _maxBlockCount;
    int _startToken;
};

} // namespace CPlusPlus

#endif // CPLUSPLUS_BACKWARDSSCANNER_H
