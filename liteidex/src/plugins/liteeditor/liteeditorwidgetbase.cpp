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
// Module: liteeditorwidgetbase.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditorwidgetbase.h"
#include "qtc_texteditor/basetextdocumentlayout.h"
#include <QCoreApplication>
#include <QTextBlock>
#include <QPainter>
#include <QStyle>
#include <QDebug>
#include <QMessageBox>
#include <QToolTip>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QScrollBar>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

class TextEditExtraArea : public QWidget {
public:
    TextEditExtraArea(LiteEditorWidgetBase *edit):QWidget(edit) {
        textEdit = edit;
        setAutoFillBackground(true);
    }
public:

    QSize sizeHint() const {
        return QSize(textEdit->extraAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent *event){
        textEdit->extraAreaPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void leaveEvent(QEvent *event){
        textEdit->extraAreaLeaveEvent(event);
    }

    void wheelEvent(QWheelEvent *event) {
        QCoreApplication::sendEvent(textEdit->viewport(), event);
    }
protected:
    LiteEditorWidgetBase *textEdit;
};

LiteEditorWidgetBase::LiteEditorWidgetBase(QWidget *parent)
    : QPlainTextEdit(parent),
      m_editorMark(0),
      m_contentsChanged(false),
      m_lastCursorChangeWasInteresting(false)
{
    setLineWrapMode(QPlainTextEdit::NoWrap);
    m_extraArea = new TextEditExtraArea(this);
    m_extraForeground = QColor(Qt::darkCyan);
    m_extraBackground = m_extraArea->palette().color(QPalette::Background);
    m_CurrentLineBackground = QColor(180,200,200,128);

    setLayoutDirection(Qt::LeftToRight);
    viewport()->setMouseTracking(true);
    m_lineNumbersVisible = true;
    m_marksVisible = true;
    m_codeFoldingVisible = true;
    m_rightLineVisible = true;
    m_rightLineWidth = 80;
    m_lastSaveRevision = 0;
    m_extraAreaSelectionNumber = -1;
    m_autoIndent = true;
    m_bLastBraces = false;
    m_bTabUseSpace = false;
    m_nTabSize = 4;
    m_mouseOnFoldedMarker = false;
    setTabWidth(4);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(slotUpdateExtraAreaWidth()));
    connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged(bool)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(slotUpdateRequest(QRect, int)));
    connect(this->document(),SIGNAL(contentsChange(int,int,int)),this,SLOT(editContentsChanged(int,int,int)));

    QTextDocument *doc = this->document();
    if (doc) {
        TextEditor::BaseTextDocumentLayout *layout = new TextEditor::BaseTextDocumentLayout(doc);
        doc->setDocumentLayout(layout);
        connect(layout,SIGNAL(updateBlock(QTextBlock)),this,SLOT(updateBlock(QTextBlock)));
    }
}

LiteEditorWidgetBase::~LiteEditorWidgetBase()
{
}

void LiteEditorWidgetBase::setEditorMark(LiteApi::IEditorMark *mark)
{
    m_editorMark = mark;
    if (m_editorMark) {
        connect(m_editorMark,SIGNAL(markChanged()),m_extraArea,SLOT(update()));
    }
}

void LiteEditorWidgetBase::setTabWidth(int n)
{
    int charWidth = QFontMetrics(font()).averageCharWidth();
    m_nTabSize = n;
    setTabStopWidth(charWidth * n);
}

void LiteEditorWidgetBase::setTabUseSpace(bool b)
{
    m_bTabUseSpace = b;
}

void LiteEditorWidgetBase::initLoadDocument()
{
    m_lastSaveRevision = document()->revision();
    document()->setModified(false);
}


void LiteEditorWidgetBase::editContentsChanged(int pos, int, int)
{
    m_contentsChanged = true;
}

static bool findMatchBrace(QTextCursor &cur, TextEditor::TextBlockUserData::MatchType &type,int &pos1, int &pos2)
{
    QTextBlock block = cur.block();
    int pos = cur.positionInBlock();
    pos1 = -1;
    pos2 = -1;
    if (block.isValid()) {
        TextEditor::TextBlockUserData *data = static_cast<TextEditor::TextBlockUserData*>(block.userData());
        if (data) {
            TextEditor::Parentheses ses = data->parentheses();
            TextEditor::Parenthesis::Type typ = TextEditor::Parenthesis::Opened;
            QChar chr;
            int i = ses.size();
            while(i--) {
                TextEditor::Parenthesis s = ses.at(i);
                if (s.pos == pos || s.pos+1 == pos) {
                    pos1 = cur.block().position()+s.pos;
                    typ = s.type;
                    chr = s.chr;
                    break;
                }
            }
            if (pos1 != -1) {
                if (typ == TextEditor::Parenthesis::Opened) {
                    cur.setPosition(pos1);
                    type = TextEditor::TextBlockUserData::checkOpenParenthesis(&cur,chr);
                    pos2 = cur.position()-1;
                } else {
                    cur.setPosition(pos1+1);
                    type = TextEditor::TextBlockUserData::checkClosedParenthesis(&cur,chr);
                    pos2 = cur.position();
                }
                return true;
            }
        }
    }
    return false;
}

void LiteEditorWidgetBase::gotoMatchBrace()
{
    QTextCursor cur = this->textCursor();
    TextEditor::TextBlockUserData::MatchType type;
    int pos1 = -1;
    int pos2 = -1;
    if (findMatchBrace(cur,type,pos1,pos2) && type == TextEditor::TextBlockUserData::Match) {
        cur.setPosition(pos2);
        this->setTextCursor(cur);
        if (!cur.block().isVisible()) {
            unfold();
        }
        ensureCursorVisible();
    }
}

void LiteEditorWidgetBase::highlightCurrentLine()
{    
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(m_CurrentLineBackground);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();

        extraSelections.append(selection);
    }
    QTextCursor cur = textCursor();
    TextEditor::TextBlockUserData::MatchType type;
    int pos1 = -1;
    int pos2 = -1;
    if (findMatchBrace(cur,type,pos1,pos2)) {
        if (type == TextEditor::TextBlockUserData::Match) {
            QTextEdit::ExtraSelection selection;
            cur.setPosition(pos1);
            cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
            selection.cursor = cur;
            selection.format.setFontUnderline(true);
            extraSelections.append(selection);

            cur.setPosition(pos2);
            cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
            selection.cursor = cur;
            selection.format.setFontUnderline(true);
            extraSelections.append(selection);
        } else if (type == TextEditor::TextBlockUserData::Mismatch) {
            QTextEdit::ExtraSelection selection;
            cur.setPosition(pos1);
            cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,1);
            selection.cursor = cur;
            selection.format.setFontUnderline(true);
            selection.format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            selection.format.setForeground(Qt::red);
            extraSelections.append(selection);
        }
    }
    QList<QTextEdit::ExtraSelection> all = this->extraSelections();
    QMutableListIterator<QTextEdit::ExtraSelection> i(all);
    while(i.hasNext()) {
        i.next();
        foreach(QTextEdit::ExtraSelection sel, m_extraSelections) {
            if (sel.cursor == i.value().cursor && sel.format == i.value().format) {
                i.remove();
                break;
            }
        }
    }
    m_extraSelections = extraSelections;
    all.append(extraSelections);
    setExtraSelections(all);
}

