/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: liteeditorwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORWIDGET_H
#define LITEEDITORWIDGET_H

#include "liteeditorwidgetbase.h"

class LiteEditorWidget : public LiteEditorWidgetBase
{
    Q_OBJECT
public:
    explicit LiteEditorWidget(LiteApi::IApplication *app, QWidget *parent = 0);
    void setContextMenu(QMenu *contextMenu);
    void setCompleter(LiteApi::ICompleter *completer);
    void setSpellCheckZoneDontComplete(bool b) {m_bSpellCheckZoneDontComplete = b; }
    void setScrollWheelZooming(bool b) {
        m_scrollWheelZooming = b;
    }
protected:
    void contextMenuEvent(QContextMenuEvent *);
    void keyPressEvent(QKeyEvent *e);
    void inputMethodEvent(QInputMethodEvent * e);
    void focusInEvent(QFocusEvent *e);
    void wheelEvent(QWheelEvent *e);
    virtual QMimeData *createMimeDataFromSelection() const;
    virtual bool canInsertFromMimeData(const QMimeData *source) const;
    virtual void insertFromMimeData(const QMimeData *source);
public slots:
    void codeCompleter();
    void zoomIn(int range = 1);
    void zoomOut(int range = 1);
signals:
    void requestFontZoom(int);
public:
    void updateFont(const QFont &font);
    QString cursorToHtml(QTextCursor cursor) const;
signals:
    void completionPrefixChanged(QString,bool);
public:
    QString wordUnderCursor() const;
    QString textUnderCursor(QTextCursor tc) const;
    QString importUnderCursor(QTextCursor tc) const;
    virtual void dropEvent(QDropEvent *event);
protected:
    LiteApi::ICompleter *m_completer;
    QMenu      *m_contextMenu;
    bool m_scrollWheelZooming;
    bool m_bSpellCheckZoneDontComplete;
};

#endif // LITEEDITORWIDGET_H
