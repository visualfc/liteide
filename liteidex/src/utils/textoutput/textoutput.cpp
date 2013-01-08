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
// Module: textoutput.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "textoutput.h"

#include <QTextCharFormat>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTextCursor>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

TextOutput::TextOutput(bool readOnly, QWidget *parent) :
    TerminalEdit(parent)
{
    this->setReadOnly(readOnly);
    m_fmt = this->currentCharFormat();
}

void TextOutput::append(const QString &text)
{
    TerminalEdit::append(text,&m_fmt);
}

void TextOutput::append(const QString &text,const QBrush &foreground)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(foreground);
    TerminalEdit::append(text,&f);
}

void TextOutput::appendTag0(const QString &text, bool error)
{
    QTextCharFormat f = m_fmt;
    f.setFontWeight(QFont::Bold);
    if (error) {
        f.setForeground(Qt::red);
    } else {
        f.setForeground(Qt::darkBlue);
    }
    TerminalEdit::append(text,&f);
}

void TextOutput::appendTag1(const QString &text, bool error)
{
    QTextCharFormat f = m_fmt;
    f.setFontWeight(QFont::Bold);
    if (error) {
        f.setForeground(Qt::red);
    } else {
        f.setForeground(Qt::black);
    }
    TerminalEdit::append(text,&f);
}

void TextOutput::updateExistsTextColor(const QBrush &foreground)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(foreground);
    QTextCursor all = this->textCursor();
    all.select(QTextCursor::Document);
    all.setCharFormat(f);
}

void TextOutput::setMaxLine(int max)
{
    this->setMaximumBlockCount(max);;
}