static int foldBoxWidth(const QFontMetrics &fm)
{
    const int lineSpacing = fm.lineSpacing();
    return lineSpacing/2+lineSpacing%2+1;
}

QWidget* LiteEditorWidgetBase::extraArea()
{
    return m_extraArea;
}

void LiteEditorWidgetBase::setCurrentLineColor(const QColor &background)
{
    if (background.isValid()) {
        m_CurrentLineBackground = background;
    } else {
        m_CurrentLineBackground = QColor(180,200,200,128);
    }
}

void LiteEditorWidgetBase::setExtraColor(const QColor &foreground,const QColor &background)
{
    if (foreground.isValid()) {
        m_extraForeground = foreground;
    } else {
        m_extraForeground = QColor(Qt::darkCyan);
    }
    if (background.isValid()) {
        m_extraBackground = background;
    } else {
        m_extraBackground = m_extraArea->palette().color(QPalette::Background);
    }
}

int LiteEditorWidgetBase::extraAreaWidth()
{
    int space = 0;
    const QFontMetrics fm(m_extraArea->fontMetrics());
    if (m_lineNumbersVisible) {
        QFont fnt = m_extraArea->font();
        fnt.setBold(true);
        const QFontMetrics linefm(fnt);
        int digits = 2;
        int max = qMax(1, blockCount());
        while (max >= 100) {
            max /= 10;
            ++digits;
        }
        space += linefm.width(QLatin1Char('9')) * digits;
    }
    if (m_marksVisible) {
        int markWidth = fm.lineSpacing();
        space += markWidth;
    } else {
        space += 3;
    }
    if (m_codeFoldingVisible) {
        space += foldBoxWidth(fm);
    }
    space += 2;

    return space;
}

void LiteEditorWidgetBase::drawFoldingMarker(QPainter *painter, const QPalette &pal,
                                       const QRect &rect,
                                       bool expanded) const
{
    painter->save();
    painter->setPen(Qt::NoPen);
    int size = rect.size().width();
    int sqsize = 2*(size/2);

    QColor textColor = m_extraForeground;
    QColor brushColor = m_extraBackground;

    textColor.setAlpha(100);
    brushColor.setAlpha(100);

    QPolygon a;
    if (expanded) {
        // down arrow
        //a.setPoints(3, 0, sqsize/3,  sqsize/2, sqsize  - sqsize/3,  sqsize, sqsize/3);
        a.setPoints(3, 1, sqsize/2+sqsize/3,  sqsize/2+sqsize/3, sqsize/2+sqsize/3,sqsize/2+sqsize/3,1);
    } else {
        // right arrow
        a.setPoints(3, sqsize - sqsize/3, sqsize/2,  sqsize/2 - sqsize/3, 0,  sqsize/2 - sqsize/3, sqsize);
    }
    painter->translate(0.5, 0.5);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->translate(rect.topLeft());
    painter->setPen(textColor);
    if (expanded) {
        painter->setBrush(textColor);
    } else {
        painter->setBrush(brushColor);
    }
    painter->drawPolygon(a);
    painter->restore();

}

void LiteEditorWidgetBase::extraAreaPaintEvent(QPaintEvent *e)
{
    QTextDocument *doc = document();

    int selStart = textCursor().selectionStart();
    int selEnd = textCursor().selectionEnd();

    QPalette pal = m_extraArea->palette();
    pal.setCurrentColorGroup(QPalette::Active);
    QPainter painter(m_extraArea);
    const QFontMetrics fm(m_extraArea->font());

    int fmLineSpacing = fm.lineSpacing();
    int markWidth = 0;
    if (m_marksVisible)
        markWidth += fm.lineSpacing();

    const int collapseColumnWidth = m_codeFoldingVisible ? foldBoxWidth(fm): 0;
    const int extraAreaWidth = m_extraArea->width() - collapseColumnWidth;

    painter.fillRect(e->rect(), pal.color(QPalette::Base));
    painter.fillRect(e->rect().intersected(QRect(0, 0, m_extraArea->width(), INT_MAX)),
                     m_extraBackground);

    painter.setPen(QPen(m_extraForeground,1,Qt::DotLine));
    painter.drawLine(extraAreaWidth - 3, e->rect().top(), extraAreaWidth - 3, e->rect().bottom());
    painter.drawLine(e->rect().width()-1, e->rect().top(), e->rect().width()-1, e->rect().bottom());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
    qreal bottom = top;

    painter.setPen(QPen(m_extraForeground,1));
    while (block.isValid() && top <= e->rect().bottom()) {

        top = bottom;
        const qreal height = blockBoundingRect(block).height();
        bottom = top + height;
        QTextBlock nextBlock = block.next();

        QTextBlock nextVisibleBlock = nextBlock;
        int nextVisibleBlockNumber = blockNumber + 1;

        if (!nextVisibleBlock.isVisible()) {
            // invisible blocks do have zero line count
            nextVisibleBlock = doc->findBlockByLineNumber(nextVisibleBlock.firstLineNumber());
            nextVisibleBlockNumber = nextVisibleBlock.blockNumber();
        }

        if (bottom < e->rect().top()) {
            block = nextVisibleBlock;
            blockNumber = nextVisibleBlockNumber;
            continue;
        }

        if (m_codeFoldingVisible || m_marksVisible) {
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing, false);

            int previousBraceDepth = block.previous().userState();
            if (previousBraceDepth >= 0)
                previousBraceDepth >>= 8;
            else
                previousBraceDepth = 0;

            int braceDepth = block.userState();
            if (!nextBlock.isVisible()) {
                QTextBlock lastInvisibleBlock = nextVisibleBlock.previous();
                if (!lastInvisibleBlock.isValid())
                    lastInvisibleBlock = doc->lastBlock();
                braceDepth = lastInvisibleBlock.userState();
            }
            if (braceDepth >= 0)
                braceDepth >>= 8;
            else
                braceDepth = 0;

            if (TextEditor::TextBlockUserData *userData = static_cast<TextEditor::TextBlockUserData*>(block.userData())) {
                if (m_marksVisible) {
                    int xoffset = 0;
                    foreach (TextEditor::ITextMark *mrk, userData->marks()) {
                        int x = 0;
                        int radius = fmLineSpacing - 1;
                        QRect r(x + xoffset, top, radius, radius);
                        mrk->paint(&painter, r);
                        xoffset += 2;
                    }
                }
            }

            if (m_codeFoldingVisible) {
                TextEditor::TextBlockUserData *nextBlockUserData = TextEditor::BaseTextDocumentLayout::testUserData(nextBlock);

                bool drawBox = nextBlockUserData
                               && TextEditor::BaseTextDocumentLayout::foldingIndent(block) < nextBlockUserData->foldingIndent();

                int boxWidth = foldBoxWidth(fm)+1;
                if (drawBox) {
                    bool expanded = nextBlock.isVisible();
                    QRect box(extraAreaWidth-2, top + (fm.lineSpacing()-boxWidth)/2,
                              boxWidth-1,boxWidth-1);
                    drawFoldingMarker(&painter, pal, box, expanded);
                }
            }

            painter.restore();
        }


        if (block.revision() != m_lastSaveRevision) {
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing, false);
            if (block.revision() < 0)
                painter.setPen(QPen(Qt::darkGreen, 2));
            else
                painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(extraAreaWidth - 1, top, extraAreaWidth - 1, bottom - 1);
            painter.restore();
        }

