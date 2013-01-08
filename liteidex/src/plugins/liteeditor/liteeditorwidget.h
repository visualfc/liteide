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
// Module: liteeditorwidget.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LITEEDITORWIDGET_H
#define LITEEDITORWIDGET_H

#include "liteeditorwidgetbase.h"

class QCompleter;
class LiteEditorWidget : public LiteEditorWidgetBase
{
    Q_OBJECT
public:
    explicit LiteEditorWidget(QWidget *parent = 0);
    void setContextMenu(QMenu *contextMenu);
    void setCompleter(QCompleter *m_completer);
    QCompleter *completer() const;
    void setPrefixMin(int min) {m_completionPrefixMin = min; }
    int prefixMin() const{
        return m_completionPrefixMin;
    }
protected:
    void contextMenuEvent(QContextMenuEvent *);
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    void wheelEvent(QWheelEvent *e);
    virtual QMimeData *createMimeDataFromSelection() const;
public slots:
    void zoomIn(int range = 1);
    void zoomOut(int range = 1);
signals:
    void requestFontZoom(int);
public:
    QString cursorToHtml(QTextCursor cursor) const;
signals:
    void completionPrefixChanged(QString);
public:
    QString wordUnderCursor() const;
    QString textUnderCursor(QTextCursor tc) const;
protected:
    QCompleter *m_completer;
    QMenu      *m_contextMenu;
    int m_completionPrefixMin;
    bool m_scrollWheelZooming;
};

#endif // LITEEDITORWIDGET_H
