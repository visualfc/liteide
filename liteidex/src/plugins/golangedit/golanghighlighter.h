/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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

#include "qtc_texteditor/syntaxhighlighter.h"

#include <QTextCharFormat>

class GolangHighlighter : public TextEditor::SyntaxHighlighter
{
    Q_OBJECT

public:
    GolangHighlighter(QTextDocument *document = 0);
    virtual ~GolangHighlighter();
    virtual void highlightBlock(const QString &text);

private:
    void highlightWord(QStringRef word, int position, int length);
    void highlightLine(const QString &line, int position, int length,
                       const QTextCharFormat &format);
    void highlightCommentLine(const QString &line, int position, int length);

    bool isPPKeyword(const QStringRef &text) const;
protected:
    QStringList m_todoList;
};

#endif // GOLANGHIGHLIGHTER_H
