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
    m_scrollWheelZooming(true),
    m_bSpellCheckZoneDontComplete(false)
{
    this->m_averageCharWidth = QFontMetrics(this->font()).averageCharWidth();
}

void LiteEditorWidget::setContextMenu(QMenu *contextMenu)
{
    m_contextMenu = contextMenu;
}

void LiteEditorWidget::setCompleter(LiteApi::ICompleter *completer)
{
    m_completer = completer;
}

void LiteEditorWidget::codeCompleter()
{
    QTextCursor cursor = this->textCursor();
    bool isInImport = false;
    if (m_textLexer->isInStringOrComment(cursor)) {
        isInImport = m_textLexer->isInImport(cursor);
        if (!isInImport) {
            return;
        }
    }
    if (isInImport) {
        QString completionPrefix = importUnderCursor(textCursor());
        m_completer->setCompletionContext(LiteApi::CompleterImportContext);
        m_completer->setCompletionPrefix("");
        m_completer->startCompleter(completionPrefix);
    } else {
        QString completionPrefix = textUnderCursor(textCursor());
        if (completionPrefix.startsWith(".")) {
            completionPrefix.insert(0,'@');
        }
        m_completer->setCompletionContext(LiteApi::CompleterCodeContext);
        m_completer->setCompletionPrefix("");
        emit completionPrefixChanged(completionPrefix,true);
        m_completer->startCompleter(completionPrefix);
    }
}

QString LiteEditorWidget::wordUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

QString LiteEditorWidget::importUnderCursor(QTextCursor tc) const
{
    QString text = tc.block().text().left(tc.positionInBlock());
    if (text.isEmpty()) {
        return QString();
    }
    static QRegExp reg("[\"`][a-zA-Z0-9_\\-\\.\\/]*$");
    int index = reg.indexIn(text);
    if (index < 0) {
        return QString();
    }
    return text.right(reg.matchedLength()-1);
}

QString LiteEditorWidget::textUnderCursor(QTextCursor tc) const
{
    QString text = tc.block().text().left(tc.positionInBlock());
    if (text.isEmpty()) {
        return QString();
    }
    //int index = text.lastIndexOf(QRegExp("\\b[a-zA-Z_][a-zA-Z0-9_\.]+"));
    static QRegExp reg("[a-zA-Z0-9_\\.]+[a-zA-Z0-9_\\.\\:]*$");
    int index = reg.indexIn(text);
    if (index < 0) {
        return QString();
    }
    return text.right(reg.matchedLength());
    //int index = text.lastIndexOf(QRegExp("[\w]+$"));
    //     qDebug() << ">" << text << index;
    //     int left = text.lastIndexOf(QRegExp("[ |\t|\"|\(|\)|\'|<|>]"));
    //     text = text.right(text.length()-left+1);
    //return "";
}

void LiteEditorWidget::focusInEvent(QFocusEvent *e)
{
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
    if (!m_completer) {
        LiteEditorWidgetBase::keyPressEvent(e);
        return;
    }
    if (m_inputCursorOffset > 0) {
        m_completer->hidePopup();
        LiteEditorWidgetBase::keyPressEvent(e);
        return;
    }

    if (m_completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
        case Qt::Key_Shift:
            e->ignore();
            return; // let the completer do default behavior
        case Qt::Key_N:
        case Qt::Key_P:
            if (e->modifiers() == Qt::ControlModifier) {
                e->ignore();
                return;
            }
        default:
            break;
        }
    }

    bool isInImport = false;
    if (m_textLexer->isInStringOrComment(this->textCursor())) {
        isInImport = m_textLexer->isInImport(this->textCursor());
        if (!isInImport) {
            LiteEditorWidgetBase::keyPressEvent(e);
            m_completer->hidePopup();
            return;
        }
    }

    LiteEditorWidgetBase::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

    //always break if ctrl is pressed and there's a key
//    if (((e->modifiers() & Qt::ControlModifier) && !e->text().isEmpty())) {
//        return;
//    }
    if (e->modifiers() & Qt::ControlModifier) {
        if (!e->text().isEmpty()) {
            m_completer->hidePopup();
        }
        return;
    }

    if (e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) {
        return;
    }

    if (e->text().isEmpty()) {
        if (e->key() != Qt::Key_Backspace) {
            m_completer->hidePopup();
            return;
        }
    }
    //import line
    if (isInImport) {
        QString completionPrefix = importUnderCursor(textCursor());
        if (completionPrefix.isEmpty()) {
            return;
        }
        m_completer->setCompletionContext(LiteApi::CompleterImportContext);
        m_completer->setCompletionPrefix("");
        m_completer->startCompleter(completionPrefix);
        return;
    }

    //static QString eow("~!@#$%^&*()+{}|:\"<>?,/;'[]\\-="); // end of word
    static QString eow("~!@#$%^&*()+{}|\"<>?,/;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor(textCursor());
    if (completionPrefix.startsWith("...")) {
        completionPrefix = completionPrefix.mid(3);
    } else if (completionPrefix.startsWith(".")) {
        completionPrefix.insert(0,'@');
    }

    if (hasModifier ||
            e->text().isEmpty() ||
            ( (completionPrefix.length() < m_completer->prefixMin()) && (completionPrefix.right(1) != ".") ) ||
            eow.contains(e->text().right(1))) {
        if (m_completer->popup()->isVisible()) {
            m_completer->popup()->hide();
            //fmt.Print( -> Print
            if (e->text() == "(") {
                QTextCursor cur = textCursor();
                cur.movePosition(QTextCursor::Left);
                QString lastPrefix = textUnderCursor(cur);
                if (lastPrefix.startsWith(".")) {
                    lastPrefix.insert(0,"@");
                }
                if (!lastPrefix.isEmpty() &&
                        lastPrefix == m_completer->completionPrefix() ) {
                    if (lastPrefix == m_completer->currentCompletion() ||
                            lastPrefix.endsWith("."+m_completer->currentCompletion())) {
                        m_completer->updateCompleteInfo(m_completer->currentIndex());
                    }
                }
            }
        }
        return;
    }

    m_completer->setCompletionContext(LiteApi::CompleterCodeContext);
    emit completionPrefixChanged(completionPrefix,false);
    m_completer->startCompleter(completionPrefix);
}

