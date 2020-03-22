#include "vtermwidgetbase.h"
#include <QFontMetrics>
#include <QPainter>
#include <QScrollBar>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QInputMethodEvent>
#include <QResizeEvent>
#include <QTextLayout>
#include <QVector>
#include <QTextFormat>
#include <QTextCharFormat>
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


VTermWidgetBase::VTermWidgetBase(int rows, int cols, QWidget *parent)
    : QAbstractScrollArea(parent)
{
    this->setAttribute(Qt::WA_InputMethodEnabled,true);
#ifdef Q_OS_LINUX
    setFont(QFont("DejaVu Sans Mono",11));
#else
    setFont(QFont("Menlo",12));
#endif
    m_sbListCapacity = 10000;
    m_rows = rows;
    m_cols = cols;

    m_cursor.row = -1;
    m_cursor.col = -1;
    m_cursor.visible = false;
    m_ignoreScroll = false;
    m_darkMode = false;

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
    //QRect rc = vtermrect_to_qrect(rect);
//    qDebug() << "vterm_damage1"<< rect.start_row << rect.end_row << rect.start_col <<  rect.end_col;
    //rect = qrect_to_vtermrect(rc);
//    qDebug() << "vterm_damage"<< rect.start_row << rect.end_row << rect.start_col <<  rect.end_col;

    viewport()->update(vtermrect_to_qrect(rect));
    this->clearSelection();
    return 1;
}

QRect VTermWidgetBase::vtermrect_to_qrect(VTermRect rect)
{
    QPoint topLeft = QPoint(
            rect.start_col * m_cellSize.width(),
            rect.start_row * m_cellSize.height());
    QPoint bottomRight = QPoint(
            (rect.end_col+1) * m_cellSize.width(),
            (rect.end_row+1) * m_cellSize.height());
    return  QRect(topLeft,bottomRight);
}

VTermRect VTermWidgetBase::qrect_to_vtermrect(QRect rect)
{
    VTermRect rc;
    rc.start_row = rect.top()/m_cellSize.height();
    rc.end_row = rect.bottom()/m_cellSize.height();
    rc.start_col = rect.left()/m_cellSize.width();
    rc.end_col = rect.right()/m_cellSize.width();
    return rc;
}

