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
// Module: vtermwidgetbase.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "vtermwidgetbase.h"
#include <QFontMetrics>
#include <QPainter>
#include <QScrollBar>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QInputMethodEvent>
#include <QResizeEvent>
#include <QTextLayout>
#include <QVector>
#include <QTextFormat>
#include <QTextCharFormat>
#include <QApplication>
#include <QDebug>
#include <QImage>

QColor toQColor(VTermColor *c, int alpha = 255)
{
   return QColor(c->rgb.red, c->rgb.green, c->rgb.blue,alpha);
};

bool attrs_is_equal(VTermScreenCellAttrs *a, VTermScreenCellAttrs *b)
{
    return  a->bold == b->bold && a->italic == b->italic && a->strike == b->strike && a->underline == b->underline;
}


int vterm_damage(VTermRect rect, void *user)
{
    return static_cast<VTermWidgetBase*>(user)->vterm_damage(rect);
}

int vterm_moverect(VTermRect dest, VTermRect src, void *user)
{
    return static_cast<VTermWidgetBase*>(user)->vterm_moverect(dest,src);
}

int vterm_movecursor(VTermPos pos, VTermPos oldpos, int visible, void *user)
{
    return static_cast<VTermWidgetBase*>(user)->vterm_movecursor(pos,oldpos,visible);
}

int vterm_settermprop(VTermProp prop, VTermValue *val, void *user)
{
    return static_cast<VTermWidgetBase*>(user)->vterm_settermprop(prop,val);
}

int vterm_bell(void *user)
{
    return static_cast<VTermWidgetBase*>(user)->vterm_bell();
}

int vterm_resize(int rows, int cols, void *user)
{
    return static_cast<VTermWidgetBase*>(user)->vterm_resize(rows,cols);
}

int vterm_sb_pushline(int cols, const VTermScreenCell *cells, void *user)
{
    return static_cast<VTermWidgetBase*>(user)->vterm_sb_pushline(cols,cells);
}

int vterm_sb_popline(int cols, VTermScreenCell *cells, void *user)
{
    return static_cast<VTermWidgetBase*>(user)->vterm_sb_popline(cols,cells);
}


static VTermScreenCallbacks vterm_screen_callbacks = {
    vterm_damage,
    vterm_moverect,
    vterm_movecursor,
    vterm_settermprop,
    vterm_bell,
    vterm_resize,
    vterm_sb_pushline,
    vterm_sb_popline,
};


VTermWidgetBase::VTermWidgetBase(LiteApi::IApplication *app, int rows, int cols, const QFont &font, QWidget *parent)
    : QAbstractScrollArea(parent),
    m_liteApp(app)
{
    this->setAttribute(Qt::WA_InputMethodEnabled,true);
    this->setFont(font);

    m_sbListCapacity = 10000;
    m_rows = rows;
    m_cols = cols;
    m_lineBuf.resize(m_cols);

    m_cursor.row = -1;
    m_cursor.col = -1;
    m_cursor.visible = false;
    m_ignoreScroll = false;
    m_darkMode = false;
    m_leftButtonPress = false;

    m_vt = vterm_new(rows,cols);
    m_screen = vterm_obtain_screen(m_vt);
    m_state = vterm_obtain_state(m_vt);

    vterm_screen_set_callbacks(m_screen,&vterm_screen_callbacks,this);
    vterm_set_utf8(m_vt,1);
    vterm_screen_set_damage_merge(m_screen,  VTERM_DAMAGE_SCROLL);
    vterm_screen_enable_altscreen(m_screen,1);

//    static QColor colorTable[] = {
//        QColor(0x00,0x00,0x00), QColor(0xB2,0x18,0x18), // Black, Red
//        QColor(0x18,0xB2,0x18), QColor(0xB2,0x68,0x18), // Green, Yellow
//        QColor(0x18,0x18,0xB2), QColor(0xB2,0x18,0xB2), // Blue, Magenta
//        QColor(0x18,0xB2,0xB2), QColor(0xB2,0xB2,0xB2), // Cyan, White
//        QColor(0x68,0x68,0x68), QColor(0xFF,0x54,0x54),
//        QColor(0x54,0xFF,0x54), QColor(0xFF,0xFF,0x54),
//        QColor(0x54,0x54,0xFF), QColor(0xFF,0x54,0xFF),
//        QColor(0x54,0xFF,0xFF), QColor(0xFF,0xFF,0xFF),
//    };

//    for (int i = 0;  i < 16; i++) {
//         QColor c = colorTable[i];
//         setPaletteColor(i,c.red(),c.green(),c.blue());
//    }

    memset(&m_empytCell,0,sizeof (VTermScreenCell));
    m_empytCell.chars[0] = '\0';
    m_empytCell.width = 1;

    setDarkMode(false);

    vterm_screen_reset(m_screen, 1);

    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    m_ptOffset = QPoint(4,2);

    connect(this,SIGNAL(selectionChanged()),viewport(),SLOT(update()));
}