//        if (/*m_marksVisible &&*/ m_editorMark) {
//            m_editorMark->paint(&painter,blockNumber,0,top,fmLineSpacing-0.5,fmLineSpacing-1);
//        }
        if (m_lineNumbersVisible) {
            painter.setPen(QPen(m_extraForeground,2));//pal.color(QPalette::BrightText));
            const QString &number = QString::number(blockNumber + 1);
            bool selected = (
                    (selStart < block.position() + block.length()
                    && selEnd > block.position())
                    || (selStart == selEnd && selStart == block.position())
                    );
            if (selected) {
                painter.save();
                QFont f = painter.font();
                f.setBold(true);
                painter.setFont(f);
                //painter.setPen(QPen(Qt::black,2));
            }
            painter.drawText(QRectF(markWidth, top, extraAreaWidth - markWidth - 4, height), Qt::AlignRight, number);
            if (selected)
                painter.restore();
            painter.setPen(QPen(m_extraForeground,1));//pal.color(QPalette::BrightText));
        }
        block = nextVisibleBlock;
        blockNumber = nextVisibleBlockNumber;
    }

}

void LiteEditorWidgetBase::extraAreaMouseEvent(QMouseEvent *e)
{
    QTextCursor cursor = cursorForPosition(QPoint(0, e->pos().y()));
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick) {
        if (e->button() == Qt::LeftButton) {
            int boxWidth = foldBoxWidth(fontMetrics());
            QTextBlock block = cursor.block();
            bool canFold = TextEditor::BaseTextDocumentLayout::canFold(block);
            if (m_codeFoldingVisible && canFold && e->pos().x() >= extraAreaWidth() - boxWidth-4) {
                if (!cursor.block().next().isVisible()) {
                    toggleBlockVisible(cursor.block());
                    //moveCursorVisible(false);
                } else {
                    QTextBlock c = cursor.block();
                    toggleBlockVisible(c);
                    moveCursorVisible(false);
                }
            } else {
                QTextCursor selection = cursor;
                selection.setVisualNavigation(true);
                m_extraAreaSelectionNumber = selection.blockNumber();
                selection.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                selection.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
                setTextCursor(selection);
            }
        }
    } else if (m_extraAreaSelectionNumber >= 0) {
        QTextCursor selection = cursor;
        selection.setVisualNavigation(true);
        if (e->type() == QEvent::MouseMove) {
            QTextBlock anchorBlock = document()->findBlockByNumber(m_extraAreaSelectionNumber);
            selection.setPosition(anchorBlock.position());
            if (cursor.blockNumber() < m_extraAreaSelectionNumber) {
                selection.movePosition(QTextCursor::EndOfBlock);
                selection.movePosition(QTextCursor::Right);
            }
            selection.setPosition(cursor.block().position(), QTextCursor::KeepAnchor);
            if (cursor.blockNumber() >= m_extraAreaSelectionNumber) {
                selection.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                selection.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            }
        } else {
            m_extraAreaSelectionNumber = -1;
            return;
        }
        setTextCursor(selection);
    }
}

void LiteEditorWidgetBase::extraAreaLeaveEvent(QEvent *)
{

}

void LiteEditorWidgetBase::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    m_extraArea->setGeometry(
        QStyle::visualRect(layoutDirection(), cr,
                           QRect(cr.left(), cr.top(), extraAreaWidth(), cr.height())));
}


void LiteEditorWidgetBase::slotUpdateExtraAreaWidth()
{
    if (isLeftToRight())
        setViewportMargins(extraAreaWidth(), 0, 0, 0);
    else
        setViewportMargins(0, 0, extraAreaWidth(), 0);
}

void LiteEditorWidgetBase::slotModificationChanged(bool m)
{
    if (m)
        return;

    int oldLastSaveRevision = m_lastSaveRevision;
    m_lastSaveRevision = document()->revision();

    if (oldLastSaveRevision != m_lastSaveRevision) {
        QTextBlock block = document()->begin();
        while (block.isValid()) {
            if (block.revision() < 0 || block.revision() != oldLastSaveRevision) {
                block.setRevision(-m_lastSaveRevision - 1);
            } else {
                block.setRevision(m_lastSaveRevision);
            }
            block = block.next();
        }
    }
    m_extraArea->update();
}

void LiteEditorWidgetBase::slotUpdateRequest(const QRect &r, int dy)
{
    if (dy)
        m_extraArea->scroll(0, dy);
    else if (r.width() > 4) { // wider than cursor width, not just cursor blinking
        m_extraArea->update(0, r.y(), m_extraArea->width(), r.height());
        //if (!d->m_searchExpr.isEmpty()) {
        //    const int m = d->m_searchResultOverlay->dropShadowWidth();
        //    viewport()->update(r.adjusted(-m, -m, m, m));
        //}
    }

    if (r.contains(viewport()->rect()))
        slotUpdateExtraAreaWidth();
}

static bool convertPosition(const QTextDocument *document, int pos, int *line, int *column)
{
    QTextBlock block = document->findBlock(pos);
    if (!block.isValid()) {
        (*line) = -1;
        (*column) = -1;
        return false;
    } else {
        (*line) = block.blockNumber() + 1;
        (*column) = pos - block.position();
        return true;
    }
}

