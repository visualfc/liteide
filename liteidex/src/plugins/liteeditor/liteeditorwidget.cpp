/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: liteeditorwidget.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "liteeditorwidget.h"

#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextDocumentFragment>
#include <QMimeData>
#include <QMenu>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteEditorWidget::LiteEditorWidget(LiteApi::IApplication *app, QWidget *parent) :
    LiteEditorWidgetBase(app,parent),
    m_completer(0),
    m_contextMenu(0),
    m_completionPrefixMin(3),
    m_scrollWheelZooming(true),
    m_bSpellCheckZoneDontComplete(false)
{
}

void LiteEditorWidget::setContextMenu(QMenu *contextMenu)
{
    m_contextMenu = contextMenu;
}

void LiteEditorWidget::setCompleter(QCompleter *completer)
{
    if (m_completer)
        QObject::disconnect(m_completer, 0, this, 0);

    m_completer = completer;
}

QCompleter *LiteEditorWidget::completer() const
{
    return m_completer;
}

void LiteEditorWidget::codeCompleter()
{
    QString completionPrefix = textUnderCursor(textCursor());
    if (completionPrefix.startsWith(".")) {
        completionPrefix.insert(0,'@');
    }

    m_completer->setCompletionPrefix("");
    emit completionPrefixChanged(completionPrefix,true);

    if (completionPrefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }

    if (m_completer->currentCompletion() == completionPrefix) {
        m_completer->popup()->hide();
        return;
    }

    QRect cr = cursorRect();
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());

    m_completer->complete(cr); // popup it up!
}

QString LiteEditorWidget::wordUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

QString LiteEditorWidget::textUnderCursor(QTextCursor tc) const
{
    QString text = tc.block().text().left(tc.positionInBlock());
    if (text.isEmpty()) {
        return QString();
    }
    //int index = text.lastIndexOf(QRegExp("\\b[a-zA-Z_][a-zA-Z0-9_\.]+"));
    static QRegExp reg("[a-zA-Z_\\.]+[a-zA-Z0-9_\\.]*$");
    int index = reg.indexIn(text);
    if (index < 0) {
        return QString();
    }
    return text.right(reg.matchedLength());
    //int index = text.lastIndexOf(QRegExp("[\w]+$"));
    //     qDebug() << ">" << text << index;
    //     int left = text.lastIndexOf(QRegExp("[ |\t|\"|\(|\)|\'|<|>]"));
    //     text = text.right(text.length()-left+1);
    return "";
}

void LiteEditorWidget::focusInEvent(QFocusEvent *e)
{
    if (m_completer)
        m_completer->setWidget(this);
    LiteEditorWidgetBase::focusInEvent(e);
}

void LiteEditorWidget::wheelEvent(QWheelEvent *e)
{
    if (m_scrollWheelZooming && e->modifiers() & Qt::ControlModifier) {
        const int delta = e->delta();
        if (delta < 0)
            zoomOut();
        else if (delta > 0)
            zoomIn();
        return;
    }
    LiteEditorWidgetBase::wheelEvent(e);
}

void LiteEditorWidget::contextMenuEvent(QContextMenuEvent *e)
{
    QTextCursor cur = this->textCursor();
    if (!cur.hasSelection()) {
        cur = this->cursorForPosition(e->pos());
        this->setTextCursor(cur);
    }
    if (m_contextMenu) {
        m_contextMenu->exec(e->globalPos());
    }
}