void VTermWidgetBase::setPaletteColor(int index, uint8_t r, uint8_t g, uint8_t b)
{
    VTermColor col;
    vterm_color_rgb(&col, r, g, b);
    vterm_state_set_palette_color(m_state, index, &col);
}

VTermWidgetBase::~VTermWidgetBase()
{
    for (int i = 0; i < m_sbList.size(); ++i) {
        delete m_sbList[i];
    }
    m_sbList.clear();
    vterm_free(m_vt);
}

int VTermWidgetBase::vterm_damage(VTermRect rect)
{
//    QRect rc = mapVTermRectToRect(rect);
//    qDebug() << "vterm_damage1"<< rect.start_row << rect.end_row << rect.start_col <<  rect.end_col;
//    rect = mapRectToVTermRect(rc);
//    qDebug() << "vterm_damage2"<< rect.start_row << rect.end_row << rect.start_col <<  rect.end_col;

    this->clearSelection();
    viewport()->update(mapVTermRectToRect(rect));
    return 1;
}

QRect VTermWidgetBase::mapVTermRectToRect(VTermRect rect)
{
    QPoint topLeft = QPoint(
            rect.start_col * m_cellSize.width(),
            rect.start_row * m_cellSize.height());
    QPoint bottomRight = QPoint(
            (rect.end_col+1) * m_cellSize.width(),
            (rect.end_row+1) * m_cellSize.height());
    topLeft += m_ptOffset;
    bottomRight += m_ptOffset;
    return  QRect(topLeft,bottomRight);
}

VTermRect VTermWidgetBase::mapRectToVTermRect(QRect rect)
{
    VTermRect rc;
    rect.translate(-m_ptOffset);
    rc.start_row = rect.top() /m_cellSize.height();
    rc.end_row = rect.bottom()/m_cellSize.height()-1;
    rc.start_col = rect.left()/m_cellSize.width();
    rc.end_col = rect.right()/m_cellSize.width()-1;
    return rc;
}

QPoint VTermWidgetBase::mapPointToCell(QPoint pt)
{
    int row = (pt.y()-m_ptOffset.y())/m_cellSize.height();
    int col = (pt.x()-m_ptOffset.x())/m_cellSize.width();
    return  QPoint(col,row+topVisibleRow());
}

int VTermWidgetBase::scrollbackRowSize() const
{
    return m_sbList.size();
}

int VTermWidgetBase::termRows() const
{
    return m_rows;
}

int VTermWidgetBase::termCols() const
{
    return m_cols;
}

int VTermWidgetBase::topVisibleRow() const
{
    return this->verticalScrollBar()->value()-m_sbList.size();
}

int VTermWidgetBase::allRowSize() const
{
    return  m_sbList.size()+m_rows;
}

int VTermWidgetBase::startRow() const
{
    return -scrollbackRowSize();
}

int VTermWidgetBase::endRow() const
{
    return  m_rows;
}

QString VTermWidgetBase::getLineText(int row, int start_col, int end_col) const
{
    VTermRect rc;
    rc.start_row = row;
    rc.end_row = row+1;
    rc.start_col = start_col;
    rc.end_col = end_col;
    size_t len = end_col-start_col;
    size_t n = _get_chars(m_screen,0, (void*)(&m_lineBuf[0]),len,rc);
    return  QString::fromUcs4(&m_lineBuf[0],n);
}

QString VTermWidgetBase::selectedText() const
{
    if (m_selection.isNull()) {
        return QString();
    }
    if (m_selection.height() == 1) {
        return getLineText(m_selection.top(),m_selection.left(),m_selection.right());
    }
    int start_row = m_selection.top();
    int end_row = m_selection.bottom();
    QStringList lines;
    lines.append(getLineText(start_row,m_selection.left(),m_cols));
    for (int row = start_row+1; row < end_row; row++) {
        lines.append(getLineText(row,0,m_cols));
    }
    lines.append(getLineText(end_row,0,m_selection.right()));
#ifdef Q_OS_WIN
    QString sep = "\r\n";
#else
    QString sep = "\n";
#endif
    return lines.join(sep);
}

QRect VTermWidgetBase::selectedRect() const
{
    return  m_selection;
}

