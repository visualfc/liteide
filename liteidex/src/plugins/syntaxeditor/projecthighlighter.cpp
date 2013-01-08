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
// Module: projecthighlighter.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "projecthighlighter.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


ProjectHighlighter::ProjectHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document)
{
    keywordFormat.setForeground(Qt::darkBlue);
    commentFormat.setForeground(Qt::darkCyan);

    HighlightingRule rule;
    //keyword
    rule.pattern = QRegExp("\\b"
                           "(TARGET|DESTDIR|GOFILES|CGOFILES|GCOPT|GLOPT)"
                           "\\b");
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegExp("^\\s*#.*");
    rule.format = commentFormat;
    highlightingRules.append(rule);
}

void ProjectHighlighter::highlightBlock(const QString &text)
{
    int startPos = 0;
    //int endPos = text.length();
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text,startPos);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, startPos+index + length);
        }
    }
}
