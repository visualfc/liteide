/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: liteeditormark.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: liteeditormark.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "liteeditormark.h"
#include "qtc_texteditor/basetextdocumentlayout.h"
#include "liteeditorwidget.h"
#include <QTextDocument>
#include <QTextBlock>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteTextMark::LiteTextMark(int type, QObject *parent)
    : TextEditor::ITextMark(parent),
      m_type(type)
{

}


LiteEditorMarkTypeManager::LiteEditorMarkTypeManager(QObject *parent) :
    LiteApi::IEditorMarkTypeManager(parent)
{
}

LiteEditorMarkTypeManager::~LiteEditorMarkTypeManager()
{
    qDeleteAll(m_typeMarkMap);
    m_typeMarkMap.clear();
}

void LiteEditorMarkTypeManager::registerMark(int type, const QIcon &icon)
{
    LiteTextMark *mark = new LiteTextMark(type,this);
    mark->setIcon(icon);
    m_typeMarkMap.insert(type,mark);
}

QList<int> LiteEditorMarkTypeManager::markTypeList() const
{
    return m_typeMarkMap.keys();
}

LiteTextMark *LiteEditorMarkTypeManager::mark(int type) const
{
    return m_typeMarkMap.value(type);
}

LiteEditorMark::LiteEditorMark(LiteEditorMarkTypeManager *manager, QTextDocument *document, QObject *parent) :
    LiteApi::IEditorMark(parent),
    m_manager(manager),
    m_document(document)
{
}

void LiteEditorMark::addMark(int line, int type)
{
    const QTextBlock &block = m_document->findBlockByNumber(line);
    if (!block.isValid()) {
        return;
    }
    TextEditor::ITextMark *mark = m_manager->mark(type);
    if (!mark) {
        return;
    }
    TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
    if (data) {
        if (data->marks().contains(mark)) {
            return;
        }
        data->addMark(mark);
        emit markChanged();
    }
}

void LiteEditorMark::removeMark(int line, int type)
{
    const QTextBlock &block = m_document->findBlockByNumber(line);
    if (!block.isValid()) {
        return;
    }
    TextEditor::ITextMark *mark = m_manager->mark(type);
    if (!mark) {
        return;
    }
    TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
    if (data) {
        if (data->removeMark(mark)) {
            emit markChanged();
        }
    }
}

QList<int> LiteEditorMark::markList(int type) const
{
    QList<int> lineList;
    QTextBlock block = m_document->firstBlock();
    while (block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data) {
            foreach(TextEditor::ITextMark *mark, data->marks() ) {
                if ( ((LiteTextMark*)mark)->type() == type ) {
                    lineList.append(block.blockNumber());
                    break;
                }
            }
        }
        block = block.next();
    }
    return lineList;
}

QList<int> LiteEditorMark::lineTypeList(int line) const
{
    QList<int> typeList;
    const QTextBlock &block = m_document->findBlockByNumber(line);
    if (!block.isValid()) {
        return typeList;
    }
    TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
    if (data) {
        foreach(TextEditor::ITextMark *mark, data->marks()) {
            typeList.append(((LiteTextMark*)mark)->type());
        }
    }
    return typeList;
}