int VTermWidgetBase::vterm_moverect(VTermRect dest, VTermRect src)
{
//    qDebug() << "vterm_moverect" << dest.start_row << dest.end_row << src.start_row << src.end_row;
    //viewport()->update();
    QRegion re;
    re += mapVTermRectToRect(dest);
    re += mapVTermRectToRect(src);
    viewport()->update(re);
    return 1;
}

int VTermWidgetBase::vterm_movecursor(VTermPos pos, VTermPos oldpos, int visible)
{
//    qDebug() << "vterm_movecursor" << pos.row << pos.col << oldpos.row << oldpos.col+1;
    m_cursor.row = pos.row;
    m_cursor.col = pos.col;
    m_cursor.visible = visible;
    QRegion re;
    VTermRect rc1 = {pos.row,pos.row,pos.col,pos.col+1};
    re += mapVTermRectToRect(rc1);
    VTermRect rc2 = {oldpos.row,oldpos.row,oldpos.col,oldpos.col+1};
    re += mapVTermRectToRect(rc2);
    viewport()->update(re);
    return 1;
}
/*
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       // number
  VTERM_PROP_MOUSE,             // number

*/

int VTermWidgetBase::vterm_settermprop(VTermProp prop, VTermValue *val)
{
    //qDebug() << "vterm_settermprop" << prop << val->number;
    switch (prop) {
    case VTERM_PROP_CURSORVISIBLE:
        m_cursor.visible = val->boolean;
        break;
    case VTERM_PROP_CURSORBLINK:
        m_cursor.blink = val->boolean;
        break;
    case VTERM_PROP_ALTSCREEN:
        m_altScreen = val->boolean;
        this->verticalScrollBar()->setEnabled(!m_altScreen);
        //this->verticalScrollBar()->setRange(0,m_altScreen ? 0 : m_sbList.size());
        break;
    case VTERM_PROP_TITLE:
        emit titleChanged(QString::fromUtf8(val->string));
        break;
    case VTERM_PROP_ICONNAME:
        emit iconNameChanged(QString::fromUtf8(val->string));
        break;
    case VTERM_PROP_REVERSE:
//        qDebug() << "VTERM_PROP_REVERSE" << val->number;
        break;
    case VTERM_PROP_CURSORSHAPE:
        m_cursor.shape = val->number;
        break;
    case VTERM_PROP_MOUSE:
        m_propMouse = val->number;
        break;
    default:
        break;
    }
    //this->viewport()->update();
    return 1;
}

int VTermWidgetBase::vterm_bell()
{
    return 1;
}

int VTermWidgetBase::vterm_resize(int rows, int cols)
{
    m_rows = rows;
    m_cols = cols;
    m_lineBuf.resize(m_cols);
    //qDebug() << "vterm_resize" << rows << cols << m_cellWidth << m_cellHeight;
    emit sizeChanged(m_rows,m_cols);
    return 1;
}

int VTermWidgetBase::vterm_sb_pushline(int cols, const VTermScreenCell *cells)
{
//    qDebug() << "vterm_sb_pushline" << cols;
    ScrollbackLine *sb = new ScrollbackLine;
    sb->cols = cols;
    sb->cells.resize(cols);
    memcpy(&sb->cells[0],cells,sizeof(cells[0])*size_t(cols));
    m_sbList.push_front(sb);
    while (m_sbList.size() > m_sbListCapacity) {
        m_sbList.pop_back();
    }

    this->verticalScrollBar()->setRange(0,m_sbList.size());
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
    return 1;
}

int VTermWidgetBase::vterm_sb_popline(int cols, VTermScreenCell *cells)
{
//    qDebug() << "vterm_sb_popline" << cols;
    if (m_sbList.isEmpty()) {
        return 0;
    }
    ScrollbackLine *sb = m_sbList.front();
    int ncells = sb->cols;
    if (ncells > cols) {
        ncells = cols;
    }
    memcpy(cells, &sb->cells[0], sizeof(cells[0]) * size_t(ncells));
    if (cols > ncells) {
        memset(&cells[ncells],0,sizeof(cells[0])*size_t(cols-ncells));
    }
    for (int i = ncells; i < cols; ++i) {
        cells[i].width = 1;
        cells[i].fg = m_defaultFg;
        cells[i].bg = m_defaultBg;
    }
    m_sbList.pop_front();
    delete sb;
    this->verticalScrollBar()->setRange(0,m_sbList.size());
    return 1;
}

