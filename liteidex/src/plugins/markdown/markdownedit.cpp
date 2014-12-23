/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
#include <QRegExp>
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

//class Separator : public QAction
//{
//public:
//    Separator(QObject *parent) :
//        QAction(parent)
//    {
//        setSeparator(true);
//    }
//};

//static void updateToolTip(QToolBar *toolBar)
//{
//    foreach(QAction *act, toolBar->actions()) {
//        if (act->isSeparator()) {
//            continue;
//        }
//        if (act->toolTip() == act->text() && !act->shortcut().isEmpty()) {
//            act->setToolTip(QString("%1 (%2)").arg(act->text()).arg(act->shortcut().toString()));
//        }
//    }
//}

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
    m_editor->setWordWrap(true);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"Markdown");

    QAction *h1 = new QAction(QIcon("icon:markdown/images/h1.png"),tr("Header (h1)"),this);
    actionContext->regAction(h1,"Header1","Ctrl+1");

    QAction *h2 = new QAction(QIcon("icon:markdown/images/h2.png"),tr("Header (h2)"),this);
    actionContext->regAction(h2,"Header2","Ctrl+2");

    QAction *h3 = new QAction(QIcon("icon:markdown/images/h3.png"),tr("Header (h3)"),this);
    actionContext->regAction(h3,"Header3","Ctrl+3");

    QAction *h4 = new QAction(QIcon("icon:markdown/images/h4.png"),tr("Header (h4)"),this);
    actionContext->regAction(h4,"Header4","Ctrl+4");

    QAction *h5 = new QAction(QIcon("icon:markdown/images/h5.png"),tr("Header (h5)"),this);
    actionContext->regAction(h5,"Header5","Ctrl+5");

    QAction *h6 = new QAction(QIcon("icon:markdown/images/h6.png"),tr("Header (h6)"),this);
    actionContext->regAction(h6,"Header6","Ctrl+6");

    QAction *bold = new QAction(QIcon("icon:markdown/images/bold.png"),tr("Bold"),this);
    actionContext->regAction(bold,"Bold",QKeySequence::Bold);

    QAction *italic = new QAction(QIcon("icon:markdown/images/italic.png"),tr("Italic"),this);
    actionContext->regAction(italic,"Italic",QKeySequence::Italic);

    QAction *code = new QAction(QIcon("icon:markdown/images/code.png"),tr("Inline Code"),this);
    actionContext->regAction(code,"InlineCode","Ctrl+K");

    QAction *link = new QAction(QIcon("icon:markdown/images/link.png"),tr("Link"),this);
    actionContext->regAction(link,"Link","Ctrl+Shift+L");

    QAction *image = new QAction(QIcon("icon:markdown/images/image.png"),tr("Image"),this);
    actionContext->regAction(image,"Image","Ctrl+Shift+I");

    QAction *ul = new QAction(QIcon("icon:markdown/images/ul.png"),tr("Unordered List"),this);
    actionContext->regAction(ul,"UnorderedList","Ctrl+Shift+U");

    QAction *ol = new QAction(QIcon("icon:markdown/images/ol.png"),tr("Ordered List"),this);
    actionContext->regAction(ol,"OrderedList","Ctrl+Shift+O");

    QAction *bq = new QAction(QIcon("icon:markdown/images/quote.png"),tr("Blockquote"),this);
    actionContext->regAction(bq,"Blockquote","Ctrl+Shift+Q");

    QAction *hr = new QAction(QIcon("icon:markdown/images/hr.png"),tr("Horizontal Rule"),this);
    actionContext->regAction(hr,"HorizontalRule","Ctrl+Shift+H");

    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        QMenu *h = menu->addMenu(tr("Heading"));
        h->addAction(h1);
        h->addAction(h2);
        h->addAction(h3);
        h->addAction(h4);
        h->addAction(h5);
        h->addAction(h6);
        menu->addSeparator();
        menu->addAction(link);
        menu->addAction(image);
        menu->addSeparator();
        menu->addAction(bold);
        menu->addAction(italic);
        menu->addAction(code);
        menu->addSeparator();
        menu->addAction(ul);
        menu->addAction(ol);
        menu->addSeparator();
        menu->addAction(bq);
        menu->addAction(hr);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(link);
        menu->addAction(image);
        menu->addSeparator();
        menu->addAction(bold);
        menu->addAction(italic);
        menu->addAction(code);
        menu->addSeparator();
        menu->addAction(ul);
        menu->addAction(ol);
        menu->addSeparator();
        menu->addAction(bq);
        menu->addAction(hr);
    }

    QToolBar *toolBar = LiteApi::getEditToolBar(editor);
    if (toolBar) {
        toolBar->addSeparator();
        toolBar->addAction(h1);
        toolBar->addAction(h2);
        toolBar->addAction(h3);
        toolBar->addSeparator();
        toolBar->addAction(link);
        toolBar->addAction(image);
        toolBar->addSeparator();
        toolBar->addAction(bold);
        toolBar->addAction(italic);
        toolBar->addAction(code);
        toolBar->addSeparator();
        toolBar->addAction(ul);
        toolBar->addAction(ol);
        toolBar->addSeparator();
        toolBar->addAction(bq);
        toolBar->addAction(hr);
        //updateToolTip(toolBar);
    }

    connect(editor,SIGNAL(destroyed()),this,SLOT(deleteLater()));
    connect(h1,SIGNAL(triggered()),this,SLOT(h1()));
    connect(h2,SIGNAL(triggered()),this,SLOT(h2()));
    connect(h3,SIGNAL(triggered()),this,SLOT(h3()));
    connect(h4,SIGNAL(triggered()),this,SLOT(h4()));
    connect(h5,SIGNAL(triggered()),this,SLOT(h5()));
    connect(h6,SIGNAL(triggered()),this,SLOT(h6()));
    connect(bold,SIGNAL(triggered()),this,SLOT(bold()));
    connect(italic,SIGNAL(triggered()),this,SLOT(italic()));
    connect(code,SIGNAL(triggered()),this,SLOT(code()));
    connect(link,SIGNAL(triggered()),this,SLOT(link()));
    connect(image,SIGNAL(triggered()),this,SLOT(image()));
    connect(ul,SIGNAL(triggered()),this,SLOT(ul()));
    connect(ol,SIGNAL(triggered()),this,SLOT(ol()));
    connect(bq,SIGNAL(triggered()),this,SLOT(bq()));
    connect(hr,SIGNAL(triggered()),this,SLOT(hr()));

    //m_ed->installEventFilter(this);
}