void LiteEditorWidget::inputMethodEvent(QInputMethodEvent *e)
{
    if (!e->preeditString().isEmpty()) {
        m_completer->hidePopup();
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

static const char kVerticalTextBlockMimeType[] = "application/vnd.liteide.vblocktext";

QMimeData *LiteEditorWidget::createMimeDataFromSelection() const
{
    QTextCursor cursor = textCursor();
    if (m_inBlockSelectionMode) {
        QMimeData *mimeData = new QMimeData;
        QString text = this->copyBlockSelection();
        mimeData->setData(QLatin1String(kVerticalTextBlockMimeType), text.toUtf8());
        mimeData->setText(text); // for exchangeability
        return mimeData;
    } else if (cursor.hasSelection()) {
        QMimeData *mimeData = new QMimeData;
        QString text = cursor.selectedText();
        convertToPlainText(text);
        mimeData->setText(text);
        // Copy the selected text as HTML
        mimeData->setHtml(cursorToHtml(cursor));
        return mimeData;
    }
    return 0;
}

bool LiteEditorWidget::canInsertFromMimeData(const QMimeData *source) const
{
    return QPlainTextEdit::canInsertFromMimeData(source);
}

void LiteEditorWidget::insertFromMimeData(const QMimeData *source)
{
    if (isReadOnly())
        return;

    if (source->hasFormat(QLatin1String(kVerticalTextBlockMimeType))) {
        QString text = QString::fromUtf8(source->data(QLatin1String(kVerticalTextBlockMimeType)));
        if (text.isEmpty())
            return;

        QStringList lines = text.split(QLatin1Char('\n'));
        QTextCursor cursor = textCursor();
        cursor.beginEditBlock();
        const TextEditor::TabSettings &ts = this->tabSettings();
        int initialCursorPosition = cursor.position();
        int column = ts.columnAt(cursor.block().text(), cursor.positionInBlock());
        cursor.insertText(lines.first());
        for (int i = 1; i < lines.count(); ++i) {
            QTextBlock next = cursor.block().next();
            if (next.isValid()) {
                cursor.setPosition(next.position());
            } else {
                cursor.movePosition(QTextCursor::EndOfBlock);
                cursor.insertBlock();
            }
            int offset = 0;
            int position = ts.positionAtColumn(cursor.block().text(), column, &offset);
            cursor.setPosition(cursor.block().position() + position);
            if (offset < 0) {
                cursor.deleteChar();
                cursor.insertText(QString(-offset, QLatin1Char(' ')));
            } else {
                cursor.insertText(QString(offset, QLatin1Char(' ')));
            }
            cursor.insertText(lines.at(i));
        }
        cursor.setPosition(initialCursorPosition);
        cursor.endEditBlock();
        setTextCursor(cursor);
        ensureCursorVisible();
        return;
    }

    QPlainTextEdit::insertFromMimeData(source);
}

void LiteEditorWidget::zoomIn(int range)
{
    emit requestFontZoom(range*10);
}

void LiteEditorWidget::zoomOut(int range)
{
    emit requestFontZoom(-range*10);
}

void LiteEditorWidget::updateFont(const QFont &font)
{
    this->setFont(font);
    this->extraArea()->setFont(font);
    this->m_averageCharWidth = QFontMetrics(font).averageCharWidth();
    this->updateTabWidth();
    this->slotUpdateExtraAreaWidth();
    if (this->m_completer) {
        this->m_completer->popup()->setFont(font);
    }
}

void LiteEditorWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        LiteEditorWidgetBase::dropEvent(event);
        return;
    }
    bool hasFile = false;
    foreach (QUrl url, urls) {
        QString fileName = url.toLocalFile();
        if (!fileName.isEmpty()) {
            QFileInfo info(fileName);
            if (info.isFile()) {
                m_liteApp->fileManager()->openEditor(fileName,true);
            } else if(info.isDir()) {
                m_liteApp->fileManager()->addFolderList(fileName);
            }
            hasFile = true;
        }
    }
    if (hasFile) {
        event->accept();
        return;
    }
    LiteEditorWidgetBase::dropEvent(event);
}