QByteArray LiteEditorWidgetBase::saveState() const
{
    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    stream << 1; // version number
    stream << verticalScrollBar()->value();
    stream << horizontalScrollBar()->value();
    int line, column;
    convertPosition(document(),textCursor().position(), &line, &column);
    stream << line-1;
    stream << column;

    // store code folding state
    QList<int> foldedBlocks;
    QTextBlock block = document()->firstBlock();
    while (block.isValid()) {
        if (block.userData() && static_cast<TextEditor::TextBlockUserData*>(block.userData())->folded()) {
            int number = block.blockNumber();
            foldedBlocks += number;
        }
        block = block.next();
    }
    stream << foldedBlocks;
    return state;
}

bool LiteEditorWidgetBase::restoreState(const QByteArray &state)
{
    if (state.isEmpty()) {
        return false;
    }
    int version;
    int vval;
    int hval;
    int lval;
    int cval;
    QDataStream stream(state);
    stream >> version;
    stream >> vval;
    stream >> hval;
    stream >> lval;
    stream >> cval;

    if (version >= 1) {
        QList<int> collapsedBlocks;
        stream >> collapsedBlocks;
        QTextDocument *doc = document();
        foreach(int blockNumber, collapsedBlocks) {
            QTextBlock block = doc->findBlockByNumber(qMax(0, blockNumber));
            if (block.isValid())
                TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, false);
        }
    }

    m_lastCursorChangeWasInteresting = false; // avoid adding last position to history

    gotoLine(lval, cval,false);
    verticalScrollBar()->setValue(vval);
    horizontalScrollBar()->setValue(hval);
    saveCurrentCursorPositionForNavigation();

    return true;
}

void LiteEditorWidgetBase::saveCurrentCursorPositionForNavigation()
{
    m_lastCursorChangeWasInteresting = true;
    m_tempNavigationState = saveState();
}

void LiteEditorWidgetBase::slotCursorPositionChanged()
{
    if (m_lastCursorChangeWasInteresting) {
        //navigate change
        emit navigationStateChanged(m_tempNavigationState);
        m_lastCursorChangeWasInteresting = false;
    } else {
        this->saveCurrentCursorPositionForNavigation();
    }

    //emit navigationStateChanged(saveState());
    /*
    if (!m_contentsChanged && m_lastCursorChangeWasInteresting) {
        //navigate change
        emit navigationStateChanged(m_tempNavigationState);
        m_lastCursorChangeWasInteresting = false;
    } else if (m_contentsChanged) {
        this->saveCurrentCursorPositionForNavigation();
    }
    */
    highlightCurrentLine();
}

void LiteEditorWidgetBase::slotUpdateBlockNotify(const QTextBlock &)
{

}

void LiteEditorWidgetBase::maybeSelectLine()
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection()) {
        const QTextBlock &block = cursor.block();
        if (block.next().isValid()) {
            cursor.setPosition(block.position());
            cursor.setPosition(block.next().position(), QTextCursor::KeepAnchor);
        } else {
            cursor.movePosition(QTextCursor::EndOfBlock);
            cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
        }
        setTextCursor(cursor);
    }
}

void LiteEditorWidgetBase::gotoLine(int line, int column, bool center)
{
    m_lastCursorChangeWasInteresting = false;
    const int blockNumber = line;
    const QTextBlock &block = document()->findBlockByNumber(blockNumber);
    if (block.isValid()) {
        QTextCursor cursor(block);
        if (column > 0) {
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
        } else {
            int pos = cursor.position();
            while (document()->characterAt(pos).category() == QChar::Separator_Space) {
                ++pos;
            }
            cursor.setPosition(pos);
        }
        setTextCursor(cursor);
        if (!cursor.block().isVisible()) {
            unfold();
        }
        if (center) {
            centerCursor();
        } else {
            ensureCursorVisible();
        }
    }
}

QChar LiteEditorWidgetBase::characterAt(int pos) const
{
    return document()->characterAt(pos);
}
void LiteEditorWidgetBase::handleHomeKey(bool anchor)
{
    QTextCursor cursor = textCursor();
    QTextCursor::MoveMode mode = QTextCursor::MoveAnchor;

    if (anchor)
        mode = QTextCursor::KeepAnchor;

    const int initpos = cursor.position();
    int pos = cursor.block().position();
    QChar character = characterAt(pos);
    const QLatin1Char tab = QLatin1Char('\t');

    while (character == tab || character.category() == QChar::Separator_Space) {
        ++pos;
        if (pos == initpos)
            break;
        character = characterAt(pos);
    }

    // Go to the start of the block when we're already at the start of the text
    if (pos == initpos)
        pos = cursor.block().position();

    cursor.setPosition(pos, mode);
    setTextCursor(cursor);
}


void LiteEditorWidgetBase::gotoLineStart()
{
    handleHomeKey(false);
}

void LiteEditorWidgetBase::gotoLineStartWithSelection()
{
    handleHomeKey(true);
}

void LiteEditorWidgetBase::gotoLineEnd()
{
    moveCursor(QTextCursor::EndOfLine);
}

void LiteEditorWidgetBase::gotoLineEndWithSelection()
{
    moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
}

void LiteEditorWidgetBase::duplicate()
{
    QTextCursor cursor = textCursor();
    QTextCursor move = cursor;
    move.beginEditBlock();
    if (cursor.hasSelection()) {
        QString text = move.selectedText();
        int pos = move.selectionEnd();
        move.setPosition(pos);
        move.insertText(text);
        int end = move.position();
        move.setPosition(pos);
        move.setPosition(end,QTextCursor::KeepAnchor);
    } else {
        int pos = move.positionInBlock();
        move.movePosition(QTextCursor::StartOfBlock);
        move.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        QString text = move.selectedText();
        move.movePosition(QTextCursor::EndOfBlock);
        move.insertBlock();
        int start = move.position();
        move.insertText(text);
        move.setPosition(start);
        move.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,pos);
    }
    move.endEditBlock();
    setTextCursor(move);
}

// shift+del
void LiteEditorWidgetBase::cutLine()
{
    maybeSelectLine();
    cut();
}

// ctrl+ins
void LiteEditorWidgetBase::copyLine()
{
    QTextCursor prevCursor = textCursor();
    maybeSelectLine();
    copy();
    setTextCursor(prevCursor);
}

void LiteEditorWidgetBase::deleteLine()
{
    maybeSelectLine();
    textCursor().removeSelectedText();
}