bool VTermWidgetBase::fetchCell(int row, int col, VTermScreenCell *cell) const
{
    if (row < 0) {
        if (m_sbList.isEmpty()) {
            *cell = m_empytCell;
            return  false;
        }
        ScrollbackLine *sb = m_sbList.at(-row-1);
        if (col < sb->cols) {
            *cell = sb->cells[col];
        } else {
            *cell = m_empytCell;
            return false;
        }
    } else {
       vterm_screen_get_cell(m_screen,VTermPos{row,col},cell);
    }
    vterm_screen_convert_color_to_rgb(m_screen, &cell->fg);
    vterm_screen_convert_color_to_rgb(m_screen, &cell->bg);
    return true;
}

bool VTermWidgetBase::event(QEvent *e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        if (ke->key() == Qt::Key_Tab) {
            keyPressEvent(ke);
            return true;
        }
    }
    return QAbstractScrollArea::event(e);
}

void VTermWidgetBase::setFont(const QFont &fnt)
{
    QFontMetrics fm(fnt);
#ifdef Q_OS_WIN
    m_cellSize.setWidth(fm.averageCharWidth());
#else
    m_cellSize.setWidth(fm.maxWidth());
#endif
    m_cellSize.setHeight(fm.height());
    QAbstractScrollArea::setFont(fnt);
}

void VTermWidgetBase::setTermSize(int rows, int cols)
{
    m_ignoreScroll = true;
    vterm_set_size(m_vt,rows,cols);
    vterm_screen_flush_damage(m_screen);
    m_ignoreScroll = false;
    int width = rows*m_cellSize.width();
    int height = cols*m_cellSize.height();
    this->viewport()->setMinimumSize(width,height);
}

void VTermWidgetBase::inputWrite(const QByteArray &data)
{
    vterm_input_write(m_vt,data.data(),size_t(data.length()));
    vterm_screen_flush_damage(m_screen);
    //this->viewport()->update();

}

void VTermWidgetBase::inputKey(Qt::Key _key, Qt::KeyboardModifier _mod)
{
    VTermModifier mod = qt_to_vtermModifier(_mod);
    VTermKey key = qt_to_vtermKey(_key,_mod & Qt::KeypadModifier);
    if (key != VTERM_KEY_NONE) {
        if (key == VTERM_KEY_ESCAPE)
            mod = VTERM_MOD_NONE;
        vterm_keyboard_key(m_vt, key, mod);
    }
    flushOutput();
}

void VTermWidgetBase::setDarkMode(bool b)
{
    m_darkMode = b;
    if (m_darkMode) {
        vterm_color_rgb(&m_defaultBg,30,30,30);
        vterm_color_rgb(&m_defaultFg,200,200,200);
        m_clrSelect = QColor(86,86,84);
    } else {
        vterm_color_rgb(&m_defaultBg,255,255,255);
        vterm_color_rgb(&m_defaultFg,0,0,0);
        m_clrSelect = QColor(179,215,253);
    }
    m_clrCursor = toQColor(&m_defaultFg,128);

    vterm_state_set_default_colors(m_state,&m_defaultFg,&m_defaultBg);

    m_empytCell.bg = m_defaultBg;
    m_empytCell.fg = m_defaultFg;

    vterm_screen_reset(m_screen, 1);
}

bool VTermWidgetBase::isDarkMode() const
{
    return  m_darkMode;
}

void VTermWidgetBase::paintEvent(QPaintEvent *e)
{
    QPainter p(viewport());
    p.fillRect(this->rect(),toQColor(&m_defaultBg));

//    qDebug() << e->region().rectCount();
//   for (QRegion::const_iterator it=e->region().begin(); it != e->region().end(); it++) {
//       //p.fillRect(*it,toQColor(&m_defaultBg));
//       //p.setClipRect(*it);
//       VTermRect rc = qrect_to_vtermrect(*it);
//       if (rc.end_row == rc.start_row) {
//           rc.end_row++;
//       }
//       qDebug() << "update" << rc.start_row << rc.end_row << rc.start_col << rc.end_col << "-" << m_rows << m_cols;
//       drawScreenCell(p,rc);
//   }
//   return;
//    //qDebug() << "check" << this->verticalScrollBar()->value()-m_sbList.size();
    VTermRect rect;
    rect.start_row = topVisibleRow();
    rect.end_row = rect.start_row+m_rows;
    rect.start_col = 0;
    rect.end_col = m_cols;

    //qDebug() << "drawScreenCell" << rect.start_row << rect.end_row << rect.start_col << rect.end_col;
    drawScreenCell(p,rect);
}

