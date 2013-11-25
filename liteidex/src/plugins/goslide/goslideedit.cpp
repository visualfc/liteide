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
// Module: goslideedit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "goslideedit.h"
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QTextCursor>
#include <QTextBlock>

GoSlideEdit::GoSlideEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent) :
    QObject(parent), m_liteApp(app)
{
    m_editor = LiteApi::getTextEditor(editor);
    if (!m_editor) {
        return;
    }
    m_ed = LiteApi::getPlainTextEdit(editor);
    if (m_ed) {
        m_ed->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    }

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GoSlide");

    QAction *s1 = new QAction(QIcon("icon:goslide/images/s1.png"),tr("Section (s1)"),this);
    actionContext->regAction(s1,"Section","Ctrl+1");

    QAction *s2 = new QAction(QIcon("icon:goslide/images/s2.png"),tr("Subsection (s2)"),this);
    actionContext->regAction(s2,"Subsection","Ctrl+2");

    QAction *s3 = new QAction(QIcon("icon:goslide/images/s3.png"),tr("Sub-subsection (s3)"),this);
    actionContext->regAction(s3,"Sub-subsection","Ctrl+3");

    QAction *comment = new QAction(tr("Comment/Uncomment Selection"),this);
    actionContext->regAction(comment,"Comment","Ctrl+/");

    connect(m_editor,SIGNAL(destroyed()),this,SLOT(deleteLater()));
    connect(s1,SIGNAL(triggered()),this,SLOT(s1()));
    connect(s2,SIGNAL(triggered()),this,SLOT(s2()));
    connect(s3,SIGNAL(triggered()),this,SLOT(s3()));
    connect(comment,SIGNAL(triggered()),this,SLOT(comment()));

    QToolBar *toolBar = LiteApi::findExtensionObject<QToolBar*>(editor,"LiteApi.QToolBar");
    if (toolBar) {
        toolBar->addSeparator();
        toolBar->addAction(s1);
        toolBar->addAction(s2);
        toolBar->addAction(s3);
    }

    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(s1);
        menu->addAction(s2);
        menu->addAction(s3);
        menu->addSeparator();
        menu->addAction(comment);
    }

    menu = LiteApi::getContextMenu(editor);
    if (menu) {
       menu->addSeparator();
       menu->addAction(s1);
       menu->addAction(s2);
       menu->addAction(s3);
       menu->addSeparator();
       menu->addAction(comment);
    }
}

static void insert_block(const QString &tag, QTextCursor &cur, QTextBlock &block, bool blockStart, GoSlideEdit::update_flag flag)
{
    if (blockStart) {
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
    if (flag == GoSlideEdit::UPDATE_ADD) {
        cur.insertText(tag);
    } else if (flag == GoSlideEdit::UPDATE_REMOVE ||
               flag == GoSlideEdit::UPDATE_SWITCH) {
        int len = 0;
        if (cur.block().text().startsWith(tag)) {
            len = tag.length();
        } else if (cur.block().text().startsWith(tag.trimmed())) {
            len = tag.trimmed().length();
        }
        if (len > 0) {
            cur.setPosition(block.position());
            cur.movePosition(QTextCursor::NextCharacter,
                                QTextCursor::KeepAnchor,
                                len);
            cur.removeSelectedText();
        }
        if (len == 0 && flag == GoSlideEdit::UPDATE_SWITCH) {
            cur.insertText(tag);
        }
    }
}

void GoSlideEdit::update_head(const QString &tag, bool blockStart, update_flag flag)
{
    QTextCursor cur = m_ed->textCursor();
    cur.beginEditBlock();
    if (cur.hasSelection()) {
        QTextBlock begin = m_ed->document()->findBlock(cur.selectionStart());
        QTextBlock end = m_ed->document()->findBlock(cur.selectionEnd());
        if (end.position() == cur.selectionEnd()) {
            end = end.previous();
        }
        QTextBlock block = begin;
        do {
            if (block.text().length() > 0) {
               insert_block(tag,cur,block,blockStart,flag);
            }
            block = block.next();
        } while(block.isValid() && block.position() <= end.position());
    } else {
        QTextBlock block = cur.block();
        insert_block(tag,cur,block,blockStart,flag);
    }
    cur.endEditBlock();
    m_ed->setTextCursor(cur);
}

void GoSlideEdit::s1()
{
    update_head("* ");
}

void GoSlideEdit::s2()
{
    update_head("** ");
}

void GoSlideEdit::s3()
{
    update_head("*** ");
}

void GoSlideEdit::comment()
{
    update_head("# ",true,UPDATE_SWITCH);
}