bool LiteEditorWidgetBase::findPrevBlock(QTextCursor &cursor, int indent, const QString &skip) const
{
    QTextBlock block = cursor.block().previous();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data && data->foldingIndent() == indent) {
            QString text = block.text().trimmed();
            if (text.isEmpty() || text.startsWith(skip)) {
                block = block.previous();
                continue;
            }
            cursor.setPosition(block.position());
            return true;
        }
        block = block.previous();
    }
    return false;
}

bool LiteEditorWidgetBase::findStartBlock(QTextCursor &cursor, int indent) const
{
    QTextBlock block = cursor.block();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data && data->foldingIndent() == indent) {
            cursor.setPosition(block.position());
            return true;
        }
        block = block.previous();
    }
    return false;
}


bool LiteEditorWidgetBase::findEndBlock(QTextCursor &cursor, int indent) const
{
    QTextBlock block = cursor.block().next();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data && data->foldingIndent() == indent) {
            cursor.setPosition(block.previous().position());
            return true;
        }
        block = block.next();
    }
    return false;
}

bool LiteEditorWidgetBase::findNextBlock(QTextCursor &cursor, int indent, const QString &skip) const
{
    QTextBlock block = cursor.block().next();
    while(block.isValid()) {
        TextEditor::TextBlockUserData *data = TextEditor::BaseTextDocumentLayout::testUserData(block);
        if (data && data->foldingIndent() == indent) {
            QString text = block.text().trimmed();
            if (text.isEmpty() || text.startsWith(skip)) {
                block = block.next();
                continue;
            }
            cursor.setPosition(block.position());
            return true;
        }
        block = block.next();
    }
    return false;
}


void LiteEditorWidgetBase::gotoPrevBlock()
{
    QTextCursor cursor = this->textCursor();
    if (!findPrevBlock(cursor,0)) {
        cursor.movePosition(QTextCursor::Start);
    }
    this->setTextCursor(cursor);
}

void LiteEditorWidgetBase::gotoNextBlock()
{
    QTextCursor cursor = this->textCursor();
    if (!findNextBlock(cursor,0)) {
        cursor.movePosition(QTextCursor::End);
    }
    this->setTextCursor(cursor);
}

void LiteEditorWidgetBase::selectBlock()
{
    QTextCursor cursor = this->textCursor();
    if (!findStartBlock(cursor,0)) {
        return;
    }
    QTextCursor end = this->textCursor();
    if (!findEndBlock(end,0)) {
        return;
    }
    cursor.setPosition(end.position()+end.block().length()-1,QTextCursor::KeepAnchor);
    this->setTextCursor(cursor);
}

bool LiteEditorWidgetBase::event(QEvent *e)
{
    m_contentsChanged = false;
    return QPlainTextEdit::event(e);
}

void LiteEditorWidgetBase::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Insert && e->modifiers() == Qt::NoModifier) {
        this->setOverwriteMode(!this->overwriteMode());
        emit overwriteModeChanged(this->overwriteMode());
        return;
    }
    bool ro = isReadOnly();
    if (m_bLastBraces == true && e->key() == m_lastBraces) {
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor);
        setTextCursor(cursor);
        m_bLastBraces = false;
        return;
    }

    m_lastBraces = false;
    QChar mr;
    QString mrList = " ";
    switch (e->key()) {
        case '{':
            if (m_autoBraces0)
                mr = '}';
            break;
        case '(':
            if (m_autoBraces1)
                mr = ')';
            break;
        case '[':
            if (m_autoBraces2)
                mr = ']';
            break;
        case '\'':
            if (m_autoBraces3)
                mr = '\'';
            break;
        case '\"':
            if (m_autoBraces4)
                mr = '\"', mrList += "()[]{}";
            break;
    }
    if (!mr.isNull()) {
        QPlainTextEdit::keyPressEvent(e);
        QTextCursor cursor = textCursor();
        int pos = cursor.positionInBlock();
        QString text = cursor.block().text();
        if (pos == text.length() || mrList.contains(text.at(pos))) {
            cursor.beginEditBlock();
            pos = cursor.position();
            cursor.insertText(mr);
            cursor.setPosition(pos);
            cursor.endEditBlock();
            setTextCursor(cursor);
            m_bLastBraces = true;
            m_lastBraces = mr;
        }
        return;
    }

    if ( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return ) {
        if (m_autoIndent) {
            indentEnter(textCursor());
            return;
        }
    }
    if (e == QKeySequence::MoveToStartOfBlock
            || e == QKeySequence::SelectStartOfBlock){
        if ((e->modifiers() & (Qt::AltModifier | Qt::ShiftModifier)) == (Qt::AltModifier | Qt::ShiftModifier)) {
            e->accept();
            return;
        }
        handleHomeKey(e == QKeySequence::SelectStartOfBlock);
        e->accept();
    } else if (e == QKeySequence::MoveToStartOfLine
               || e == QKeySequence::SelectStartOfLine){
        if ((e->modifiers() & (Qt::AltModifier | Qt::ShiftModifier)) == (Qt::AltModifier | Qt::ShiftModifier)) {
            e->accept();
            return;
        }
        QTextCursor cursor = textCursor();
        if (QTextLayout *layout = cursor.block().layout()) {
            if (layout->lineForTextPosition(cursor.position() - cursor.block().position()).lineNumber() == 0) {
                handleHomeKey(e == QKeySequence::SelectStartOfLine);
                e->accept();
                return;
            }
        }
    } else {
        switch (e->key()) {
        case Qt::Key_Tab:
        case Qt::Key_Backtab: {
            if (ro) break;
            QTextCursor cursor = textCursor();
            indentText(cursor, e->key() == Qt::Key_Tab);
            e->accept();
            return;
        }
        }
    }
    QPlainTextEdit::keyPressEvent(e);
}

void LiteEditorWidgetBase::indentBlock(QTextBlock block, bool bIndent)
{
    QTextCursor cursor(block);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.removeSelectedText();
    if (bIndent) {
        cursor.insertText(this->tabText());
    } else {
        QString text = block.text();
        if (!text.isEmpty()) {
            if (text.at(0) == '\t') {
                cursor.deleteChar();
            } else if (m_bTabUseSpace && text.startsWith(QString(m_nTabSize,' '))) {
                int count = m_nTabSize;
                while (count--) {
                    cursor.deleteChar();
                }
            } else if (text.at(0) == ' ') {
                cursor.deleteChar();
            }
        }
    }
    cursor.endEditBlock();
}

