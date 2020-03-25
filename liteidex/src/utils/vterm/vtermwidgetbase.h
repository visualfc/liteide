/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2020 LiteIDE. All rights reserved.
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
// Module: vtermwidgetbase.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef VTERMWIDGETBASE_H
#define VTERMWIDGETBASE_H

#include <QAbstractScrollArea>
#include <QBasicTimer>
#include <QDebug>

extern "C" {
#include "libvterm/include/vterm.h"
}

typedef struct
{
    int row;
    int col;
    bool visible;
    bool blink;
    int shape;
} QVTermCursor;

typedef struct {
  int cols;
  QVector<VTermScreenCell> cells;
  QString text;
} ScrollbackLine;

class VTermWidgetBase : public QAbstractScrollArea
{
    Q_OBJECT
public:
    VTermWidgetBase(int rows, int cols, QWidget *parent);
    virtual ~VTermWidgetBase();
    void setFont(const QFont &fnt);
    void setTermSize(int rows, int cols);
    void inputWrite(const QByteArray &data);
    void setDarkMode(bool b);
    bool isDarkMode() const;
public:
    int vterm_damage(VTermRect rect);
    int vterm_moverect(VTermRect dest, VTermRect src);
    int vterm_movecursor(VTermPos pos, VTermPos oldpos, int visible);
    int vterm_settermprop(VTermProp prop, VTermValue *val);
    int vterm_bell();
    int vterm_resize(int rows, int cols);
    int vterm_sb_pushline(int cols, const VTermScreenCell *cells);
    int vterm_sb_popline(int cols, VTermScreenCell *cells);
    void setPaletteColor(int index, uint8_t r, uint8_t g, uint8_t b);
    QRect mapVTermRectToRect(VTermRect rect);
    VTermRect mapRectToVTermRect(QRect rect);
public:
    QPoint mapPointToCell(QPoint pt);
    int allRowSize() const;
    int startRow() const;
    int endRow() const;
    int topVisibleRow() const;
    int scrollbackRowSize() const;
    int termRows() const;
    int termCols() const;
    QString selectedText() const;
    QRect selectedRect() const;
    void setSelection(QPoint cellStart, QPoint cellEnd);
    void setSelectionByRow(int row);
    void setSelectionUnderWord(int row, int col);
    void selectAll();
    void clearSelection();
    bool hasSelection() const;
    bool isSelection(int row, int col) const;
    bool adjustFetchCell(int row, int *col, VTermScreenCell *cell);
protected:
    void updateSelection(QPoint scenePos);
protected:
    bool fetchCell(int row, int col, VTermScreenCell *cell) const;
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void timerEvent(QTimerEvent *e);
    void inputMethodEvent(QInputMethodEvent *e);
    void resizeEvent(QResizeEvent *e);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void flushOutput();
    virtual void write_data(const char *buf, int len);
    void drawScreenCell(QPainter &p, VTermRect rect);
signals:
    void iconNameChanged(QString);
    void titleChanged(QString);
    void sizeChanged(int rows, int cols);
    void output(char *buf, int len);
    void selectionChanged();
protected:
    int m_rows;
    int m_cols;
    int  m_propMouse;
    int m_sbListCapacity;
    bool m_altScreen;
    bool m_ignoreScroll;
    bool m_darkMode;
    char textbuf[0x1fff];
    VTerm       *m_vt;
    VTermScreen *m_screen;
    VTermState  *m_state;
    VTermColor   m_defaultFg;
    VTermColor   m_defaultBg;
    QColor       m_clrSelect;
    QColor       m_clrCursor;
    QSize m_cellSize;
    QVTermCursor   m_cursor;
    QList<ScrollbackLine*> m_sbList;
    VTermScreenCell m_empytCell;
    VTermRect       m_selected;
    Qt::MouseButton  m_mouseButton;
    QRect m_selection;
    QPoint m_ptOrg;
    QPoint  m_ptOffset;
    QBasicTimer m_trippleClickTimer;
    QPoint m_trippleClickPoint;
};

VTermModifier qt_to_vtermModifier(Qt::KeyboardModifiers mod);
VTermKey qt_to_vtermKey(int key, bool keypad);


#endif // VTERMWIDGETBASE_H
