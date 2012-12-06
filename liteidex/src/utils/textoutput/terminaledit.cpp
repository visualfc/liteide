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
// Module: terminaledit.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: terminaledit.cpp,v 1.0 2011-8-12 visualfc Exp $

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
    QPlainTextEdit(parent)
{
    //setReadOnly(true);
    //m_lineEdit = new QLineEdit(this);
    this->setLineWrapMode(QPlainTextEdit::NoWrap);
    this->setCursorWidth(6);
    m_lastPos = 0;
    m_leftPos = 0;
    m_bPress = false;
    m_selectCursor = this->textCursor();

    m_normalFmt.setBackground(Qt::white);
    m_selectFmt.setBackground(Qt::darkCyan);

    this->mergeCurrentCharFormat(m_normalFmt);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_menu = new QMenu(this);
    m_selCopyAct = new QAction(tr("Copy"),this);
    m_selCopyAct->setShortcut(QKeySequence::Copy);
    m_pasteAct = new QAction(tr("Paste"),this);
    m_pasteAct->setShortcut(QKeySequence::Paste);
    m_selAllAct = new QAction(tr("Select All"),this);
    m_selAllAct->setShortcut(QKeySequence::SelectAll);
    m_menu->addAction(m_selCopyAct);
    m_menu->addAction(m_pasteAct);
    m_menu->addSeparator();
    m_menu->addAction(m_selAllAct);

    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuRequested(QPoint)));
    connect(m_selCopyAct,SIGNAL(triggered()),this,SLOT(selectCopy()));
    connect(m_pasteAct,SIGNAL(triggered()),this,SLOT(paste()));
    connect(m_selAllAct,SIGNAL(triggered()),this,SLOT(selectAll()));
}

void TerminalEdit::clearAll()
{
    m_lastPos = 0;
    this->clear();
}

void TerminalEdit::append(const QString &text, QTextCharFormat *fmt)
{
    QTextCursor cur = this->textCursor();
    cur.movePosition(QTextCursor::End);
    if (fmt) {
        cur.setCharFormat(*fmt);
    }
    cur.insertText(text);
    this->setTextCursor(cur);
    m_lastPos = cur.position();
}

void TerminalEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_C && e->modifiers() == Qt::CTRL) {
        this->selectCopy();
        return;
    }
    if (e->key() == Qt::Key_V && e->modifiers() == Qt::CTRL) {
        this->paste();
        return;
    }
    if (e->key() == Qt::Key_A && e->modifiers() == Qt::CTRL) {
        this->selectAll();
        return;
    }

    if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down ||
            e->key() == Qt::Key_PageDown ||
            e->key() == Qt::Key_PageUp) {
        return;
    }
    if (( e->key() == Qt::Key_Backspace ||
            e->key() == Qt::Key_Left )
        && this->textCursor().position() <= m_lastPos) {
        return;
    }
    if (e->key() == Qt::Key_Home) {
        QTextCursor cur = this->textCursor();
        cur.setPosition(m_lastPos);
        this->setTextCursor(cur);
        return;
    }

    if (e->modifiers() != Qt::NoModifier) {
        //return;
    }

    if (e->key() == Qt::Key_Enter ||
            e->key() == Qt::Key_Return) {
        QTextCursor cur = this->textCursor();
        int pos = m_lastPos;
        m_lastPos = cur.position();
        cur.setPosition(pos,QTextCursor::KeepAnchor);
        QString text = cur.selectedText();
#ifdef Q_OS_WIN
        text.append("\r\n");
#else
        text.append("\n");
#endif
        emit enterText(text);
    }

    QPlainTextEdit::keyPressEvent(e);
}

void TerminalEdit::keyReleaseEvent(QKeyEvent *e)
{
    QPlainTextEdit::keyReleaseEvent(e);
}

void TerminalEdit::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_bPress = true;
        m_leftPos = cursorForPosition(e->pos()).position();
        if (m_selectCursor.hasSelection()) {
            m_selectCursor.mergeCharFormat(m_normalFmt);
        }
        if (m_leftPos >= m_lastPos) {
            QTextCursor cur = this->textCursor();
            cur.setPosition(m_leftPos);
            this->setTextCursor(cur);
        }
    }
}

void TerminalEdit::mouseMoveEvent(QMouseEvent *e)
{
    if (m_bPress) {
        int pos = cursorForPosition(e->pos()).position();
        if (pos > m_leftPos) {
            if (m_selectCursor.hasSelection()) {
                m_selectCursor.mergeCharFormat(m_normalFmt);
            }
            m_selectCursor.setPosition(m_leftPos);
            m_selectCursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,pos-m_leftPos);
            m_selectCursor.mergeCharFormat(m_selectFmt);
        }
    }
}

void TerminalEdit::mouseReleaseEvent(QMouseEvent *e)
{
    m_bPress = false;
}

void TerminalEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (m_selectCursor.hasSelection()) {
        m_selectCursor.mergeCharFormat(m_normalFmt);
    }
    m_selectCursor = cursorForPosition(e->pos());
    m_selectCursor.select(QTextCursor::LineUnderCursor);
    m_selectCursor.mergeCharFormat(m_selectFmt);

    emit dbclickEvent(m_selectCursor);
}

void TerminalEdit::contextMenuRequested(QPoint pos)
{
    QPoint globalPos = this->mapToGlobal(pos);
    int curpos = this->cursorForPosition(pos).position();
    if (m_menu && m_menu->actions().count() > 0) {
        m_selCopyAct->setEnabled(m_selectCursor.hasSelection());
        m_pasteAct->setEnabled(this->canPaste() && curpos >= m_lastPos);
        m_menu->popup(globalPos);
    }
}

void TerminalEdit::selectCopy()
{
    QString text = m_selectCursor.selectedText();
    QClipboard *clipborad = QApplication::clipboard();
    if (clipborad) {
        clipborad->setText(text);
    }
}

void TerminalEdit::selectAll()
{
    m_selectCursor.select(QTextCursor::Document);
    m_selectCursor.mergeCharFormat(m_selectFmt);
}