void LiteEditorWidgetBase::indentCursor(QTextCursor cur, bool bIndent)
{
   cur.beginEditBlock();
    if (bIndent) {
        cur.insertText(this->tabText());
    } else {         
        QString text = cur.block().text();
        int pos = cur.positionInBlock()-1;
        if (pos >= 0) {
            if (text.at(pos) == '\t') {
                cur.deletePreviousChar();
            } else if (m_bTabUseSpace &&
                       (pos-m_nTabSize+1 >= 0) &&
                       (text.mid(pos-m_nTabSize+1,m_nTabSize) == QString(m_nTabSize,' '))) {
                int count = m_nTabSize;
                while (count--) {
                    cur.deletePreviousChar();
                }
            } else if (text.at(pos) == ' ') {
                cur.deletePreviousChar();
            }
       }
    }
    cur.endEditBlock();
}

void LiteEditorWidgetBase::indentText(QTextCursor cur,bool bIndent)
{
    QTextDocument *doc = document();
    cur.beginEditBlock();
    if (!cur.hasSelection()) {
        indentCursor(cur,bIndent);
    } else {
        QTextBlock block = doc->findBlock(cur.selectionStart());
        QTextBlock end = doc->findBlock(cur.selectionEnd());
        if (end.position() == cur.selectionEnd()) {
            end = end.previous();
        }
        if (block == end && cur.selectionStart() != block.position() ) {
            cur.removeSelectedText();
            //indentCursor(cur,bIndent);
            if (bIndent) {
                cur.insertText(this->tabText());
            }
            goto end;
        }
        bool bResetPos = bIndent && cur.selectionStart() == block.position();
        bool bStart = cur.position() == cur.selectionStart();
        int startPos = cur.selectionStart();

        do {
            indentBlock(block,bIndent);
            block = block.next();
        } while (block.isValid() && block.position() <= end.position());
        int endPos = cur.selectionEnd();
        if (bResetPos) {
            if (bStart) {
                cur.setPosition(endPos);
                cur.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor,endPos-startPos);
            } else {
                cur.setPosition(startPos);
                cur.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,endPos-startPos);
            }
        }
    }
end:
    cur.endEditBlock();
    setTextCursor(cur);
}

QString LiteEditorWidgetBase::tabText(int n) const
{
    if (m_bTabUseSpace) {
        return QString(m_nTabSize*n,' ');
    }
    return QString(n,'\t');
}

void LiteEditorWidgetBase::indentEnter(QTextCursor cur)
{
    QTextBlock block = cur.block();
    if (block.isValid() && block.next().isValid() && !block.next().isVisible()) {
        unfold();
    }
    cur.beginEditBlock();
    int pos = cur.position()-cur.block().position();
    QString text = cur.block().text();
    int i = 0;
    int tab = 0;
    int space = 0;
    QString inText = "\n";
    while (i < text.size()) {
        if (!text.at(i).isSpace())
            break;
        if (text.at(i) == ' ') {
            space++;
        } else if (text.at(i) == '\t') {
            tab++;
        }
        i++;
    }
    tab += space/m_nTabSize;
    inText += this->tabText(tab);

    text.trimmed();
    if (!text.isEmpty()) {
        if (pos >= text.size()) {
            const QChar ch = text.at(text.size()-1);
            if (ch == '{' || ch == '(') {
                inText += this->tabText();
            }
        } else if (pos == text.size()-1 && text.size() >= 3) {
            const QChar l = text.at(text.size()-2);
            const QChar r = text.at(text.size()-1);
            if ( (l == '{' && r == '}') ||
                 (l == '(' && r== ')') ) {
                cur.insertText(inText);
                int pos = cur.position();
                cur.insertText(inText);
                cur.setPosition(pos);
                this->setTextCursor(cur);
                cur.insertText(this->tabText());
                cur.endEditBlock();
                return;
            }
        }
    }
    cur.insertText(inText);
    cur.endEditBlock();
    ensureCursorVisible();
}

void LiteEditorWidgetBase::showToolTip(const QTextCursor &cursor, const QString &tip)
{
    QRect rc = cursorRect(cursor);
    QPoint pt = mapToGlobal(rc.topRight());
    QToolTip::showText(pt,tip,this);
}

void LiteEditorWidgetBase::hideToolTip()
{
    QToolTip::hideText();
}

void LiteEditorWidgetBase::moveCursorVisible(bool ensureVisible)
{
    QTextCursor cursor = this->textCursor();
    if (!cursor.block().isVisible()) {
        cursor.setVisualNavigation(true);
        cursor.movePosition(QTextCursor::Up);
        this->setTextCursor(cursor);
    }
    if (ensureVisible)
        this->ensureCursorVisible();
}

void LiteEditorWidgetBase::toggleBlockVisible(const QTextBlock &block)
{
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(document()->documentLayout());

    bool visible = block.next().isVisible();
    TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, !visible);
    documentLayout->requestUpdate();
    documentLayout->emitDocumentSizeChanged();
}

void LiteEditorWidgetBase::foldIndentChanged(QTextBlock block)
{
    if (!block.isVisible()) {
        QTextDocument *doc = document();
        TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());
        block.setVisible(true);
        documentLayout->requestUpdate();
    }
}

void LiteEditorWidgetBase::updateBlock(QTextBlock cur)
{

}

void LiteEditorWidgetBase::fold()
{
    QTextDocument *doc = document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());
    QTextBlock block = textCursor().block();
    if (!(TextEditor::BaseTextDocumentLayout::canFold(block) && block.next().isVisible())) {
        // find the closest previous block which can fold
        int indent = TextEditor::BaseTextDocumentLayout::foldingIndent(block);
        while (block.isValid() && (TextEditor::BaseTextDocumentLayout::foldingIndent(block) >= indent || !block.isVisible()))
            block = block.previous();
    }
    if (block.isValid()) {
        TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, false);
        this->moveCursorVisible(true);
        documentLayout->requestUpdate();
        documentLayout->emitDocumentSizeChanged();
    }
}

void LiteEditorWidgetBase::unfold()
{
    QTextDocument *doc = document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());

    QTextBlock block = textCursor().block();
    while (block.isValid() && !block.isVisible())
        block = block.previous();
    TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, true);
    this->moveCursorVisible(true);
    documentLayout->requestUpdate();
    documentLayout->emitDocumentSizeChanged();
}

void LiteEditorWidgetBase::foldAll()
{
    QTextDocument *doc = document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());

    QTextBlock block = doc->firstBlock();

    while (block.isValid()) {
        if (TextEditor::BaseTextDocumentLayout::canFold(block))
            TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, false);
        block = block.next();
    }

    moveCursorVisible(true);
    documentLayout->requestUpdate();
    documentLayout->emitDocumentSizeChanged();
    centerCursor();
}