void VTermWidgetBase::drawScreenCell(QPainter &p, VTermRect rect)
{
    QFont fnt = this->font();
    QFontMetrics fm(fnt);
    p.setPen(toQColor(&m_defaultFg));
    p.setBrush(toQColor(&m_defaultBg));


    VTermScreenCell cell;
    int xoff = m_ptOffset.x();//(this->width()-this->verticalScrollBar()->sizeHint().width() -m_cellSize.width()*m_cols)/2;
    int yoff = 1-fm.descent()+m_ptOffset.y();

    QRect cursorRect;
    QPen oldPen = p.pen();
    QBrush oldBrush = p.brush();
    for (int row = rect.start_row; row < rect.end_row; row++) {
        int x = xoff;
        int y = yoff+(row-rect.start_row)*m_cellSize.height();
        QString text;
        VTermColor last_bg = m_defaultBg;
        VTermColor last_fg = m_defaultFg;
        VTermScreenCellAttrs last_attr = m_empytCell.attrs;
        QTextLayout::FormatRange lastFR;
        QFont lastFnt = this->font();
        p.setPen(oldPen);
        p.setBrush(oldBrush);
        p.setFont(this->font());
        for (int col = rect.start_col; col < rect.end_col; col++) {
            bool b = fetchCell(row,col,&cell);
            VTermColor *bg = &cell.bg;
            VTermColor *fg = &cell.fg;
            if (cell.attrs.reverse) {
                qSwap(bg,fg);
            }
            last_fg = *fg;
            last_bg = *bg;
            QString c;
            if (!b || !cell.chars[0]) {
                text += ' ';
                cell.width = 1;
                c = ' ';
            } else {
                c = QString::fromUcs4(cell.chars);
                text += c;
            }

            QRect rc(x+col*m_cellSize.width(),y+fm.descent(),m_cellSize.width()*cell.width,m_cellSize.height());
            if (m_cursor.visible && m_cursor.row == row && m_cursor.col == col) {
                cursorRect = rc;
            }
            if (!attrs_is_equal(&last_attr,&cell.attrs)) {
                QFont fnt = this->font();
                if (cell.attrs.bold) {
                    fnt.setWeight(QFont::Bold);
                }
                if (cell.attrs.italic) {
                    fnt.setItalic(true);
                }
                if (cell.attrs.strike) {
                    fnt.setStrikeOut(true);
                }
                if (cell.attrs.underline) {
                    fnt.setUnderline(true);
                }
                p.setFont(fnt);
            }
            last_attr = cell.attrs;

            if (!vterm_color_is_equal(&m_defaultBg,bg)) {
                p.fillRect(rc,toQColor(bg));
            }
            if (!vterm_color_is_equal(&m_defaultFg,fg)) {
                p.setPen(toQColor(fg));
            } else {
                p.setPen(oldPen);
            }
            if (isSelection(row,col)) {
                p.fillRect(rc,m_clrSelect);
            }

            p.drawText(x+col*m_cellSize.width(),y+m_cellSize.height(),c);
            if (cell.width > 1) {
                col += cell.width-1;
            }
        }
    }

    if (cursorRect.isEmpty()) {
        return;
    }
//    VTERM_PROP_CURSORSHAPE_BLOCK = 1,
//    VTERM_PROP_CURSORSHAPE_UNDERLINE,
//    VTERM_PROP_CURSORSHAPE_BAR_LEFT,
    switch (m_cursor.shape) {
    case VTERM_PROP_CURSORSHAPE_BLOCK:
        break;
    case VTERM_PROP_CURSORSHAPE_UNDERLINE:
        cursorRect.setTop(cursorRect.bottom()-2);
        break;
    case VTERM_PROP_CURSORSHAPE_BAR_LEFT:
        cursorRect.setRight(cursorRect.left()+2);
        break;
    }
    //p.fillRect(cursorRect,QColor(40,40,40,128));
    if (this->hasFocus()) {
        p.fillRect(cursorRect,m_clrCursor);
    } else {
        p.setPen(m_clrCursor);
        p.drawRect(cursorRect);
    }
}

void VTermWidgetBase::keyPressEvent(QKeyEvent *e)
{
    VTermModifier mod = qt_to_vtermModifier(e->modifiers());
    VTermKey key = qt_to_vtermKey(e->key(),e->modifiers() & Qt::KeypadModifier);
    if (key != VTERM_KEY_NONE) {
        if (key == VTERM_KEY_ESCAPE)
            mod = VTERM_MOD_NONE;
        vterm_keyboard_key(m_vt, key, mod);
    } else if (e->text().length()) {
        vterm_keyboard_unichar(
                    m_vt,
                    e->text().toUcs4()[0],
                mod);
    }
    flushOutput();
}

