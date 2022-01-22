/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "searchresulttreeitemdelegate.h"
#include "searchresulttreeitemroles.h"

#include <QPainter>
#include <QApplication>

#include <QModelIndex>
#include <QDebug>

using namespace Find::Internal;

SearchResultTreeItemDelegate::SearchResultTreeItemDelegate(QObject *parent)
  : QItemDelegate(parent)
{
}

void SearchResultTreeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    static const int iconSize = 16;

    painter->save();

    QStyleOptionViewItem opt = setOptions(index, option);
    painter->setFont(opt.font);

    QItemDelegate::drawBackground(painter, opt, index);

    // ---- do the layout
    QRect checkRect;
    QRect pixmapRect;
    QRect textRect;

    // check mark
    bool checkable = (index.model()->flags(index) & Qt::ItemIsUserCheckable);
    Qt::CheckState checkState = Qt::Unchecked;
    if (checkable) {
        QVariant checkStateData = index.data(Qt::CheckStateRole);
        checkState = static_cast<Qt::CheckState>(checkStateData.toInt());
#if QT_VERSION >= 0x050000
        checkRect = doCheck(opt, opt.rect, checkStateData);
#else // Qt4
        checkRect = check(opt, opt.rect, checkStateData);
#endif
    }

    // icon
    QIcon icon = index.model()->data(index, ItemDataRoles::ResultIconRole).value<QIcon>();
    if (!icon.isNull())
        pixmapRect = QRect(0, 0, iconSize, iconSize);

    // text
    textRect = opt.rect.adjusted(0, 0, checkRect.width() + pixmapRect.width(), 0);

    // do layout
    doLayout(opt, &checkRect, &pixmapRect, &textRect, false);

    // ---- draw the items
    // icon
    if (!icon.isNull())
        QItemDelegate::drawDecoration(painter, opt, pixmapRect, icon.pixmap(iconSize));

    // line numbers
    int lineNumberAreaWidth = drawLineNumber(painter, opt, textRect, index);
    textRect.adjust(lineNumberAreaWidth, 0, 0, 0);

    // text and focus/selection
    drawText(painter, opt, textRect, index);
    QItemDelegate::drawFocus(painter, opt, opt.rect);

    // check mark
    if (checkable)
        QItemDelegate::drawCheck(painter, opt, checkRect, checkState);

    painter->restore();
}

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
typedef QStyleOptionViewItem QStyleOptionViewItemV3;
#endif

// returns the width of the line number area
int SearchResultTreeItemDelegate::drawLineNumber(QPainter *painter, const QStyleOptionViewItemV3 &option,
                                                 const QRect &rect,
                                                 const QModelIndex &index) const
{
    static const int lineNumberAreaHorizontalPadding = 4;
    int lineNumber = index.model()->data(index, ItemDataRoles::ResultLineNumberRole).toInt();
    if (lineNumber < 1)
        return 0;
    const bool isSelected = option.state & QStyle::State_Selected;
    QString lineText = QString::number(lineNumber);
    int minimumLineNumberDigits = qMax((int)m_minimumLineNumberDigits, lineText.count());
    int fontWidth = painter->fontMetrics().width(QString(minimumLineNumberDigits, QLatin1Char('0')));
    int lineNumberAreaWidth = lineNumberAreaHorizontalPadding + fontWidth + lineNumberAreaHorizontalPadding;
    QRect lineNumberAreaRect(rect);
    lineNumberAreaRect.setWidth(lineNumberAreaWidth);

    QPalette::ColorGroup cg = QPalette::Normal;
    if (!(option.state & QStyle::State_Active))
        cg = QPalette::Inactive;
    else if (!(option.state & QStyle::State_Enabled))
        cg = QPalette::Disabled;

    painter->fillRect(lineNumberAreaRect, QBrush(isSelected ?
        option.palette.brush(cg, QPalette::Highlight) :
        option.palette.color(cg, QPalette::Base).darker(111)));

    QStyleOptionViewItemV3 opt = option;
    opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
    opt.palette.setColor(cg, QPalette::Text, Qt::darkGray);

    const QStyle *style = QApplication::style();
    const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, 0) + 1;

    const QRect rowRect = lineNumberAreaRect.adjusted(-textMargin, 0, textMargin-lineNumberAreaHorizontalPadding, 0);
    QItemDelegate::drawDisplay(painter, opt, rowRect, lineText);

    return lineNumberAreaWidth;
}

