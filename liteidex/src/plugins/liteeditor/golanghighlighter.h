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
// Module: golanghighlighter.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGHIGHLIGHTER_H
#define GOLANGHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QVector>
#include <QTextCharFormat>
#include <QSet>
#include <QSharedPointer>

class QTextDocument;

class GolangHighlighter: public QSyntaxHighlighter
{
    Q_OBJECT
public:
    GolangHighlighter(QTextDocument* document);
protected:
    enum
    {
        STATE_BACKQUOTES = 0x04,
        STATE_SINGLELINE_COMMENT = 0x08,
        STATE_MULTILINE_COMMENT = 0x10
    };
    virtual void highlightBlock(const QString &text);
    bool highlightPreBlock(const QString &text, int &startPos, int &endPos);
    int findQuotesEndPos(const QString &text, int startPos, const QChar &endChar);
public:
    QSharedPointer< QSet<QString> >   allWords;
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QRegExp         regexpQuotesAndComment;
    QTextCharFormat functionFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat identFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat quotesFormat;
};


#endif // GOLANGHIGHLIGHTER_H