void LiteEditorWidgetBase::unfoldAll()
{
    QTextDocument *doc = document();
    TextEditor::BaseTextDocumentLayout *documentLayout = qobject_cast<TextEditor::BaseTextDocumentLayout*>(doc->documentLayout());

    QTextBlock block = doc->firstBlock();
    while (block.isValid()) {
        if (TextEditor::BaseTextDocumentLayout::canFold(block))
            TextEditor::BaseTextDocumentLayout::doFoldOrUnfold(block, true);
        block = block.next();
    }

    moveCursorVisible(true);
    documentLayout->requestUpdate();
    documentLayout->emitDocumentSizeChanged();
    centerCursor();
}

QTextBlock LiteEditorWidgetBase::foldedBlockAt(const QPoint &pos, QRect *box) const
{
    QPointF offset(contentOffset());
    QTextBlock block = firstVisibleBlock();
    qreal top = blockBoundingGeometry(block).translated(offset).top();
    qreal bottom = top + blockBoundingRect(block).height();

    int viewportHeight = viewport()->height();

    while (block.isValid() && top <= viewportHeight) {
        QTextBlock nextBlock = block.next();
        if (block.isVisible() && bottom >= 0) {
            if (nextBlock.isValid() && !nextBlock.isVisible()) {
                QTextLayout *layout = block.layout();
                QTextLine line = layout->lineAt(layout->lineCount()-1);
                QRectF lineRect = line.naturalTextRect().translated(offset.x(), top);
                lineRect.adjust(0, 0, -1, -1);

                QRectF collapseRect(lineRect.right() + 12,
                                    lineRect.top(),
                                    fontMetrics().width(QLatin1String(" {...}; ")),
                                    lineRect.height());
                if (collapseRect.contains(pos)) {
                    QTextBlock result = block;
                    if (box)
                        *box = collapseRect.toAlignedRect();
                    return result;
                } else {
                    block = nextBlock;
                    while (nextBlock.isValid() && !nextBlock.isVisible()) {
                        block = nextBlock;
                        nextBlock = block.next();
                    }
                }
            }
        }

        block = nextBlock;
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
    }
    return QTextBlock();
}

void LiteEditorWidgetBase::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        QTextBlock foldedBlock = foldedBlockAt(e->pos());
        if (foldedBlock.isValid()) {
            toggleBlockVisible(foldedBlock);
            viewport()->setCursor(Qt::IBeamCursor);
        }
    }

    QPlainTextEdit::mousePressEvent(e);
}

void LiteEditorWidgetBase::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() == Qt::NoButton) {
        const QTextBlock collapsedBlock = foldedBlockAt(e->pos());
        if (collapsedBlock.isValid() && !m_mouseOnFoldedMarker) {
            m_mouseOnFoldedMarker = true;
            viewport()->setCursor(Qt::PointingHandCursor);
        } else if (!collapsedBlock.isValid() && m_mouseOnFoldedMarker) {
            m_mouseOnFoldedMarker = false;
            viewport()->setCursor(Qt::IBeamCursor);
        }
    } else {
        QPlainTextEdit::mouseMoveEvent(e);
    }
    if (viewport()->cursor().shape() == Qt::BlankCursor)
        viewport()->setCursor(Qt::IBeamCursor);
}

static void fillBackground(QPainter *p, const QRectF &rect, QBrush brush, QRectF gradientRect = QRectF())
{
    p->save();
    if (brush.style() >= Qt::LinearGradientPattern && brush.style() <= Qt::ConicalGradientPattern) {
        if (!gradientRect.isNull()) {
            QTransform m = QTransform::fromTranslate(gradientRect.left(), gradientRect.top());
            m.scale(gradientRect.width(), gradientRect.height());
            brush.setTransform(m);
            const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
        }
    } else {
        p->setBrushOrigin(rect.topLeft());
    }
    p->fillRect(rect, brush);
    p->restore();
}


