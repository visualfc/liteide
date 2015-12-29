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
// Module: editorutil.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "editorutil.h"
#include <QTextBlock>
#include <QTextCursor>
#include <QPlainTextEdit>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

typedef void (*EnumEditorProc)(QTextCursor &cursor, QTextBlock &block, void *param);

void EditorUtil::EnumEditor(QPlainTextEdit *ed, EnumEditorProc proc, void *param)
{
    if (!ed) {
        return;
    }
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

void EditorUtil::MarkSelection(QPlainTextEdit *ed, const QString &mark1, const QString &mark2)
{
    if (!ed) {
        return;
    }
    QTextCursor cur = ed->textCursor();
    cur.beginEditBlock();
    if (cur.hasSelection()) {
        QTextBlock begin = ed->document()->findBlock(cur.selectionStart());
        QTextBlock end = ed->document()->findBlock(cur.selectionEnd());
        if (end.position() == cur.selectionEnd()) {
            end = end.previous();
        }
        int n1 = cur.selectionStart();
        int n2 = cur.selectionEnd();
        QTextBlock block = begin;
        do {
            int c1 = block.position();
            int c2 = c1+block.text().length();
            if (block.position() == begin.position() && c1 < n1) {
                c1 = n1;
            }
            if (c2 > n2) {
                c2 = n2;
            }
            if (c2 > c1) {
                if (!mark1.isEmpty()) {
                    cur.setPosition(c1);
                    cur.insertText(mark1);
                    n2 += mark1.length();
                }
                if (!mark2.isEmpty()) {
                    cur.setPosition(c2+mark1.length());
                    cur.insertText(mark2);
                    n2 += mark2.length();
                }
            }
            block = block.next();
        } while(block.isValid() && block.position() <= end.position());
    } else {
        int pos = cur.position();
        cur.insertText(mark1+mark2);
        cur.setPosition(pos+mark1.length());
    }
    cur.endEditBlock();
    ed->setTextCursor(cur);
}

void EditorUtil::MarkSelection(QPlainTextEdit *ed, const QString &mark)
{
    EditorUtil::MarkSelection(ed,mark,mark);
}

void EditorUtil::loadDiff(QTextCursor &cursor, const QString &diff)
{
    QRegExp reg("@@\\s+\\-(\\d+),?(\\d*)\\s+\\+(\\d+),?(\\d*)\\s+@@");
    //@@ -1,11 +1,9 @@
    //@@ -1,9 +1 @@
    //@@ -1 +1,9 @@
    QTextBlock block;
    int line = -1;
    int line_add = 0;
    foreach(QString s, diff.split('\n')) {
        if (s.length() == 0) {
            continue;
        }
        QChar ch = s.at(0);
        if (ch == '@') {
            if (reg.indexIn(s) == 0) {
                int s1 = reg.cap(1).toInt();
                int s2 = reg.cap(2).toInt();
                //int n1 = reg.cap(3).toInt();
                int n2 = reg.cap(4).toInt();
                line = line_add+s1;
                block = cursor.document()->findBlockByNumber(line-1);
                line_add += n2-s2;//n2+n1-(s2+s1);
                continue;
            }
        }
        if (line == -1) {
            continue;
        }
        if (ch == '+') {
            cursor.setPosition(block.position());
            cursor.insertText(s.right(s.length()-1)+"\n");
            block = cursor.block();
            //break;
        } else if (ch == '-') {
            cursor.setPosition(block.position());
            if (block.next().isValid()) {
                cursor.setPosition(block.next().position(), QTextCursor::KeepAnchor);
            } else {
                cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            }
            cursor.removeSelectedText();
            block = cursor.block();
        } else if (ch == ' ') {
            block = block.next();
        } else if (ch == '\\') {
        }
    }
}


