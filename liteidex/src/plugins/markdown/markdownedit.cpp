/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: markdownedit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "markdownedit.h"
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QTextCursor>
#include <QTextBlock>
#include <QPlainTextEdit>
#include <QTextDocument>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

class Separator : public QAction
{
public:
    Separator(QObject *parent) :
        QAction(parent)
    {
        setSeparator(true);
    }
};

MarkdownEdit::MarkdownEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent) :
    QObject(parent), m_liteApp(app)
{
    m_editor = LiteApi::getTextEditor(editor);
    if (!m_editor) {
        return;
    }
    m_ed = LiteApi::getPlainTextEdit(editor);
    if (!m_ed) {
        return;
    }
    m_ed->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    QAction *h1 = new QAction(QIcon("icon:markdown/images/h1.png"),"H1",this);
    h1->setShortcut(QKeySequence("Ctrl+1"));
    QAction *h2 = new QAction(QIcon("icon:markdown/images/h2.png"),"H2",this);
    h2->setShortcut(QKeySequence("Ctrl+2"));
    QAction *h3 = new QAction(QIcon("icon:markdown/images/h3.png"),"H3",this);
    h3->setShortcut(QKeySequence("Ctrl+3"));
    QAction *bold = new QAction(QIcon("icon:markdown/images/bold.png"),"Bold",this);
    bold->setShortcut(QKeySequence::Bold);
    QAction *italic = new QAction(QIcon("icon:markdown/images/italic.png"),"Italic",this);
    italic->setShortcut(QKeySequence::Italic);
    QAction *code = new QAction(QIcon("icon:markdown/images/code.png"),"Inline Code",this);
    code->setShortcut(QKeySequence("Ctrl+K"));

    QList<QAction*> actions;
    actions << new Separator(this);
    actions << h1 << h2 << h3;
    actions << bold << italic << code;

    QMenu *menu = LiteApi::findExtensionObject<QMenu*>(editor,"LiteApi.Menu.Edit");
    QToolBar *toolBar = LiteApi::findExtensionObject<QToolBar*>(editor,"LiteApi.QToolBar");
    QAction *spacer = LiteApi::findExtensionObject<QAction*>(editor,"LiteApi.QToolBar.Spacer");

    if (menu) {
        menu->addActions(actions);
    }
    if (toolBar) {
        toolBar->insertActions(spacer,actions);
    }

    connect(editor,SIGNAL(destroyed()),this,SLOT(deleteLater()));
    connect(h1,SIGNAL(triggered()),this,SLOT(h1()));
    connect(h2,SIGNAL(triggered()),this,SLOT(h2()));
    connect(h3,SIGNAL(triggered()),this,SLOT(h3()));
    connect(bold,SIGNAL(triggered()),this,SLOT(bold()));
    connect(italic,SIGNAL(triggered()),this,SLOT(italic()));
    connect(code,SIGNAL(triggered()),this,SLOT(code()));
}

MarkdownEdit::~MarkdownEdit()
{
}



void MarkdownEdit::insert_head(const QString &tag)
{
    QTextCursor cur = m_ed->textCursor();
    this->gotoLine(cur.block().firstLineNumber(),0);
    cur = m_ed->textCursor();
    cur.insertText(tag+" ");
    m_ed->setTextCursor(cur);
}

void MarkdownEdit::mark_selection(const QString &mark)
{
    QTextCursor cur = m_ed->textCursor();
    cur.beginEditBlock();
    if (cur.hasSelection()) {
        int s = cur.selectionStart();
        int e = cur.selectionEnd();
        QString text = cur.selectedText();
        cur.removeSelectedText();
        cur.insertText(mark+text+mark);
        cur.setPosition(s);
        cur.setPosition(e+mark.length()*2,QTextCursor::KeepAnchor);
    } else {
        cur.insertText(mark+mark);
        cur.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,mark.length());
    }
    cur.endEditBlock();
    m_ed->setTextCursor(cur);
}

void MarkdownEdit::h1()
{
    insert_head("#");
}

void MarkdownEdit::h2()
{
    insert_head("##");
}

void MarkdownEdit::h3()
{
    insert_head("###");
}

void MarkdownEdit::bold()
{
    mark_selection("**");
}

void MarkdownEdit::italic()
{
    mark_selection("_");
}

void MarkdownEdit::code()
{
    mark_selection("`");
}


void MarkdownEdit::gotoLine(int line, int column)
{
    const int blockNumber = line;
    const QTextBlock &block = m_ed->document()->findBlockByLineNumber(blockNumber);
    if (block.isValid()) {
        QTextCursor cursor(block);
        if (column > 0) {
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
        } else {
            int pos = cursor.position();
            while (m_ed->document()->characterAt(pos).category() == QChar::Separator_Space) {
                ++pos;
            }
            cursor.setPosition(pos);
        }
        m_ed->setTextCursor(cursor);
        m_ed->ensureCursorVisible();
    }
 }
