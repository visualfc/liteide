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
// Module: highlightermanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "highlightermanager.h"
#include "colorstyle/colorstyle.h"
#include "qtc_texteditor/syntaxhighlighter.h"
#include "memory.h"

using namespace TextEditor;

HighlighterManager::HighlighterManager(QObject *parent) :
    LiteApi::IHighlighterManager(parent)
{

}

void HighlighterManager::addFactory(LiteApi::IHighlighterFactory *factory)
{
    m_factoryList.push_front(factory);
}

void HighlighterManager::removeFactory(LiteApi::IHighlighterFactory *factory)
{
    m_factoryList.removeOne(factory);
}

QList<LiteApi::IHighlighterFactory *> HighlighterManager::factoryList() const
{
    return m_factoryList;
}

QStringList HighlighterManager::mimeTypeList() const
{
    QStringList mimeTypes;
    foreach(LiteApi::IHighlighterFactory *factory, m_factoryList) {
        mimeTypes.append(factory->mimeTypes());
    }
    mimeTypes.removeDuplicates();
    return mimeTypes;
}

LiteApi::IHighlighterFactory *HighlighterManager::findFactory(const QString &mimeType) const
{
    foreach(LiteApi::IHighlighterFactory *factory, m_factoryList) {
        if (factory->mimeTypes().contains(mimeType)) {
            return factory;
        }
    }
    return 0;
}

static bool setTextCharStyle(QTextCharFormat &fmt, const QString &name, const ColorStyleScheme *scheme)
{
    const ColorStyle *style = scheme->findStyle(name);
    if (!style) {
        return false;
    }
    QColor fore = style->foregound();
    if (fore.isValid()) {
        fmt.setForeground(fore);
    }
    QColor back = style->background();
    if (back.isValid()) {
        fmt.setBackground(back);
    }
    fmt.setFontItalic(style->italic());
    if (style->bold()) {
        fmt.setFontWeight(QFont::Bold);
    } else {
        fmt.setFontWeight(QFont::Normal);
    }
    return true;
}


