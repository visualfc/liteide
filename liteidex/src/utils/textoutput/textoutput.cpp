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
// Module: textoutput.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "textoutput.h"
#include "colorstyle/colorstyle.h"
#include "../liteapp/liteapp_global.h"
#include <QTextCharFormat>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextBlock>
#include <QElapsedTimer>
#include <QTime>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

static void fadeText(QTextCursor& cur) {
    QTextCharFormat f(cur.charFormat());
    QColor color(f.foreground().color());
    color.setAlpha(160);
    f.setForeground(color);
    cur.setCharFormat(f);
}

TextOutput::TextOutput(LiteApi::IApplication *app, bool readOnly, QWidget *parent) :
    TerminalEdit(parent),
    m_liteApp(app)
{
    this->setReadOnly(readOnly);

    m_fmt = this->currentCharFormat();
    m_defPalette = this->palette();
    m_clrText = m_defPalette.foreground().color();
    m_clrTag = Qt::darkBlue;
    m_clrError = Qt::red;
    m_existsTimer.start();

    connect(m_liteApp->editorManager(),SIGNAL(colorStyleSchemeChanged()),this,SLOT(loadColorStyleScheme()));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));

    this->applyOption(OPTION_LITEOUTPUT);
}

void TextOutput::append(const QString &text)
{
    append(text, m_clrText);
}

void TextOutput::append(const QString &text,const QBrush &foreground)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(foreground);
    appendAndReset(text, f);
}

void TextOutput::appendTag(const QString &text, bool error, bool time)
{
    QTextCharFormat f = m_fmt;
    f.setFontWeight(QFont::Bold);
    if (error) {
        f.setForeground(m_clrError);
    } else {
        f.setForeground(m_clrTag);
    }    
    if (time) {
        appendAndReset(QTime::currentTime().toString("hh:mm:ss: ")+text, f);
    } else {
        appendAndReset(text, f);
    }
}

void TextOutput::appendAndReset(const QString &text, QTextCharFormat& f)
{
    TerminalEdit::append(text, &f);
    m_existsTimer.restart();
}

void TextOutput::updateExistsTextColor(bool force)
{
    if (!m_existsTimer.hasExpired(2500) && !force) return;

    QTextDocument* doc = document();
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        QTextCursor cur(it);
        cur.select(QTextCursor::BlockUnderCursor);
        fadeText(cur);
    }
}

void TextOutput::setMaxLine(int max)
{
    this->setMaximumBlockCount(max);;
}

void TextOutput::loadColorStyleScheme()
{
    bool useColorShceme = m_liteApp->settings()->value(OUTPUT_USECOLORSCHEME,true).toBool();

    const ColorStyleScheme *colorScheme = m_liteApp->editorManager()->colorStyleScheme();
    const ColorStyle *text = colorScheme->findStyle("Text");
    const ColorStyle *selection = colorScheme->findStyle("Selection");
    const ColorStyle *keyword = colorScheme->findStyle("Keyword");
    const ColorStyle *error = colorScheme->findStyle("Error");

    QPalette p = this->m_defPalette;
    if (useColorShceme && text) {
        if (text->foregound().isValid()) {
            p.setColor(QPalette::Text,text->foregound());
            p.setColor(QPalette::Foreground, text->foregound());
        }
        if (text->background().isValid()) {
            p.setColor(QPalette::Base, text->background());
        }
    }
    if (useColorShceme && selection) {
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

    if (useColorShceme && keyword && keyword->foregound().isValid()) {
        m_clrTag = keyword->foregound();
    } else {
        m_clrTag = Qt::darkBlue;
    }
    if (useColorShceme && error && error->foregound().isValid()) {
        m_clrError = error->foregound();
    } else {
        m_clrError = Qt::red;
    }

    QTextCursor cur(document());
    cur.select(QTextCursor::Document);
    cur.setCharFormat(m_fmt);
    fadeText(cur);
}

void TextOutput::applyOption(QString opt)
{
    if (opt != OPTION_LITEOUTPUT) {
        return;
    }
    QString fontFamily = m_liteApp->settings()->value(OUTPUT_FAMILY).toString();
    int fontSize = m_liteApp->settings()->value(OUTPUT_FONTSIZE,12).toInt();

    int fontZoom = m_liteApp->settings()->value(OUTPUT_FONTZOOM,100).toInt();

    bool antialias = m_liteApp->settings()->value(OUTPUT_ANTIALIAS,true).toBool();

    bool lineWrap = m_liteApp->settings()->value(OUTPUT_LINEWRAP,false).toBool();

    QFont font = this->font();
    if (!fontFamily.isEmpty()) {
        font.setFamily(fontFamily);
    }

    font.setPointSize(fontSize*fontZoom/100.0);
    if (antialias) {
        font.setStyleStrategy(QFont::PreferAntialias);
    } else {
        font.setStyleStrategy(QFont::NoAntialias);
    }
    this->setFont(font);

    this->setLineWrapMode(lineWrap ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);

    this->loadColorStyleScheme();
}
