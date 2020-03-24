/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: editorutil.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef EDITORUTIL_H
#define EDITORUTIL_H

#include "liteapi/liteapi.h"

typedef void (*EnumEditorProc)(QTextCursor &cursor, QTextBlock &block, void *param);
class EditorUtil
{   
public:   
    static void EnumEditor(QPlainTextEdit *ed, EnumEditorProc proc, void *param);
    static void InsertHead(QPlainTextEdit *ed, const QString &tag, bool blockStart = true);
    static void RemoveHead(QPlainTextEdit *ed, const QStringList &tags, bool blockStart = true);
    static void SwitchHead(QPlainTextEdit *ed, const QString &tagAdd, const QStringList &tagRemove, bool blockStart = true);
    static void MarkSelection(QPlainTextEdit *ed, const QString &mark1, const QString &mark2);
    static void MarkSelection(QPlainTextEdit *ed, const QString &mark);
    static void loadDiff(QTextCursor &cursor, const QString &diff);
    static QString unifiedDiffText(const QString &text1, const QString &text2);
};

#endif // EDITORUTIL_H
