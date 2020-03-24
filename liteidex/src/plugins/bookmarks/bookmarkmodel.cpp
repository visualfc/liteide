/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
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
// Module: bookmarkmodel.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "bookmarkmodel.h"
#include <QPainter>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

void BookmarkModel::addNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    beginInsertRows(QModelIndex(), m_nodeList.size(), m_nodeList.size());

    BookmarkNode *bn = createBookmarkNode(mark,node);
    m_nodeList.append(bn);
    m_nodeMap.insert(node,bn);

    endInsertRows();
    //selectionModel()->setCurrentIndex(index(m_bookmarksList.size()-1 , 0, QModelIndex()), QItemSelectionModel::Select | QItemSelectionModel::Clear);
}

void BookmarkModel::removeNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    BookmarkNode *bn = findBookmarkNode(mark,node);
    if (!bn) {
        return;
    }
    int idx = m_nodeList.indexOf(bn);
    beginRemoveRows(QModelIndex(), idx, idx);

    m_nodeMap.remove(node);

    delete bn;

    m_nodeList.removeAt(idx);
    endRemoveRows();
 //   if (selectionModel()->currentIndex().isValid())
    //       selectionModel()->setCurrentIndex(selectionModel()->currentIndex(), QItemSelectionModel::Select | QItemSelectionModel::Clear);
}

void BookmarkModel::updateNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node)
{
    BookmarkNode *bn = findBookmarkNode(mark,node);
    if (!bn) {
        return;
    }
    bn->setLineNumber(node->blockNumber()+1);
    bn->setLineText(node->block().text());
    int idx = m_nodeList.indexOf(bn);
    QModelIndex i = index(idx,0,QModelIndex());
    emit dataChanged(i,i);
}

BookmarkNode *BookmarkModel::createBookmarkNode(LiteApi::IEditorMark *mark, LiteApi::IEditorMarkNode *node) const
{
    BookmarkNode *n = new BookmarkNode();
    n->setFilePath(mark->filePath());
    n->setLineNumber(node->blockNumber()+1);
    n->setLineText(node->block().text());
    return n;
}

BookmarkNode *BookmarkModel::bookmarkNodeForIndex(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() >= m_nodeList.size())
        return 0;
    return m_nodeList.at(index.row());
}

BookmarkNode *BookmarkModel::findBookmarkNode(LiteApi::IEditorMark */*mark*/, LiteApi::IEditorMarkNode *node) const
{
    return m_nodeMap.value(node);
}

BookmarkModel::BookmarkModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex BookmarkModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();
    else
        return createIndex(row, column);
}

QModelIndex BookmarkModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int BookmarkModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_nodeList.size();
}

int BookmarkModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 1;
}

QVariant BookmarkModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() !=0 || index.row() < 0 || index.row() >= m_nodeList.count())
        return QVariant();

    BookmarkNode *node = m_nodeList.at(index.row());
    if (role == BookmarkModel::FileName)
        return node->fileName();
    if (role == BookmarkModel::LineNumber)
        return node->lineNumber();
    if (role == BookmarkModel::FilePath)
        return node->filePath();
    if (role == BookmarkModel::LineText)
        return node->lineText();
    if (role == BookmarkModel::Note)
        return node->noteText();
    if (role == Qt::ToolTipRole)
        return QDir::toNativeSeparators(node->filePath());
    return QVariant();
}

BookmarkDelegate::BookmarkDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QSize BookmarkDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QFontMetrics fm(option.font);
    QSize s;
    s.setWidth(option.rect.width());
    s.setHeight(fm.height() * 2 + 10);
    return s;
}