void VTermWidgetBase::mouseMoveEvent(QMouseEvent *e)
{
//    vterm_mouse_move(m_vt,row,col,qt_to_vtermModifier(e->modifiers()));
//    this->viewport()->update();
    if (m_leftButtonPress) {
        updateSelection(e->pos());
        viewport()->update();
    }
}

void VTermWidgetBase::mousePressEvent(QMouseEvent *e)
{
//    vterm_mouse_button(m_vt,e->button(),true,qt_to_vtermModifier(e->modifiers()));
//    this->viewport()->update();
    if (e->button() == Qt::LeftButton) {
        if (m_trippleClickTimer.isActive()
                && ( (e->pos() - m_trippleClickPoint).manhattanLength() < QApplication::startDragDistance())) {
            QPoint cell = mapPointToCell(e->pos());
            setSelectionByRow(cell.y());
            m_trippleClickTimer.stop();
        } else {
            this->clearSelection();
        }
        m_ptOrg = e->pos();
        m_leftButtonPress = true;
    }
}

void VTermWidgetBase::updateSelection(QPoint scenePos)
{
    QPoint start = mapPointToCell(m_ptOrg);
    QPoint end = mapPointToCell(scenePos);
    if (start != end) {
        setSelection(start, end);
    }
}

void VTermWidgetBase::mouseReleaseEvent(QMouseEvent *e)
{
 //   vterm_mouse_button(m_vt,e->button(),false,qt_to_vtermModifier(e->modifiers()));
//    this->viewport()->update();
    if (e->button() == Qt::LeftButton) {
        this->updateSelection(e->pos());
        m_leftButtonPress = false;
    }
}

void VTermWidgetBase::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_trippleClickPoint = e->pos();
        m_trippleClickTimer.start(QApplication::doubleClickInterval(),this);
        QPoint cell = mapPointToCell(e->pos());
        setSelectionUnderWord(cell.y(),cell.x());
    }
}

void VTermWidgetBase::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_trippleClickTimer.timerId()) {
        m_trippleClickTimer.stop();
    }
}

void VTermWidgetBase::inputMethodEvent(QInputMethodEvent *e) {
    QVector<uint> str = e->commitString().toUcs4();
    foreach(uint c, str) {
        vterm_keyboard_unichar(m_vt,c,VTERM_MOD_NONE);
    }

    flushOutput();
    viewport()->update();
}

void VTermWidgetBase::resizeEvent(QResizeEvent *e)
{
    e->accept();

    // save scroll
    int rows = (e->size().height()-m_ptOffset.y()*2) /m_cellSize.height();
    int cols = (e->size().width()-this->verticalScrollBar()->sizeHint().width() - m_ptOffset.x()*2) /m_cellSize.width();
    int oldMax = this->verticalScrollBar()->maximum();
    int oldValue = this->verticalScrollBar()->value();

    m_ignoreScroll = true;
    vterm_set_size(m_vt,rows,cols);
    vterm_screen_flush_damage(m_screen);
    m_ignoreScroll = false;

    // restore scroll
    int newMax = this->verticalScrollBar()->maximum();
    if (newMax!=0 && oldMax!=0) {
        int newValue = oldValue*newMax/oldMax;
        this->verticalScrollBar()->setValue(newValue);
    }

    QAbstractScrollArea::resizeEvent(e);
}

void VTermWidgetBase::focusInEvent(QFocusEvent *e)
{
    e->accept();
    viewport()->update();
}

void VTermWidgetBase::focusOutEvent(QFocusEvent *e)
{
    e->accept();
    viewport()->update();
}

void VTermWidgetBase::flushOutput()
{
    size_t len = vterm_output_read(m_vt, textbuf,
                                     sizeof(textbuf));
    if (len > 0) {
        this->write_data(textbuf,int(len));
    }
}

void VTermWidgetBase::write_data(const char *buf, int len)
{
     qDebug() <<  "output" << QString::fromUtf8(buf,int(len));
}

void VTermWidgetBase::setSelection(QPoint cellStart, QPoint cellEnd)
{
    if (cellStart.y() > cellEnd.y())
        qSwap(cellStart, cellEnd);
    if (cellStart.y() == cellEnd.y() && cellStart.x() > cellEnd.x())
        qSwap(cellStart, cellEnd);

    if (cellStart.x() < 0)
        cellStart.rx() = 0;
    if (cellStart.y() < startRow())
        cellStart.ry() = startRow();

    if (cellEnd.x() > m_cols)
        cellEnd.rx() = m_cols;
    if (cellEnd.y() > endRow())
        cellEnd.ry() = endRow();

    if (cellStart.y() >= m_rows) {
        m_selection = QRect();
    } else {
        m_selection = QRect(cellStart, cellEnd);
    }

    m_selected.start_row = cellStart.y()+topVisibleRow();
    m_selected.start_col = cellStart.x();
    m_selected.end_col = cellStart.y();
    m_selected.end_row = cellEnd.y()+topVisibleRow();

    emit selectionChanged();
 }