void LiteEditorWidget::keyPressEvent(QKeyEvent *e)
{
    if (m_completer && m_completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    LiteEditorWidgetBase::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

    //always break if ctrl is pressed and there's a key
    if (((e->modifiers() & Qt::ControlModifier) && !e->text().isEmpty()) || !m_completer) {
        return;
    }

    if (e->modifiers() & Qt::ControlModifier) {
        m_completer->popup()->hide();
        return;
    }
    
    if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) {
        return;
    }

    //static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    static QString eow("~!@#$%^&*()+{}|:\"<>?,/;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor(textCursor());
    if (completionPrefix.startsWith(".")) {
        completionPrefix.insert(0,'@');
    }

    if (hasModifier || e->text().isEmpty()||
                        ( completionPrefix.length() < m_completionPrefixMin && completionPrefix.right(1) != ".")
                        || eow.contains(e->text().right(1))) {
        m_completer->popup()->hide();
        return;
    }
    emit completionPrefixChanged(completionPrefix,false);

    if (completionPrefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }

    if (m_completer->currentCompletion() == completionPrefix) {
        m_completer->popup()->hide();
        return;
    }

    QRect cr = cursorRect();
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());

    m_completer->complete(cr); // popup it up!
}

void LiteEditorWidget::inputMethodEvent(QInputMethodEvent *e)
{
    if (!e->commitString().isEmpty() && m_completer->popup()->isVisible()) {
        m_completer->popup()->hide();
    }
    LiteEditorWidgetBase::inputMethodEvent(e);
}

static void convertToPlainText(QString &txt)
{
    QChar *uc = txt.data();
    QChar *e = uc + txt.size();

    for (; uc != e; ++uc) {
        switch (uc->unicode()) {
        case 0xfdd0: // QTextBeginningOfFrame
        case 0xfdd1: // QTextEndOfFrame
        case QChar::ParagraphSeparator:
        case QChar::LineSeparator:
            *uc = QLatin1Char('\n');
            break;
        case QChar::Nbsp:
            *uc = QLatin1Char(' ');
            break;
        default:
            ;
        }
    }
}

QString LiteEditorWidget::cursorToHtml(QTextCursor cursor) const
{
    QTextDocument *tempDocument = new QTextDocument;
    QTextCursor tempCursor(tempDocument);
    tempCursor.insertFragment(cursor.selection());

    // Apply the additional formats set by the syntax highlighter
    QTextBlock start = document()->findBlock(cursor.selectionStart());
    QTextBlock end = document()->findBlock(cursor.selectionEnd());
    end = end.next();

    const int selectionStart = cursor.selectionStart();
    const int endOfDocument = tempDocument->characterCount() - 1;
    for (QTextBlock current = start; current.isValid() && current != end; current = current.next()) {
        const QTextLayout *layout = current.layout();
        foreach (const QTextLayout::FormatRange &range, layout->additionalFormats()) {
            const int start = current.position() + range.start - selectionStart;
            const int end = start + range.length;
            if (end <= 0 || start >= endOfDocument)
                continue;
            tempCursor.setPosition(qMax(start, 0));
            tempCursor.setPosition(qMin(end, endOfDocument), QTextCursor::KeepAnchor);
            tempCursor.setCharFormat(range.format);
        }
    }

    // Reset the user states since they are not interesting
    for (QTextBlock block = tempDocument->begin(); block.isValid(); block = block.next())
        block.setUserState(-1);

    // Make sure the text appears pre-formatted
    tempCursor.setPosition(0);
    tempCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QTextBlockFormat blockFormat = tempCursor.blockFormat();
    blockFormat.setNonBreakableLines(true);
    tempCursor.setBlockFormat(blockFormat);
    QString html = tempCursor.selection().toHtml();//("utf-8");
    html.replace("\t","&nbsp&nbsp&nbsp&nbsp");
    delete tempDocument;
    return html;
}

QMimeData *LiteEditorWidget::createMimeDataFromSelection() const
{
    QTextCursor cursor = textCursor();

    if (!cursor.hasSelection()) {
        return 0;
    }
    QMimeData *mimeData = new QMimeData;
    QString text = cursor.selectedText();
    convertToPlainText(text);
    mimeData->setText(text);
    // Copy the selected text as HTML
    mimeData->setHtml(cursorToHtml(cursor));
    return mimeData;
}

void LiteEditorWidget::zoomIn(int range)
{
    emit requestFontZoom(range*10);
}

void LiteEditorWidget::zoomOut(int range)
{
    emit requestFontZoom(-range*10);
}