void BookmarkDelegate::generateGradientPixmap(int width, int height, const QColor &color, bool selected) const
{
    QColor c = color;
    c.setAlpha(0);

    QPixmap pixmap(width+1, height);
    pixmap.fill(c);

    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);

    QLinearGradient lg;
    lg.setCoordinateMode(QGradient::ObjectBoundingMode);
    lg.setFinalStop(1,0);

    lg.setColorAt(0, c);
    lg.setColorAt(0.4, color);

    painter.setBrush(lg);
    painter.drawRect(0, 0, width+1, height);

    if (selected)
        m_selectedPixmap = pixmap;
    else
        m_normalPixmap = pixmap;
}

void BookmarkDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    painter->save();

    QFontMetrics fm(opt.font);
    static int lwidth = fm.width(QLatin1String("8888")) + 18;

    QColor backgroundColor;
    QColor textColor;

    bool selected = opt.state & QStyle::State_Selected;

    if (selected) {
        painter->setBrush(opt.palette.highlight().color());
        backgroundColor = opt.palette.highlight().color();
        if (!m_selectedPixmap)
            generateGradientPixmap(lwidth, fm.height()+1, backgroundColor, selected);
    } else {
        painter->setBrush(opt.palette.background().color());
        backgroundColor = opt.palette.background().color();
        if (!m_normalPixmap)
            generateGradientPixmap(lwidth, fm.height(), backgroundColor, selected);
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(opt.rect);

    // Set Text Color
    if (opt.state & QStyle::State_Selected)
        textColor = opt.palette.highlightedText().color();
    else
        textColor = opt.palette.text().color();

    painter->setPen(textColor);


    // TopLeft
    QString topLeft = index.data(BookmarkModel::FileName).toString();
    //painter->drawText(6, 2 + opt.rect.top() + fm.ascent(), topLeft);

    QString topRight = index.data(BookmarkModel::LineNumber).toString();
    // Check whether we need to be fancy and paint some background
    int fwidth = fm.width(topLeft);
    if (fwidth + lwidth > opt.rect.width()) {
        int left = opt.rect.right() - lwidth;
        painter->drawPixmap(left, opt.rect.top(), selected ? m_selectedPixmap : m_normalPixmap);
    }
    // topRight
    painter->drawText(opt.rect.right() - fm.width(topRight) - 6 , 2 + opt.rect.top() + fm.ascent(), topRight);

    // Directory
    QColor mix;
    mix.setRgbF(0.7 * textColor.redF()   + 0.3 * backgroundColor.redF(),
                0.7 * textColor.greenF() + 0.3 * backgroundColor.greenF(),
                0.7 * textColor.blueF()  + 0.3 * backgroundColor.blueF());
    painter->setPen(mix);

    QString directory = index.data(BookmarkModel::FilePath).toString();
    int availableSpace = opt.rect.width() - fm.width("888");
    if (fm.width(directory) > availableSpace) {
        // We need a shorter directory
        availableSpace -= fm.width("...");

        int pos = directory.size();
        int idx;
        forever {
            idx = directory.lastIndexOf("/", pos-1);
            if (idx == -1) {
                // Can't happen, this means the string did fit after all?
                break;
            }
            int width = fm.width(directory.mid(idx, pos-idx));
            if (width > availableSpace) {
                directory = "..." + directory.mid(pos);
                break;
            } else {
                pos = idx;
                availableSpace -= width;
            }
        }
    }

    //painter->drawText(3, opt.rect.top() + fm.ascent() + fm.height() + 6, directory);
    painter->drawText(6, 2 + opt.rect.top() + fm.ascent(), directory);

    QString lineText = index.data(BookmarkModel::Note).toString().trimmed();
    if (lineText.isEmpty())
        lineText = index.data(BookmarkModel::LineText).toString().trimmed();

    painter->drawText(6, opt.rect.top() + fm.ascent() + fm.height() + 6, lineText);

    // Separator lines
    const QRectF innerRect = QRectF(opt.rect).adjusted(0.5, 0.5, -0.5, -0.5);
    painter->setPen(QColor::fromRgb(150,150,150));
    painter->drawLine(innerRect.bottomLeft(), innerRect.bottomRight());
    painter->restore();
}
