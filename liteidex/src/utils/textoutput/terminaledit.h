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
// Module: terminaledit.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TERMINALEDIT_H
#define TERMINALEDIT_H

#include <QPlainTextEdit>

class TerminalEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TerminalEdit(QWidget *parent = 0);
signals:
    void enterText(const QString &text);
    void dbclickEvent(const QTextCursor &cur);
public slots:
    void append(const QString &text, QTextCharFormat *fmt = 0);
    void clear();
    void contextMenuRequested(const QPoint &pt);
    void cursorPositionChanged();
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
    int m_endPostion;
    QMenu *m_contextMenu;
    QMenu *m_contextRoMenu;
    QAction *m_cut;
    QAction *m_copy;
    QAction *m_paste;
    QAction *m_selectAll;
    QAction *m_clear;
    bool    m_bFocusOut;
};


#endif // TERMINALEDIT_H
