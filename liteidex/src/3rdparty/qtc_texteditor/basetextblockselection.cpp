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

#include "basetextblockselection.h"

using namespace TextEditor;

BaseTextBlockSelection::BaseTextBlockSelection()
    :firstVisualColumn(0), lastVisualColumn(0), anchor(BottomRight)
{
}

void BaseTextBlockSelection::moveAnchor(int blockNumber, int visualColumn)
{
    if (visualColumn >= 0) {
        if (anchor % 2) {
            lastVisualColumn = visualColumn;
            if (lastVisualColumn < firstVisualColumn) {
                qSwap(firstVisualColumn, lastVisualColumn);
                anchor = (Anchor) (anchor - 1);
            }
        } else {
            firstVisualColumn = visualColumn;
            if (firstVisualColumn > lastVisualColumn) {
                qSwap(firstVisualColumn, lastVisualColumn);
                anchor = (Anchor) (anchor + 1);
            }
        }
    }

    if (blockNumber >= 0 && blockNumber < firstBlock.document()->blockCount()) {
        if (anchor <= TopRight) {
            firstBlock.setPosition(firstBlock.document()->findBlockByNumber(blockNumber).position());
            if (firstBlock.blockNumber() > lastBlock.blockNumber()) {
                qSwap(firstBlock, lastBlock);
                anchor = (Anchor) (anchor + 2);
            }
        } else {
            lastBlock.setPosition(firstBlock.document()->findBlockByNumber(blockNumber).position());
            if (lastBlock.blockNumber() < firstBlock.blockNumber()) {
                qSwap(firstBlock, lastBlock);
                anchor = (Anchor) (anchor - 2);
            }
        }
    }
    firstBlock.movePosition(QTextCursor::StartOfBlock);
    lastBlock.movePosition(QTextCursor::EndOfBlock);
}

int BaseTextBlockSelection::position(const TabSettings &ts) const
{
    const QTextBlock &block = anchor <= TopRight ? lastBlock.block() : firstBlock.block();
    const int column = anchor % 2 ? firstVisualColumn : lastVisualColumn;
    return block.position() + ts.positionAtColumn(block.text(), column);
}

QTextCursor BaseTextBlockSelection::selection(const TabSettings &ts) const
{
    QTextCursor cursor = firstBlock;
    if (anchor <= TopRight) {
        cursor.setPosition(lastBlock.block().position() + ts.positionAtColumn(lastBlock.block().text(), lastVisualColumn));
        cursor.setPosition(firstBlock.block().position() + ts.positionAtColumn(firstBlock.block().text(), firstVisualColumn),
                           QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(firstBlock.block().position() + ts.positionAtColumn(firstBlock.block().text(), firstVisualColumn));
        cursor.setPosition(lastBlock.block().position() + ts.positionAtColumn(lastBlock.block().text(), lastVisualColumn),
                           QTextCursor::KeepAnchor);
    }
    return cursor;
}

void BaseTextBlockSelection::fromSelection(const TabSettings &ts, const QTextCursor &selection)
{
    firstBlock = selection;
    firstBlock.setPosition(selection.selectionStart());
    firstVisualColumn = ts.columnAt(firstBlock.block().text(), firstBlock.positionInBlock());
    lastBlock = selection;
    lastBlock.setPosition(selection.selectionEnd());
    lastVisualColumn = ts.columnAt(lastBlock.block().text(), lastBlock.positionInBlock());
    if (selection.anchor() > selection.position())
        anchor = TopLeft;
    else
        anchor = BottomRight;

    firstBlock.movePosition(QTextCursor::StartOfBlock);
    lastBlock.movePosition(QTextCursor::EndOfBlock);
}
