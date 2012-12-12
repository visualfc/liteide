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
// Module: markdownedit.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MARKDOWNEDIT_H
#define MARKDOWNEDIT_H

#include "liteapi/liteapi.h"

class MarkdownEdit : public QObject
{
    Q_OBJECT
public:
    explicit MarkdownEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent = 0);
    virtual ~MarkdownEdit();
signals:
    
public slots:
    void insert_head(const QString &tag);
    void mark_selection(const QString &mark);
    void mark_selection(const QString &mark1, const QString &mark2);
    void h1();
    void h2();
    void h3();
    void h4();
    void h5();
    void h6();
    void bold();
    void italic();
    void code();
    void link();
    void image();
    void ul();
    void ol();
    void bq();
    void hr();
    void gotoLine(int line, int col);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
protected:
    LiteApi::IApplication *m_liteApp;
    LiteApi::ITextEditor  *m_editor;
    QPlainTextEdit        *m_ed;
};

#endif // MARKDOWNEDIT_H
