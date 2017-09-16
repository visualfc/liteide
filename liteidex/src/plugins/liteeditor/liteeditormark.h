/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: liteeditormark.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORMARK_H
#define LITEEDITORMARK_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "qtc_texteditor/basetextdocumentlayout.h"
#include "liteeditor.h"

class LiteTextMark;
class LiteEditorMarkTypeManager : public LiteApi::IEditorMarkTypeManager
{
    Q_OBJECT
public:
    LiteEditorMarkTypeManager(QObject *parent = 0);
    virtual ~LiteEditorMarkTypeManager();
    virtual void registerMark(int type, const QIcon &icon);
    virtual QList<int> markTypeList() const;
    virtual QIcon iconForType(int type) const;
protected:
    QMap<int,QIcon> m_typeIconMap;
};

typedef QMap<int,QMap<int,TextEditor::ITextMark*> > TypeLineMarkMap;

class LiteEditorMark : public LiteApi::IEditorMark
{
    Q_OBJECT
public:
    explicit LiteEditorMark(LiteEditorMarkTypeManager *manager, LiteEditor *editor);
    virtual void addMark(int line, int type);
    virtual void removeMark(int line, int type);
    virtual QList<int> markLinesByType(int type) const;
    virtual QList<QTextBlock> markBlocksByType(int type) const;
    virtual QList<int> markTypesByLine(int line) const;
    virtual LiteEditor *editor() const;
    LiteTextMark *createMarkByType(int type, int line, const QTextBlock &block);
    void removedFromEditor(LiteTextMark *mark);
    void updateLineNumber(LiteTextMark *mark, int newLine, int oldLine);
    void updateLineBlock(LiteTextMark *mark);
protected:
    LiteEditorMarkTypeManager *m_manager;
    LiteEditor * m_editor;
    QTextDocument *m_document;
    TypeLineMarkMap m_typeLineMarkMap;
};

class LiteTextMark : public TextEditor::ITextMark
{
    Q_OBJECT
public:
    LiteTextMark(LiteEditorMark *editorMark, int type, int lineNumber, const QTextBlock &block, QObject *parent = 0);
    virtual ~LiteTextMark();
    virtual void removedFromEditor();
    virtual void updateLineNumber(int lineNumber);
    virtual void updateBlock(const QTextBlock &block);
    int type() const { return m_type; }
    QTextBlock block() const;
protected:
    LiteEditorMark *m_editorMark;
    int m_type;
    QTextBlock m_block;
};

#endif // LITEEDITORMARK_H