QPoint VTermWidgetBase::mapPointToCell(QPoint pt)
{
    int row = pt.y()/m_cellSize.height();
    int col = pt.x()/m_cellSize.width();
    return  QPoint(col,row);
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

QString VTermWidgetBase::selectedText() const
{
    if (m_selection.isNull()) {
        return QString();
    }

    int start_row = m_selection.top();//+this->verticalScrollBar()->value()-m_sbList.size();
    QString text;
    VTermScreenCell cell;
    if (m_selection.height() == 1) {
        for (int col = m_selection.left(); col != m_selection.right() ; ++col) {
            bool b = fetchCell(start_row,col,&cell);
            if (!b || !cell.chars[0]) {
                break;
            }
            text += QString::fromUcs4(cell.chars);
            if (cell.width > 1) {
                col += cell.width-1;
            }
        }
    } else {
        int end_row = start_row+m_selection.height();
        for (int row = start_row; row != end_row; ++row) {
            int start_col = 0;
            int end_col = m_cols;
            if (row == start_row) {
                start_col = m_selection.left();
            } else {
#ifdef Q_OS_WIN
                text += "\r\n";
#else
                text += "\n";
#endif
                if (row == end_row-1) {
                    end_col = m_selection.right();
                }
            }
            for (int col = start_col; col < end_col; ++col) {
                bool b = fetchCell(row,col,&cell);
                if (!b || !cell.chars[0]) {
                    break;
                }
                text += QString::fromUcs4(cell.chars);
                if (cell.width > 1) {
                    col += cell.width-1;
                }
            }
        }
    }

    return  text;
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
    re += vtermrect_to_qrect(dest);
    re += vtermrect_to_qrect(src);
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
    re += vtermrect_to_qrect(rc1);
    VTermRect rc2 = {oldpos.row,oldpos.row,oldpos.col,oldpos.col+1};
    re += vtermrect_to_qrect(rc2);
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

void VTermWidgetBase::setFont(const QFont &fnt)
{
    QFontMetrics fm(fnt);
    m_cellSize.setWidth(fm.averageCharWidth());
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

void VTermWidgetBase::setDarkMode(bool b)
{
    m_darkMode = b;
    if (m_darkMode) {
        vterm_color_rgb(&m_defaultBg,30,30,30);
        vterm_color_rgb(&m_defaultFg,200,200,200);
        m_cursor.color = QColor(200,200,200,128);
        m_clrSelect = QColor(86,86,84);
    } else {
        vterm_color_rgb(&m_defaultBg,255,255,255);
        vterm_color_rgb(&m_defaultFg,0,0,0);
        m_cursor.color = QColor(0,0,0,128);
        m_clrSelect = QColor(179,215,253);
    }

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
    rect.start_row = this->verticalScrollBar()->value()-m_sbList.size();
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
    int xoff = 0;//(this->width()-this->verticalScrollBar()->sizeHint().width() -m_cellSize.width()*m_cols)/2;
    int yoff = 1-fm.descent();

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
    p.fillRect(cursorRect,m_cursor.color);
}

// paint use QTextLayout
/*
void QVTermWidget::paintEvent(QPaintEvent *e)
{
    QFont fnt = this->font();
    QFontMetrics fm(fnt);
//    QPixmap pix(e->rect().size());
//    QPainter p(&pix);
//    p.setFont(fnt);
//    p.fillRect(e->rect(),Qt::white);
    QPainter p(viewport());
    //p.fillRect(e->rect(),toQColor(&m_defaultBg));

    VTermScreenCell cell;
    int value = this->verticalScrollBar()->value() - m_sbList.size();
    int xoff = 0;
    int yoff = 0;

    QRect cursorRect;
    for (int row = value; row < m_rows+value; row++) {
        int y = yoff+(row-value)*m_cellSize.height();
        QString text;
        QVector<QTextLayout::FormatRange> fmtRanges;
        VTermColor last_bg = m_defaultBg;
        VTermColor last_fg = m_defaultFg;
        VTermScreenCellAttrs last_attr = m_empytCell.attrs;
        QTextLayout::FormatRange lastFR;
        int cursorPos = -1;
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        for (int col = 0; col < m_cols; col++) {
            bool b = fetchCell(row,col,&cell);
            VTermColor *bg = &cell.bg;
            VTermColor *fg = &cell.fg;
            if (cell.attrs.reverse) {
                qSwap(bg,fg);
            }
            if (!vterm_color_is_equal(&m_defaultBg,bg) || !vterm_color_is_equal(&m_defaultFg,fg) || !attrs_is_equal(&last_attr,&cell.attrs) ) {//|| !vterm_color_is_equal(&last_fg,fg)) {
                if (vterm_color_is_equal(&last_bg,bg) && vterm_color_is_equal(&last_fg,fg) && attrs_is_equal(&last_attr,&cell.attrs)) {
                    lastFR.length++;
                } else {
                    if (lastFR.length) {
                        fmtRanges.push_back(lastFR);

                    }
                    lastFR.format = QTextCharFormat();
                    lastFR.start = text.length();
                    lastFR.length = 1;
                    if (!vterm_color_is_equal(&m_defaultBg,bg)) {
                        lastFR.format.setBackground(QBrush(toQColor(bg)));
                    }
                    if (!vterm_color_is_equal(&m_defaultFg,fg)) {
                        lastFR.format.setForeground(QBrush(toQColor(fg)));
                    }
                    if (cell.attrs.bold) {
                        lastFR.format.setFontWeight(QFont::Bold);
                    }
                    if (cell.attrs.italic) {
                        lastFR.format.setFontItalic(true);
                    }
                    if (cell.attrs.strike) {
                        lastFR.format.setFontStrikeOut(true);
                    }
                }
            }
            last_fg = *fg;
            last_bg = *bg;
            last_attr = cell.attrs;
            QString c;
            if (!b || !cell.chars[0]) {
                text += ' ';
                cell.width = 1;
                c = ' ';
            } else {
                c = QString::fromUcs4(cell.chars);
                text += c;
            }

            if (m_cursor.visible && m_cursor.row == row && m_cursor.col == col) {
                int pos = text.length()-1;
                cursorRect = QRect(xoff+fm.width(text.left(pos)),y,fm.width(text.mid(pos)),m_cellSize.height());
                cursorPos = pos;
            }
            if (cell.width > 1) {
                col += cell.width-1;
            }
        }
        if (lastFR.length) {
            fmtRanges.push_back(lastFR);
        }
//        p.setPen(Qt::black);
//        p.drawPath(path);

        //p.drawRect(0,y,m_cellWidth,m_cellHeight);
        //QRect rc(0,0,m_cellWidth)
        //p.drawText(xoff,y,m_cellWidth*m_cols*2,m_cellHeight,0,text);
        //p.drawText(xoff,y,m_cellWidth*m_cols,m_cellHeight,Qt::AlignBottom,text);
        QTextLayout layout(text,fnt);
        layout.setFormats(fmtRanges);
        layout.beginLayout();
        QTextLine line = layout.createLine();
        //line.setNumColumns(m_cols,m_cols*m_cellSize.width());
        line.setLineWidth(m_cols*m_cellSize.width());
        layout.endLayout();
        QTextOption opt;
        opt.setUseDesignMetrics(true);
        layout.setTextOption(opt);//QTextOption(Qt::AlignJustify));
        layout.draw(&p,QPointF(xoff,y));
        if (cursorPos >= 0) {
            layout.drawCursor(&p,QPointF(xoff,y),cursorPos,cursorRect.width());
        }
       //  int stringHeight = fm.ascent() + metrics.descent();
        //p.drawText(xoff,y+fm.ascent(),text);
        //textList.push_back(text);
    }


    if (cursorRect.isEmpty()) {
        return;
    }
//    VTERM_PROP_CURSORSHAPE_BLOCK = 1,
//    VTERM_PROP_CURSORSHAPE_UNDERLINE,
//    VTERM_PROP_CURSORSHAPE_BAR_LEFT,
    switch (m_cursor.shape) {
    case VTERM_PROP_CURSORSHAPE_UNDERLINE:
        cursorRect.setTop(cursorRect.bottom()-2);
        break;
    case VTERM_PROP_CURSORSHAPE_BAR_LEFT:
        cursorRect.setRight(cursorRect.left()+2);
        break;
    }
//    QColor clr = toQColor(&m_defaultFg);
//    clr.setAlpha(128);
//    p.fillRect(cursorRect,clr);
}
*/

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
    if (e->button() == Qt::LeftButton) {
    }
    updateSelection(e->pos());
    viewport()->update();
}

void VTermWidgetBase::mousePressEvent(QMouseEvent *e)
{
//    vterm_mouse_button(m_vt,e->button(),true,qt_to_vtermModifier(e->modifiers()));
//    this->viewport()->update();
    m_mouseButton = e->button();
    this->clearSelection();
    m_ptOrg = e->pos();
}

void VTermWidgetBase::updateSelection(QPoint scenePos)
{
    QPoint start(int((m_ptOrg.x()+m_ptOffset.x()) / m_cellSize.width()),
                 int((m_ptOrg.y()+m_ptOffset.y()) / m_cellSize.height()));
    QPoint end(int((scenePos.x()+m_ptOffset.x()) / m_cellSize.width()),
               int((scenePos.y()+m_ptOffset.y()) / m_cellSize.height()));

    if (start != end) {
        setSelection(start, end);
    }
}

void VTermWidgetBase::mouseReleaseEvent(QMouseEvent *e)
{
 //   vterm_mouse_button(m_vt,e->button(),false,qt_to_vtermModifier(e->modifiers()));
//    this->viewport()->update();
    if (m_mouseButton == e->button()) {
    }
    m_mouseButton = Qt::NoButton;
    this->updateSelection(e->pos());
}

void VTermWidgetBase::mouseDoubleClickEvent(QMouseEvent *e)
{

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
    int rows = e->size().height()/m_cellSize.height();
    int cols = (e->size().width()-this->verticalScrollBar()->sizeHint().width()) /m_cellSize.width();
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
    if (cellStart.y() < 0)
        cellStart.ry() = 0;
    QSize sz = viewport()->size();
    if (cellEnd.x() > sz.width())
        cellEnd.rx() = sz.width();
    if (cellEnd.y() > sz.height())
        cellEnd.ry() = sz.height();

    cellStart.ry() += topVisibleRow();
    cellEnd.ry() += topVisibleRow();
    m_selection = QRect(cellStart, cellEnd);

    m_selected.start_row = cellStart.y()+topVisibleRow();
    m_selected.start_col = cellStart.x();
    m_selected.end_col = cellStart.y();
    m_selected.end_row = cellEnd.y()+topVisibleRow();


    emit selectionChanged();
    viewport()->update();
}

void VTermWidgetBase::selectAll()
{
    m_selection = QRect(0,-scrollbackRowSize(),m_cols+1,allRowSize());
    viewport()->update();
}

void VTermWidgetBase::clearSelection()
{
    if (m_selection.isNull())
        return;

    m_selection = QRect();

    viewport()->update();
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
