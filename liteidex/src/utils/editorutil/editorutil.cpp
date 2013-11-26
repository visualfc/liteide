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
// Module: editorutil.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "editorutil.h"

typedef void (*EnumEditorProc)(QTextCursor &cursor, QTextBlock &block, void *param);

void EditorUtil::EnumEditor(QPlainTextEdit *ed, EnumEditorProc proc, void *param)
{
    QTextCursor cur = ed->textCursor();
    cur.beginEditBlock();
    if (cur.hasSelection()) {
        QTextBlock begin = ed->document()->findBlock(cur.selectionStart());
        QTextBlock end = ed->document()->findBlock(cur.selectionEnd());
        if (end.position() == cur.selectionEnd()) {
            end = end.previous();
        }
        QTextBlock block = begin;
        do {
            if (block.text().length() > 0) {
                proc(cur,block,param);
            }
            block = block.next();
        } while(block.isValid() && block.position() <= end.position());
    } else {
        QTextBlock block = cur.block();
        proc(cur,block,param);
    }
    cur.endEditBlock();
    ed->setTextCursor(cur);
}

struct InsertHeadParam
{
    QString tag;
    bool blockStart;
};

static void insertHead(QTextCursor &cur, QTextBlock &block, void *param)
{
    InsertHeadParam *ip = (InsertHeadParam*)param;
    if (ip->blockStart) {
        cur.setPosition(block.position());
    } else {
        QString text = block.text();
        foreach(QChar c, text) {
            if (!c.isSpace()) {
                cur.setPosition(block.position()+text.indexOf(c));
                break;
            }
        }
    }
    cur.insertText(ip->tag);
}

struct RemoveHeadParam
{
    QStringList tags;
    bool blockStart;
};

struct SwitchHeadParam
{
    QString tagAdd;
    QStringList tagRemove;
    bool blockStart;
};

static void removeHead(QTextCursor &cur, QTextBlock &block, void *param)
{
    RemoveHeadParam *ip = (RemoveHeadParam*)param;
    if (ip->blockStart) {
        cur.setPosition(block.position());
    } else {
        QString text = block.text();
        foreach(QChar c, text) {
            if (!c.isSpace()) {
                cur.setPosition(block.position()+text.indexOf(c));
                break;
            }
        }
    }
    foreach (QString tag, ip->tags) {
        if (cur.block().text().startsWith(tag)) {
            cur.setPosition(block.position());
            cur.movePosition(QTextCursor::NextCharacter,
                                QTextCursor::KeepAnchor,
                                tag.length());
            cur.removeSelectedText();
            break;
        }
    }
}

static void switchHead(QTextCursor &cur, QTextBlock &block, void *param)
{
    SwitchHeadParam *ip = (SwitchHeadParam*)param;
    if (ip->blockStart) {
        cur.setPosition(block.position());
    } else {
        QString text = block.text();
        foreach(QChar c, text) {
            if (!c.isSpace()) {
                cur.setPosition(block.position()+text.indexOf(c));
                break;
            }
        }
    }
    foreach (QString tag, ip->tagRemove) {
        if (cur.block().text().startsWith(tag)) {
            cur.setPosition(block.position());
            cur.movePosition(QTextCursor::NextCharacter,
                                QTextCursor::KeepAnchor,
                                tag.length());
            cur.removeSelectedText();
            return;
        }
    }
    cur.insertText(ip->tagAdd);
}


void EditorUtil::InsertHead(QPlainTextEdit *ed, const QString &tag, bool blockStart)
{
    InsertHeadParam param = {tag,blockStart};
    EnumEditor(ed,&insertHead,&param);
}

void EditorUtil::RemoveHead(QPlainTextEdit *ed, const QStringList &tags, bool blockStart)
{
    RemoveHeadParam param = {tags,blockStart};
    EnumEditor(ed,&removeHead,&param);
}

void EditorUtil::SwitchHead(QPlainTextEdit *ed, const QString &tagAdd, const QStringList &tagRemove, bool blockStart)
{
    SwitchHeadParam param = {tagAdd,tagRemove,blockStart};
    EnumEditor(ed,&switchHead,&param);
}