void HighlighterManager::setColorStyle(TextEditor::SyntaxHighlighter *highlighter, const ColorStyleScheme *scheme)
{
    QTextCharFormat fmt_nm;
    highlighter->configureFormat(SyntaxHighlighter::Normal, fmt_nm);

    QTextCharFormat fmt_vw;
    if (!setTextCharStyle(fmt_vw,"VisualWhitespace",scheme)) {
        fmt_vw.setForeground(Qt::lightGray);
    }
    highlighter->configureFormat(SyntaxHighlighter::VisualWhitespace, fmt_vw);

    QTextCharFormat fmt_kw;
    if (!setTextCharStyle(fmt_kw,"Keyword",scheme)) {
        fmt_kw.setForeground(Qt::darkBlue);
        fmt_kw.setFontWeight(QFont::Bold);
    }
    highlighter->configureFormat(SyntaxHighlighter::Keyword, fmt_kw);

    QTextCharFormat fmt_dt;
    if (!setTextCharStyle(fmt_dt,"DataType",scheme)) {
        fmt_dt.setForeground(Qt::darkBlue);//Qt::darkMagenta);
    }
    highlighter->configureFormat(SyntaxHighlighter::DataType, fmt_dt);

    QTextCharFormat fmt_fn;
    if (!setTextCharStyle(fmt_fn,"Function",scheme)) {
        fmt_fn.setForeground(Qt::blue);
    }
    highlighter->configureFormat(SyntaxHighlighter::Function,fmt_fn);

    QTextCharFormat fmt_cmn;
    if (!setTextCharStyle(fmt_cmn,"Comment",scheme)) {
        fmt_cmn.setForeground(Qt::darkGreen);
    }
    highlighter->configureFormat(SyntaxHighlighter::Comment, fmt_cmn);

    QTextCharFormat fmt_dd;
    if (!setTextCharStyle(fmt_dd,"Decimal",scheme)) {
        fmt_dd.setForeground(Qt::darkMagenta);
    }
    // Using C_NUMBER for all kinds of numbers.
    highlighter->configureFormat(SyntaxHighlighter::Decimal, fmt_dd);

    QTextCharFormat fmt_db;
    if (!setTextCharStyle(fmt_db,"BaseN",scheme)) {
        fmt_db.setForeground(Qt::darkMagenta);
    }
    highlighter->configureFormat(SyntaxHighlighter::BaseN, fmt_db);

    QTextCharFormat fmt_df;
    if (!setTextCharStyle(fmt_df,"Float",scheme)) {
        fmt_df.setForeground(Qt::darkMagenta);
    }
    highlighter->configureFormat(SyntaxHighlighter::Float, fmt_df);


    QTextCharFormat fmt_ch;
    if (!setTextCharStyle(fmt_ch,"Char",scheme)) {
        fmt_ch.setForeground(Qt::darkGreen);
    }
    // Using C_STRING for strings and chars.
    highlighter->configureFormat(SyntaxHighlighter::Char, fmt_ch);

    QTextCharFormat fmt_cs;
    if (!setTextCharStyle(fmt_cs,"String",scheme)) {
        fmt_cs.setForeground(Qt::darkGreen);
    }
    highlighter->configureFormat(SyntaxHighlighter::String, fmt_cs);

    QTextCharFormat fmt_rm;
    if (!setTextCharStyle(fmt_rm,"RegionMarker",scheme)) {
        fmt_rm.setForeground(Qt::yellow);
    }
    highlighter->configureFormat(SyntaxHighlighter::RegionMarker,fmt_rm);

    QTextCharFormat fmt_alert;
    if (!setTextCharStyle(fmt_alert,"Alert",scheme)) {
        fmt_alert.setForeground(Qt::red);
    }
    highlighter->configureFormat(SyntaxHighlighter::Alert,fmt_alert);

    QTextCharFormat fmt_err;
    if (!setTextCharStyle(fmt_err,"Error",scheme)) {
        fmt_err.setForeground(Qt::red);
    }
    highlighter->configureFormat(SyntaxHighlighter::Error,fmt_err);

    QTextCharFormat fmt_sym;
    if (!setTextCharStyle(fmt_sym,"Symbol",scheme)) {
        fmt_sym.setForeground(Qt::red);
    }
    highlighter->configureFormat(SyntaxHighlighter::Symbol,fmt_sym);

    QTextCharFormat fmt_bf;
    if (!setTextCharStyle(fmt_bf,"BuiltinFunc",scheme)) {
        fmt_bf.setForeground(Qt::blue);
    }
    highlighter->configureFormat(SyntaxHighlighter::BuiltinFunc,fmt_bf);

    QTextCharFormat fmt_pre;
    if (!setTextCharStyle(fmt_pre,"Predeclared",scheme)) {
        fmt_pre.setForeground(Qt::blue);
    }
    highlighter->configureFormat(SyntaxHighlighter::Predeclared,fmt_pre);

    QTextCharFormat fmt_fc;
    if (!setTextCharStyle(fmt_fc,"FuncDecl",scheme)) {
        fmt_fc.setForeground(Qt::blue);
    }
    highlighter->configureFormat(SyntaxHighlighter::FuncDecl,fmt_fc);

    QTextCharFormat fmt_plc;
    if (!setTextCharStyle(fmt_plc,"Placeholder",scheme)) {
        fmt_plc.setForeground(fmt_cs.foreground());
    }
    highlighter->configureFormat(SyntaxHighlighter::Placeholder,fmt_plc);

    QTextCharFormat fmt_todo;
    if (!setTextCharStyle(fmt_todo,"ToDo",scheme)) {
        fmt_todo.setForeground(fmt_cmn.foreground());
    }
    highlighter->configureFormat(SyntaxHighlighter::ToDo,fmt_todo);

    QTextCharFormat fmt_processer;
    if (!setTextCharStyle(fmt_processer,"PreprocessorFormat",scheme)) {
        fmt_processer.setForeground(fmt_cmn.foreground());
    }
    highlighter->configureFormat(SyntaxHighlighter::PreprocessorFormat,fmt_processer);


    highlighter->rehighlight();
}

void HighlighterManager::setTabSize(TextEditor::SyntaxHighlighter *highlighter, int tabSize)
{
    if (!highlighter) {
        return;
    }
    highlighter->setTabSize(tabSize);
}
