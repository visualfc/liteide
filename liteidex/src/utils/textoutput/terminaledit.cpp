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
// Module: terminaledit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "terminaledit.h"

#include <QTextCursor>
#include <QLineEdit>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QApplication>
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

TerminalEdit::TerminalEdit(QWidget *parent) :
    QPlainTextEdit(parent), m_endPostion(0)
{
    this->setCursorWidth(4);
    this->setAcceptDrops(false);

    m_contextMenu = new QMenu(this);
    m_contextRoMenu = new QMenu(this);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    m_cut = new QAction(tr("Cut"),this);
    m_cut->setShortcut(QKeySequence::Cut);

    m_copy = new QAction(tr("Copy"),this);
    m_copy->setShortcut(QKeySequence::Copy);

    m_paste = new QAction(tr("Paste"),this);
    m_paste->setShortcut(QKeySequence::Paste);

    m_selectAll = new QAction(tr("Select All"),this);
    m_selectAll->setShortcut(QKeySequence::SelectAll);

    m_clear = new QAction(tr("Clear All"),this);

    m_contextMenu->addAction(m_cut);
    m_contextMenu->addAction(m_copy);
    m_contextMenu->addAction(m_paste);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_selectAll);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_clear);

    m_contextRoMenu->addAction(m_copy);
    m_contextRoMenu->addSeparator();
    m_contextRoMenu->addAction(m_selectAll);
    m_contextRoMenu->addSeparator();
    m_contextRoMenu->addAction(m_clear);

    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuRequested(QPoint)));
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(cursorPositionChanged()));
    connect(this,SIGNAL(selectionChanged()),this,SLOT(cursorPositionChanged()));
    connect(m_cut,SIGNAL(triggered()),this,SLOT(cut()));
    connect(m_copy,SIGNAL(triggered()),this,SLOT(copy()));
    connect(m_paste,SIGNAL(triggered()),this,SLOT(paste()));
    connect(m_selectAll,SIGNAL(triggered()),this,SLOT(selectAll()));
    connect(m_clear,SIGNAL(triggered()),this,SLOT(clear()));
}

void TerminalEdit::append(const QString &text, QTextCharFormat *fmt)
{
    setUndoRedoEnabled(false);
    QTextCursor cur = this->textCursor();
    cur.movePosition(QTextCursor::End);
    if (fmt) {
        cur.setCharFormat(*fmt);
    }
    cur.insertText(text);
    this->setTextCursor(cur);
    setUndoRedoEnabled(true);
    m_endPostion = cur.position();
}

void TerminalEdit::clear()
{
    m_endPostion = 0;
    QPlainTextEdit::clear();
}

void TerminalEdit::keyPressEvent(QKeyEvent *ke)
{
    QTextCursor cur = this->textCursor();
    int pos = cur.position();
    int end = cur.position();

    if (cur.hasSelection()) {
        pos = cur.selectionStart();
        end = cur.selectionEnd();
    }

    bool bReadOnly = pos < m_endPostion;

    if (bReadOnly && ( ke == QKeySequence::Paste || ke == QKeySequence::Cut ||
                       ke == QKeySequence::DeleteEndOfWord ||
                       ke == QKeySequence::DeleteStartOfWord)) {
        return;
    }

    if (ke == QKeySequence::DeleteStartOfWord) {
        if (!cur.hasSelection()) {
            cur.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
            if (cur.selectionStart() < m_endPostion) {
                cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,m_endPostion-cur.selectionStart());
            }
        }
        cur.removeSelectedText();
        return;
    }


    if (ke->modifiers() == Qt::NoModifier
            || ke->modifiers() == Qt::ShiftModifier
            || ke->modifiers() == Qt::KeypadModifier) {
        if (ke->key() < Qt::Key_Escape) {
            if (bReadOnly) {
                return;
            }
        } else {
            if (ke->key() == Qt::Key_Backspace) {
                if (cur.hasSelection()) {
                    if (bReadOnly) {
                        return;
                    }
                } else if (pos <= m_endPostion) {
                    return;
                }
            } else if (bReadOnly && (
                           ke->key() == Qt::Key_Delete ||
                           ke->key() == Qt::Key_Tab ||
                           ke->key() == Qt::Key_Backtab ||
                           ke->key() == Qt::Key_Return ||
                           ke->key() == Qt::Key_Enter)) {
                return;
            }
            if (ke->key() == Qt::Key_Return ||
                    ke->key() == Qt::Key_Enter) {
                QPlainTextEdit::keyPressEvent(ke);
                cur.setPosition(end,QTextCursor::MoveAnchor);
                cur.setPosition(m_endPostion,QTextCursor::KeepAnchor);
#ifdef Q_OS_WIN
                emit enterText(cur.selectedText()+"\r\n");
#else
                emit enterText(cur.selectedText()+"\n");
#endif
                return;
            }
        }
    }
    QPlainTextEdit::keyPressEvent(ke);
}

void TerminalEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    QPlainTextEdit::mouseDoubleClickEvent(e);
    QTextCursor cur = cursorForPosition(e->pos());
    emit dbclickEvent(cur);
}

void TerminalEdit::mousePressEvent(QMouseEvent *e)
{
    QPlainTextEdit::mousePressEvent(e);
    if (!this->isReadOnly() && m_bFocusOut) {
        m_bFocusOut = false;
        QTextCursor cur = this->textCursor();
        if (!cur.hasSelection()) {
            cur.movePosition(QTextCursor::End);
            this->setTextCursor(cur);
        }
    }
}

void TerminalEdit::focusOutEvent(QFocusEvent *e)
{
    QPlainTextEdit::focusOutEvent(e);
    m_bFocusOut = true;
}

void TerminalEdit::focusInEvent(QFocusEvent *e)
{
    QPlainTextEdit::focusInEvent(e);
    if (!this->isReadOnly()) {
        QTextCursor cur = this->textCursor();
        if (!cur.hasSelection()) {
            cur.movePosition(QTextCursor::End);
            this->setTextCursor(cur);
        }
    }
}

void TerminalEdit::contextMenuRequested(const QPoint &pt)
{
    QPoint globalPos = this->mapToGlobal(pt);
    if (isReadOnly()) {
        m_contextRoMenu->popup(globalPos);
    } else {
        m_contextMenu->popup(globalPos);
    }
}

void TerminalEdit::cursorPositionChanged()
{
    QTextCursor cur = this->textCursor();
    int pos = cur.position();
    if (cur.hasSelection()) {
        pos = cur.selectionStart();
        m_copy->setEnabled(true);
        if (pos < m_endPostion) {
            m_cut->setEnabled(false);
        } else {
            m_cut->setEnabled(!this->isReadOnly());
        }
    } else {
        m_copy->setEnabled(false);
        m_cut->setEnabled(false);
    }
    if (pos < m_endPostion) {
        m_paste->setEnabled(false);
    } else {
        QClipboard *clipboard = QApplication::clipboard();
        if (clipboard->mimeData()->hasText() ||
                clipboard->mimeData()->hasHtml()){
            m_paste->setEnabled(true);
        } else {
            m_paste->setEnabled(false);
        }
    }
}