void SearchResultTreeItemDelegate::drawText(QPainter *painter,
                                            const QStyleOptionViewItem &option,
                                            const QRect &rect,
                                            const QModelIndex &index) const
{
    QString text = index.model()->data(index, Qt::DisplayRole).toString();
    // show number of subresults in displayString
    if (index.model()->hasChildren(index)) {
        text += QLatin1String(" (")
                + QString::number(index.model()->rowCount(index))
                + QLatin1Char(')');
    }

    const int searchTermStart = index.model()->data(index, ItemDataRoles::SearchTermStartRole).toInt();
    int searchTermLength = index.model()->data(index, ItemDataRoles::SearchTermLengthRole).toInt();
    if (searchTermStart < 0 || searchTermStart >= text.length() || searchTermLength < 1) {
        QItemDelegate::drawDisplay(painter, option, rect, text);
        return;
    }
    // clip searchTermLength to end of line
    searchTermLength = qMin(searchTermLength, text.length() - searchTermStart);
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
    int searchTermStartPixels = painter->fontMetrics().width(text.left(searchTermStart));
    int searchTermLengthPixels = painter->fontMetrics().width(text.mid(searchTermStart, searchTermLength));

    // rects
    QRect beforeHighlightRect(rect);
    beforeHighlightRect.setRight(beforeHighlightRect.left() + searchTermStartPixels);

    QRect resultHighlightRect(rect);
    resultHighlightRect.setLeft(beforeHighlightRect.right());
    resultHighlightRect.setRight(resultHighlightRect.left() + searchTermLengthPixels);

    QRect afterHighlightRect(rect);
    afterHighlightRect.setLeft(resultHighlightRect.right());

    // paint all highlight backgrounds
    // qitemdelegate has problems with painting background when highlighted
    // (highlighted background at wrong position because text is offset with textMargin)
    // so we duplicate a lot here, see qitemdelegate for reference
    bool isSelected = option.state & QStyle::State_Selected;
    QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                              ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
        cg = QPalette::Inactive;
    QStyleOptionViewItem baseOption = option;
    baseOption.state &= ~QStyle::State_Selected;
    if (isSelected) {
        painter->fillRect(beforeHighlightRect.adjusted(textMargin, 0, textMargin, 0),
                          option.palette.brush(cg, QPalette::Highlight));
        painter->fillRect(afterHighlightRect.adjusted(textMargin, 0, textMargin, 0),
                          option.palette.brush(cg, QPalette::Highlight));
    }
    const QColor highlightBackground =
            index.model()->data(index, ItemDataRoles::ResultHighlightBackgroundColor).value<QColor>();
    painter->fillRect(resultHighlightRect.adjusted(textMargin, 0, textMargin - 1, 0), QBrush(highlightBackground));

    // Text before the highlighting
    QStyleOptionViewItem noHighlightOpt = baseOption;
    noHighlightOpt.rect = beforeHighlightRect;
    noHighlightOpt.textElideMode = Qt::ElideNone;
    if (isSelected)
        noHighlightOpt.palette.setColor(QPalette::Text, noHighlightOpt.palette.color(cg, QPalette::HighlightedText));
    QItemDelegate::drawDisplay(painter, noHighlightOpt,
                               beforeHighlightRect, text.mid(0, searchTermStart));

    // Highlight text
    QStyleOptionViewItem highlightOpt = noHighlightOpt;
    const QColor highlightForeground =
            index.model()->data(index, ItemDataRoles::ResultHighlightForegroundColor).value<QColor>();
    highlightOpt.palette.setColor(QPalette::Text, highlightForeground);
    QItemDelegate::drawDisplay(painter, highlightOpt, resultHighlightRect,
                               text.mid(searchTermStart, searchTermLength));

    // Text after the Highlight
    noHighlightOpt.rect = afterHighlightRect;
    QItemDelegate::drawDisplay(painter, noHighlightOpt, afterHighlightRect,
                               text.mid(searchTermStart + searchTermLength));
}
