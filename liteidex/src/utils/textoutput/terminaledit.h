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
// Module: terminaledit.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: terminaledit.h,v 1.0 2011-8-12 visualfc Exp $

#ifndef TERMINALEDIT_H
#define TERMINALEDIT_H

#include <QPlainTextEdit>
#include <QTextCharFormat>

class QLineEdit;
class TerminalEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TerminalEdit(QWidget *parent = 0);
    void append(const QString &text, QTextCharFormat *fmt = 0);
    void clearAll();
signals:
    void enterText(const QString &text);
    void dbclickEvent(const QTextCursor &cur);
public slots:
    void contextMenuRequested(QPoint);
    void selectCopy();
    void selectAll();
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
protected:
    QLineEdit *m_lineEdit;
    QMenu     *m_menu;
    QAction   *m_selCopyAct;
    QAction   *m_pasteAct;
    QAction   *m_selAllAct;
    int        m_lastPos;
    bool       m_bPress;
    int        m_leftPos;
    QTextCursor m_selectCursor;
    QTextCharFormat m_normalFmt;
    QTextCharFormat m_selectFmt;
};

#endif // TERMINALEDIT_H
