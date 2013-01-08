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
// Module: liteeditormark.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORMARK_H
#define LITEEDITORMARK_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "qtc_texteditor/basetextdocumentlayout.h"
#include "liteeditor.h"

class LiteTextMark : public TextEditor::ITextMark
{
    Q_OBJECT
public:
    LiteTextMark(int type, QObject *parent = 0);
    int type() const { return m_type; }
protected:
    int m_type;
};

class LiteEditorMarkTypeManager : public LiteApi::IEditorMarkTypeManager
{
    Q_OBJECT
public:
    LiteEditorMarkTypeManager(QObject *parent = 0);
    virtual ~LiteEditorMarkTypeManager();
    virtual void registerMark(int type, const QIcon &icon);
    virtual QList<int> markTypeList() const;
    virtual LiteTextMark *mark(int type) const;
protected:
    QMap<int,LiteTextMark*> m_typeMarkMap;
};

typedef QMap<int,QList<int> > MarkTypesMap;

class LiteEditorMark : public LiteApi::IEditorMark
{
    Q_OBJECT
public:
    explicit LiteEditorMark(LiteEditorMarkTypeManager *manager, QTextDocument *document, QObject *parent);
    virtual void addMark(int line, int type);
    virtual void removeMark(int line, int type);
    virtual QList<int> markList(int type) const;
    virtual QList<int> lineTypeList(int line) const;
protected:
    LiteEditorMarkTypeManager *m_manager;
    QTextDocument *m_document;
    QMap<int,TextEditor::ITextMark*> m_typeMarkMap;
    MarkTypesMap m_lineMarkTypesMap;
};

Q_DECLARE_METATYPE(MarkTypesMap)

#endif // LITEEDITORMARK_H