void VTermWidgetBase::setSelectionByRow(int row)
{
    if (row < startRow() || row >= endRow()) {
        clearSelection();
        return;
    }
    m_selection = QRect(0,row,m_cols+1,1);
    emit selectionChanged();
}

bool VTermWidgetBase::adjustFetchCell(int row, int *col, VTermScreenCell *cell)
{
    bool b = this->fetchCell(row,*col,cell);
    if (!b) {
        return false;
    }
    if (cell->chars[0] == uint32_t(-1)) {
        bool b = this->fetchCell(row,*col-1,cell);
        if (b) {
            *col = *col-1;
            return true;
        }
    }
    return b;
}

void VTermWidgetBase::setSelectionUnderWord(int row, int col)
{
    if (row < startRow() || row >= endRow() || col < 0 || col >= m_cols) {
        clearSelection();
        return;
    }
    VTermScreenCell cell;
    this->adjustFetchCell(row,&col,&cell);
    if (!cell.chars[0]) {
        int ncol = col+1;
        for (; ncol < m_cols; ++ncol) {
            this->fetchCell(row,ncol,&cell);
            if (cell.chars[0]) {
                break;
            }
        }
        int pcol = col-1;
        for (;pcol >= 0;--pcol) {
            this->fetchCell(row,pcol,&cell);
            if (cell.chars[0]) {
                break;
            }
        }
        setSelection(QPoint(pcol+1,row),QPoint(ncol,row));
    } else {
        bool isSpace = QString::fromUcs4(cell.chars)[0].isSpace();
        int width = cell.width;
        int ncol = col+width;
        for (; ncol < m_cols;) {
            this->fetchCell(row,ncol,&cell);
            if (!cell.chars[0]) {
                break;
            }
            QChar c = QString::fromUcs4(cell.chars)[0];
            if (isSpace && !c.isSpace()) {
                break;
            } else if (!isSpace && c.isSpace()) {
                break;
            }
            ncol += cell.width;
        }
        int pcol = col-1;
        for (; pcol >= 0;--pcol) {
            this->adjustFetchCell(row,&pcol,&cell);
            if (!cell.chars[0]) {
                break;
            }
            QChar c = QString::fromUcs4(cell.chars)[0];
            if (isSpace && !c.isSpace()) {
                break;
            } else if (!isSpace && c.isSpace()) {
                break;
            }
        }
        setSelection(QPoint(pcol+cell.width,row),QPoint(ncol,row));
    }
}

void VTermWidgetBase::selectAll()
{
    m_selection = QRect(0,-scrollbackRowSize(),m_cols+1,allRowSize());
    emit selectionChanged();
}

void VTermWidgetBase::clearSelection()
{
    if (m_selection.isNull())
        return;

    m_selection = QRect();

    emit selectionChanged();
}

bool VTermWidgetBase::hasSelection() const
{
    return  !m_selection.isNull();
}

bool VTermWidgetBase::isSelection(int row, int col) const
{
    if (m_selection.isNull()) {
        return false;
    }
    if (m_selection.height() == 1) {
        if (row == m_selection.top() && col >= m_selection.left() && col < m_selection.right()) {
            return  true;
        }
     } else {
        if (m_selection.top() == row) {
            if (col >= m_selection.left()) {
                return true;
            }
        } else if (m_selection.bottom() == row) {
            if (col < m_selection.right()) {
                return true;
            }
        } else if (row > m_selection.top() && row < m_selection.bottom()) {
            return true;
        }
    }
    return false;
}


VTermModifier qt_to_vtermModifier(Qt::KeyboardModifiers mod)
{
    int ret = VTERM_MOD_NONE;

    if (mod & Qt::SHIFT)
        ret |= VTERM_MOD_SHIFT;

    if (mod & Qt::ALT)
        ret |= VTERM_MOD_ALT;

    if (mod & Qt::CTRL)
        ret |= VTERM_MOD_CTRL;

    return static_cast<VTermModifier>(ret);
}