MarkdownEdit::~MarkdownEdit()
{
}



void MarkdownEdit::insert_head(const QString &tag, bool blockStart)
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
                cur.insertText(tag);
            }
            block = block.next();
        } while(block.isValid() && block.position() <= end.position());
    } else {
        if (blockStart) {
            cur.setPosition(cur.block().position());
        } else {
            QTextBlock block = cur.block();
            QString text = block.text();
            foreach(QChar c, text) {
                if (!c.isSpace()) {
                    cur.setPosition(block.position()+text.indexOf(c));
                    break;
                }
            }
        }
        cur.insertText(tag);
    }
    cur.endEditBlock();
    m_ed->setTextCursor(cur);
}

void MarkdownEdit::mark_selection(const QString &mark)
{
    mark_selection(mark,mark);
}

void MarkdownEdit::mark_selection(const QString &mark1, const QString &mark2)
{
    QTextCursor cur = m_ed->textCursor();
    cur.beginEditBlock();
    if (cur.hasSelection()) {
        QTextBlock begin = m_ed->document()->findBlock(cur.selectionStart());
        QTextBlock end = m_ed->document()->findBlock(cur.selectionEnd());
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
    m_ed->setTextCursor(cur);
}

void MarkdownEdit::h1()
{
    insert_head("# ");
}

void MarkdownEdit::h2()
{
    insert_head("## ");
}

void MarkdownEdit::h3()
{
    insert_head("### ");
}

void MarkdownEdit::h4()
{
    insert_head("#### ");
}

void MarkdownEdit::h5()
{
    insert_head("#####");
}

void MarkdownEdit::h6()
{
    insert_head("###### ");
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

void MarkdownEdit::link()
{
    QTextCursor cursor = m_ed->textCursor();
    cursor.beginEditBlock();
    if (cursor.hasSelection()) {
        int n1 = cursor.selectionStart();
        int n2 = cursor.selectionEnd();
        cursor.setPosition(n1);
        cursor.insertText("[");
        cursor.setPosition(n2+1);
        cursor.insertText("]()");
        cursor.setPosition(n2+3);
    } else {
        int n = cursor.position();
        cursor.insertText("[]()");
        cursor.setPosition(n+1);
    }
    cursor.endEditBlock();
    m_ed->setTextCursor(cursor);
}

void MarkdownEdit::image()
{
    QTextCursor cursor = m_ed->textCursor();
    cursor.beginEditBlock();
    if (cursor.hasSelection()) {
        int n1 = cursor.selectionStart();
        int n2 = cursor.selectionEnd();
        cursor.setPosition(n1);
        cursor.insertText("![");
        cursor.setPosition(n2+2);
        cursor.insertText("]()");
        cursor.setPosition(n2+4);
    } else {
        int n = cursor.position();
        cursor.insertText("![]()");
        cursor.setPosition(n+2);
    }
    cursor.endEditBlock();
    m_ed->setTextCursor(cursor);
}

void MarkdownEdit::ul()
{
    insert_head("* ",false);
}

void MarkdownEdit::ol()
{
    insert_head("1. ",false);
}

void MarkdownEdit::bq()
{
    insert_head("> ");
}

void MarkdownEdit::hr()
{
    QTextCursor cursor = m_ed->textCursor();
    if (cursor.hasSelection()) {
        cursor.setPosition(cursor.selectionEnd());
    }
    cursor.insertText("\n***\n");
    m_ed->setTextCursor(cursor);
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

bool MarkdownEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_ed && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
//            QObject::eventFilter(obj, event);
//            QTextCursor cur = m_ed->textCursor();
//            QTextBlock prev = cur.block().previous();
//            if (prev.isValid()) {
//                QString text = prev.text();
//                qDebug() << text;
//                if (text.length() > 2 && text.at(1) == ' ' && QString("*+-").contains(text.at(0))) {
//                    cur.insertText(text.left(2));
//                }
//            }
//            m_ed->setTextCursor(cur);
            return false;
        }
    }
    return QObject::eventFilter(obj, event);
}

