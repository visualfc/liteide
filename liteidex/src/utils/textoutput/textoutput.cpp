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
#include "colorstyle/colorstyle.h"
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

TextOutput::TextOutput(LiteApi::IApplication *app, bool readOnly, QWidget *parent) :
    TerminalEdit(parent),
    m_liteApp(app)
{
    this->setReadOnly(readOnly);

    m_fmt = this->currentCharFormat();
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));
}

void TextOutput::append(const QString &text)
{
    m_fmt.setForeground(m_clrText);
    TerminalEdit::append(text,&m_fmt);
}

void TextOutput::append(const QString &text,const QBrush &foreground)
{
    m_fmt.setForeground(foreground);
    TerminalEdit::append(text,&m_fmt);
}

void TextOutput::appendTag(const QString &text, bool error)
{
    m_fmt.setFontWeight(QFont::Bold);
    if (error) {
        m_fmt.setForeground(m_clrError);
    } else {
        m_fmt.setForeground(m_clrTag);
    }
    TerminalEdit::append(text,&m_fmt);
}

void TextOutput::updateExistsTextColor()
{
    QColor color = m_clrText;
    color.setAlpha(128);
    m_fmt.setForeground(color);
    QTextCursor cur = this->textCursor();
    cur.select(QTextCursor::Document);
    cur.setCharFormat(m_fmt);
}

void TextOutput::setMaxLine(int max)
{
    this->setMaximumBlockCount(max);;
}

void TextOutput::appLoaded()
{
    connect(m_liteApp->editorManager(),SIGNAL(colorStyleSchemeChanged()),this,SLOT(loadColorStyleScheme()));
    this->loadColorStyleScheme();
}

void TextOutput::loadColorStyleScheme()
{
    const ColorStyleScheme *colorScheme = m_liteApp->editorManager()->colorStyleScheme();
    const ColorStyle *text = colorScheme->findStyle("Text");
    const ColorStyle *selection = colorScheme->findStyle("Selection");
    const ColorStyle *keyword = colorScheme->findStyle("Keyword");
    const ColorStyle *error = colorScheme->findStyle("Error");

    QPalette p = this->palette();
    if (text) {
        if (text->foregound().isValid()) {
            p.setColor(QPalette::Text,text->foregound());
            p.setColor(QPalette::Foreground, text->foregound());
        }
        if (text->background().isValid()) {
            p.setColor(QPalette::Base, text->background());
        }
    }
    if (selection) {
        if (selection->foregound().isValid()) {
            p.setColor(QPalette::HighlightedText, selection->foregound());
        }
        if (selection->background().isValid()) {
            p.setColor(QPalette::Highlight, selection->background());
        }
        p.setBrush(QPalette::Inactive, QPalette::Highlight, p.highlight());
        p.setBrush(QPalette::Inactive, QPalette::HighlightedText, p.highlightedText());
    }

    QString sheet = QString("QPlainTextEdit{color:%1;background-color:%2;selection-color:%3;selection-background-color:%4;}")
                .arg(p.text().color().name())
                .arg(p.base().color().name())
                .arg(p.highlightedText().color().name())
                .arg(p.highlight().color().name());
    this->setPalette(p);
    this->setStyleSheet(sheet);

    m_clrText = p.text().color();

    m_fmt.setForeground(p.text().color());
    m_fmt.setBackground(p.base().color());

    if (keyword && keyword->foregound().isValid()) {
        m_clrTag = keyword->foregound();
    } else {
        m_clrTag = Qt::darkBlue;
    }
    if (error && error->foregound().isValid()) {
        m_clrError = error->foregound();
    } else {
        m_clrError = Qt::red;
    }
    this->updateExistsTextColor();
}