void LiteEditorWidgetBase::paintEvent(QPaintEvent *e)
{  
    //QPlainTextEdit::paintEvent(e);
    QPainter painter(viewport());
    QTextDocument *doc = this->document();
    QTextCursor cursor = textCursor();

    bool hasSelection = cursor.hasSelection();
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    QTextBlock block = firstVisibleBlock();
    QPointF offset = contentOffset();

    //copy of QPlainTextEdit::paintEvent
    QRect er = e->rect();
    QRect viewportRect = viewport()->rect();

    bool editable = !isReadOnly();

    qreal maximumWidth = document()->documentLayout()->documentSize().width();

    // Set a brush origin so that the WaveUnderline knows where the wave started
    painter.setBrushOrigin(offset);

    // keep right margin clean from full-width selection
    int maxX = offset.x() + qMax((qreal)viewportRect.width(), maximumWidth)
               - document()->documentMargin();
    er.setRight(qMin(er.right(), maxX));
    painter.setClipRect(er);


    QAbstractTextDocumentLayout::PaintContext context = getPaintContext();

    while (block.isValid()) {

        QRectF r = blockBoundingRect(block).translated(offset);
        QTextLayout *layout = block.layout();

        if (!block.isVisible()) {
            offset.ry() += r.height();
            block = block.next();
            continue;
        }

        if (r.bottom() >= er.top() && r.top() <= er.bottom()) {

            QTextBlockFormat blockFormat = block.blockFormat();

            QBrush bg = blockFormat.background();
            if (bg != Qt::NoBrush) {
                QRectF contentsRect = r;
                contentsRect.setWidth(qMax(r.width(), maximumWidth));
                fillBackground(&painter, contentsRect, bg);
            }


            QVector<QTextLayout::FormatRange> selections;
            int blpos = block.position();
            int bllen = block.length();
            for (int i = 0; i < context.selections.size(); ++i) {
                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
                const int selStart = range.cursor.selectionStart() - blpos;
                const int selEnd = range.cursor.selectionEnd() - blpos;
                if (selStart < bllen && selEnd > 0
                    && selEnd > selStart) {
                    QTextLayout::FormatRange o;
                    o.start = selStart;
                    o.length = selEnd - selStart;
                    o.format = range.format;
                    selections.append(o);
                } else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
                           && block.contains(range.cursor.position())) {
                    // for full width selections we don't require an actual selection, just
                    // a position to specify the line. that's more convenience in usage.
                    QTextLayout::FormatRange o;
                    QTextLine l = layout->lineForTextPosition(range.cursor.position() - blpos);
                    o.start = l.textStart();
                    o.length = l.textLength();
                    if (o.start + o.length == bllen - 1)
                        ++o.length; // include newline
                    o.format = range.format;
                    selections.append(o);
                }
            }

            bool drawCursor = (editable
                               && context.cursorPosition >= blpos
                               && context.cursorPosition < blpos + bllen);

            bool drawCursorAsBlock = drawCursor && overwriteMode() ;

            if (drawCursorAsBlock) {
                if (context.cursorPosition == blpos + bllen - 1) {
                    drawCursorAsBlock = false;
                } else {
                    QTextLayout::FormatRange o;
                    o.start = context.cursorPosition - blpos;
                    o.length = 1;
                    o.format.setForeground(palette().base());
                    o.format.setBackground(palette().text());
                    selections.append(o);
                }
            }


            layout->draw(&painter, offset, selections, er);

            if ((drawCursor && !drawCursorAsBlock)
                || (editable && context.cursorPosition < -1
                    && !layout->preeditAreaText().isEmpty())) {
                int cpos = context.cursorPosition;
                if (cpos < -1)
                    cpos = layout->preeditAreaPosition() - (cpos + 2);
                else
                    cpos -= blpos;
                layout->drawCursor(&painter, offset, cpos, cursorWidth());
            }
        }

        //draw tabs
        painter.save();
        painter.setPen(QPen(m_extraForeground,1,Qt::DotLine));
        QString text = block.text();
        qDebug() << text ;
        int k = 0;
        for (int i = 0; i < text.length(); i++) {
            const QChar c = text.at(i);
            if (!c.isSpace()) {
                break;
            }
            if (k%4 == 0) {
                QTextLine line = layout->lineForTextPosition(i);
                qreal l = line.cursorToX(i);
                painter.drawLine(offset.x()+l,r.top(),offset.x()+l,r.bottom());
            }
            if (c == '\t') {
                k += 4;
            } else {
                k += 1;
            }
        }
        painter.restore();


        QTextBlock nextBlock = block.next();
        //daaw wrap
        if (true) {
            int lineCount = layout->lineCount();
            if (lineCount >= 2 || !nextBlock.isValid()) {
                painter.save();
                painter.setPen(Qt::lightGray);
                for (int i = 0; i < lineCount-1; ++i) { // paint line wrap indicator
                    QTextLine line = layout->lineAt(i);
                    QRectF lineRect = line.naturalTextRect().translated(offset.x(), r.top());
                    QChar visualArrow((ushort)0x21b5);
                    painter.drawText(QPointF(lineRect.right(),
                                             lineRect.top() + line.ascent()),
                                     visualArrow);
                }
                if (!nextBlock.isValid()) { // paint EOF symbol
                    QTextLine line = layout->lineAt(lineCount-1);
                    QRectF lineRect = line.naturalTextRect().translated(offset.x(), r.top());
                    int h = 4;
                    lineRect.adjust(0, 0, -1, -1);
                    QPainterPath path;
                    QPointF pos(lineRect.topRight() + QPointF(h+4, line.ascent()));
                    path.moveTo(pos);
                    path.lineTo(pos + QPointF(-h, -h));
                    path.lineTo(pos + QPointF(0, -2*h));
                    path.lineTo(pos + QPointF(h, -h));
                    path.closeSubpath();
                    painter.setBrush(painter.pen().color());
                    painter.drawPath(path);
                }
                painter.restore();
            }
        }

        //draw fold text ...
        QTextBlock nextVisibleBlock = nextBlock;

        if (!nextVisibleBlock.isVisible()) {
            // invisible blocks do have zero line count
            nextVisibleBlock = doc->findBlockByLineNumber(nextVisibleBlock.firstLineNumber());
            // paranoia in case our code somewhere did not set the line count
            // of the invisible block to 0
            while (nextVisibleBlock.isValid() && !nextVisibleBlock.isVisible())
                nextVisibleBlock = nextVisibleBlock.next();
        }

        if (nextBlock.isValid() && !nextBlock.isVisible()) {

            bool selectThis = (hasSelection
                               && nextBlock.position() >= selectionStart
                               && nextBlock.position() < selectionEnd);
            if (selectThis) {
                painter.save();
                painter.setBrush(palette().highlight());
            }

            QTextLayout *layout = block.layout();
            QTextLine line = layout->lineAt(layout->lineCount()-1);
            QRectF lineRect = line.naturalTextRect().translated(offset.x(), r.top());
            lineRect.adjust(0, 0, -1, -1);

            QRectF collapseRect(lineRect.right() + 12,
                                lineRect.top(),
                                fontMetrics().width(QLatin1String(" {...}; ")),
                                lineRect.height());
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.translate(.5, .5);
            painter.drawRoundedRect(collapseRect.adjusted(0, 0, 0, -1), 3, 3);
            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.translate(-.5, -.5);

            QString replacement = QLatin1String("...");

            if (TextEditor::TextBlockUserData *nextBlockUserData = TextEditor::BaseTextDocumentLayout::testUserData(nextBlock)) {
                if (nextBlockUserData->foldingStartIncluded())
                    replacement.prepend(nextBlock.text().trimmed().left(1));
            }

            block = nextVisibleBlock.previous();
            if (!block.isValid())
                block = doc->lastBlock();

            if (TextEditor::TextBlockUserData *blockUserData = TextEditor::BaseTextDocumentLayout::testUserData(block)) {
                if (blockUserData->foldingEndIncluded()) {
                    QString right = block.text().trimmed();
                    if (right.endsWith(QLatin1Char(';'))) {
                        right.chop(1);
                        right = right.trimmed();
                        replacement.append(right.right(right.endsWith(QLatin1Char('/')) ? 2 : 1));
                        replacement.append(QLatin1Char(';'));
                    } else {
                        replacement.append(right.right(right.endsWith(QLatin1Char('/')) ? 2 : 1));
                    }
                }
            }

            if (selectThis)
                painter.setPen(palette().highlightedText().color());
            painter.drawText(collapseRect, Qt::AlignCenter, replacement);
            if (selectThis)
                painter.restore();
        }

        offset.ry() += r.height();

        if (offset.y() > viewportRect.height())
            break;
        block = block.next();
    }

    if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
        && (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
        painter.fillRect(QRect(QPoint((int)er.left(), (int)offset.y()), er.bottomRight()), palette().background());
    }

    if (m_rightLineVisible) {
        const QFontMetrics fm(this->font());
        int xoff = this->document()->documentMargin()+fm.averageCharWidth()*m_rightLineWidth;
        xoff -= this->horizontalScrollBar()->value();
        painter.save();
        painter.setPen(QPen(m_extraForeground,1,Qt::DotLine));
        painter.drawLine(xoff,0,xoff,rect().height());
        painter.restore();
    }
}