VTermKey qt_to_vtermKey(int key, bool keypad)
{
    if (key >= Qt::Key_F1 && key <= Qt::Key_F35)
        return static_cast<VTermKey>(VTERM_KEY_FUNCTION_0 + key - Qt::Key_F1 + 1);

    switch (key) {
        case Qt::Key_Return:
            return VTERM_KEY_ENTER;
        case Qt::Key_Tab:
            return VTERM_KEY_TAB;
        case Qt::Key_Backspace:
            return VTERM_KEY_BACKSPACE;
        case Qt::Key_Escape:
            return VTERM_KEY_ESCAPE;
        case Qt::Key_Up:
            return VTERM_KEY_UP;
        case Qt::Key_Down:
            return VTERM_KEY_DOWN;
        case Qt::Key_Left:
            return VTERM_KEY_LEFT;
        case Qt::Key_Right:
            return VTERM_KEY_RIGHT;
        case Qt::Key_Insert:
            return VTERM_KEY_INS;
        case Qt::Key_Delete:
            return VTERM_KEY_DEL;
        case Qt::Key_Home:
            return VTERM_KEY_HOME;
        case Qt::Key_End:
            return VTERM_KEY_END;
        case Qt::Key_PageUp:
            return VTERM_KEY_PAGEUP;
        case Qt::Key_PageDown:
            return VTERM_KEY_PAGEDOWN;
        case Qt::Key_multiply:
            return keypad ? VTERM_KEY_KP_MULT : VTERM_KEY_NONE;
        case Qt::Key_Plus:
            return keypad ? VTERM_KEY_KP_PLUS : VTERM_KEY_NONE;
        case Qt::Key_Comma:
            return keypad ? VTERM_KEY_KP_COMMA : VTERM_KEY_NONE;
        case Qt::Key_Minus:
            return keypad ? VTERM_KEY_KP_MINUS : VTERM_KEY_NONE;
        case Qt::Key_Period:
            return keypad ? VTERM_KEY_KP_PERIOD : VTERM_KEY_NONE;
        case Qt::Key_Slash:
            return keypad ? VTERM_KEY_KP_DIVIDE : VTERM_KEY_NONE;
        case Qt::Key_Enter:
            return keypad ? VTERM_KEY_KP_ENTER : VTERM_KEY_NONE;
        case Qt::Key_Equal:
            return keypad ? VTERM_KEY_KP_EQUAL : VTERM_KEY_NONE;
        default:
            return VTERM_KEY_NONE;
    }    
}

#define UNICODE_TAB '\t'
#define UNICODE_SPACE 0x20
#define UNICODE_LINEFEED 0x0a

static inline unsigned int utf8_seqlen(long codepoint)
{
  if(codepoint < 0x0000080) return 1;
  if(codepoint < 0x0000800) return 2;
  if(codepoint < 0x0010000) return 3;
  if(codepoint < 0x0200000) return 4;
  if(codepoint < 0x4000000) return 5;
  return 6;
}

/* Does NOT NUL-terminate the buffer */
static int fill_utf8(long codepoint, char *str)
{
  int nbytes = utf8_seqlen(codepoint);

  // This is easier done backwards
  int b = nbytes;
  while(b > 1) {
    b--;
    str[b] = 0x80 | (codepoint & 0x3f);
    codepoint >>= 6;
  }

  switch(nbytes) {
    case 1: str[0] =        (codepoint & 0x7f); break;
    case 2: str[0] = 0xc0 | (codepoint & 0x1f); break;
    case 3: str[0] = 0xe0 | (codepoint & 0x0f); break;
    case 4: str[0] = 0xf0 | (codepoint & 0x07); break;
    case 5: str[0] = 0xf8 | (codepoint & 0x03); break;
    case 6: str[0] = 0xfc | (codepoint & 0x01); break;
  }

  return nbytes;
}

size_t VTermWidgetBase::_get_chars(const VTermScreen *screen, const int utf8, void *buffer, size_t len, const VTermRect rect) const
{
  size_t outpos = 0;
  int padding = 0;

#define PUT(c)                                             \
  if(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    if(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (char *)buffer + outpos);   \
    else                                                   \
      outpos += thislen;                                   \
  }                                                        \
  else {                                                   \
    if(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    else                                                   \
      outpos++;                                            \
  }
  int row,col,i;
  for(row = rect.start_row; row < rect.end_row; row++) {
    for(col = rect.start_col; col < rect.end_col; col++) {
        VTermScreenCell cell;
        fetchCell(row, col,&cell);

      if(cell.chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      else if(cell.chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      else {
        while(padding > 0) {
          PUT(UNICODE_TAB);
          padding-=8;
        }
        padding = 0;
        for(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          PUT(cell.chars[i]);
        }
      }
    }

    if(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }
  return outpos;
}
