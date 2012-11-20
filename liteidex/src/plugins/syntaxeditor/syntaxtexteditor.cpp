/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: syntaxtexteditor.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: syntaxtexteditor.cpp,v 1.0 2011-5-12 visualfc Exp $

#include <QtGui>

#include "syntaxtexteditor.h"
#include "golanghighlighter.h"
#include "QCompleter"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


SyntaxTextEditor::SyntaxTextEditor(QWidget *parent) :
        QPlainTextEdit(parent),editCompleter(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setLineWrapMode(QPlainTextEdit::NoWrap);

    isUntitled = true;
    editorArea = new SyntaxTextEditorArea(this);
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateEditorArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    setCursorWidth(2);

    updateAreaWidth(0);
    highlightCurrentLine();
    this->autoBlock = true;
    this->autoIndent = true;
    this->autoWord = false;
}

void SyntaxTextEditor::reload()
{
    QFile file(curFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LiteIDE"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(curFile)
                             .arg(file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    setPlainText(codec->toUnicode(file.readAll()));
    /*
    QTextCursor cursor(document());
    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);
    cursor.insertText(codec->toUnicode(file.readAll()));
    cursor.endEditBlock();
    */
    QApplication::restoreOverrideCursor();
}

void SyntaxTextEditor::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.go").arg(sequenceNumber++);
    curText = curFile + "[*]";
    setWindowTitle(curText);
}

bool SyntaxTextEditor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LiteIDE"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    setPlainText(codec->toUnicode(file.readAll()));
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    return true;
}

bool SyntaxTextEditor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool SyntaxTextEditor::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

QByteArray SyntaxTextEditor::data() const
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    return codec->fromUnicode(toPlainText());
}

bool SyntaxTextEditor::saveFile(const QString &fileName)
{
    if (!this->document()->isModified())
        return true;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LiteIDE"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    file.write(codec->fromUnicode(toPlainText()));
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    return true;
}

QString SyntaxTextEditor::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void SyntaxTextEditor::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool SyntaxTextEditor::maybeSave()
{
    if (document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("LiteIDE"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void SyntaxTextEditor::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    curText = userFriendlyCurrentFile();
}

QString SyntaxTextEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

int SyntaxTextEditor::editorAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 6 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void SyntaxTextEditor::areaPaintEvent(QPaintEvent *event)
{
    QPainter painter(editorArea);

    painter.fillRect(event->rect(), Qt::lightGray);//lightGray);

//![extraAreaPaintEvent_0]

//![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
//![extraAreaPaintEvent_1]

//![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, editorArea->width()-3, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void SyntaxTextEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    editorArea->setGeometry(QRect(cr.left(), cr.top(), editorAreaWidth(), cr.height()));
}

void SyntaxTextEditor::updateAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(editorAreaWidth(), 0, 0, 0);
}

void SyntaxTextEditor::updateEditorArea(const QRect &rect, int dy)
{
    if (dy)
        editorArea->scroll(0, dy);
    else
        editorArea->update(0, rect.y(), editorArea->width(), rect.height());

    if (rect.contains(viewport()->rect())) {
        updateAreaWidth(0);
    }
}

void SyntaxTextEditor::setCompleter(SyntaxCompleter *completer)
{
    if (editCompleter)
        QObject::disconnect(editCompleter, 0, this, 0);

    editCompleter = completer;

    if (!editCompleter)
        return;

    editCompleter->setFileName(this->curFile);
    editCompleter->setWidget(this);
    QObject::connect(editCompleter, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));
}

SyntaxCompleter *SyntaxTextEditor::completer() const
{
    return this->editCompleter;
}


QString SyntaxTextEditor::wordUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

QString SyntaxTextEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::BlockUnderCursor);
    QString block = tc.selectedText();
    QStringList all = block.split(QRegExp("[\t| |(|)]"));
    if (all.isEmpty())
        return "";

    return all.last();
}

void SyntaxTextEditor::focusInEvent(QFocusEvent *e)
{
    if (editCompleter)
         editCompleter->setWidget(this);
     QPlainTextEdit::focusInEvent(e);
}

void SyntaxTextEditor::keyPressEvent(QKeyEvent *e)
{
    if (editCompleter && editCompleter->popup()->isVisible()) {
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
    if (e->key() == Qt::Key_Tab) {
        indentText(document(), textCursor(),true);
        e->accept();
        return;
    } else if (e->key() == Qt::Key_Backtab) {
        indentText(document(),textCursor(),false);
        e->accept();
        return;
    }

    if ( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return ) {
        if (this->autoIndent) {
            indentEnter(textCursor());
        } else {
            e->accept();
        }
        emit update();
        return;
    }

    if (this->autoBlock && e->key() == '{') {
        QTextCursor cursor(this->textCursor());
        cursor.insertText("{}");
        cursor.movePosition(QTextCursor::PreviousCharacter);
        setTextCursor(cursor);
        e->accept();
        return;
    }

    if (!this->autoWord) {
        QPlainTextEdit::keyPressEvent(e);
        return;
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!editCompleter || !isShortcut) // do not process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!editCompleter || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()+{}|:\"<>?,./;'[]\\-= "); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;

    QString word = wordUnderCursor();
    QString text = textUnderCursor();

    if (editCompleter->underCursor(e->key(),textCursor(), text,word)) {
        return;
    }

    if (!isShortcut && (hasModifier || e->text().isEmpty() || text.length() <= 2
                        || eow.contains(e->text().right(1)))) {
        editCompleter->hidePopup();
        return;
    }
    editCompleter->showPopup(word);
}

void SyntaxTextEditor::insertCompletion(const QString& completion)
{
    if (editCompleter->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - editCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
    editCompleter->endCompletion();
}

void SyntaxTextEditor::loadConfig(QSettings *settings, const QString &mimeType)
{
    autoIndent = settings->value(mimeType+"/autoindent",true).toBool();
    autoBlock = settings->value(mimeType+"/autoblock",true).toBool();
    curFont.setFamily(settings->value(mimeType+"/family","Courier").toString());
    curFont.setPointSize(settings->value(mimeType+"/fontsize",12).toInt());
    setFont(curFont);
    setTabStopWidth(fontMetrics().width("main"));
    autoWord = settings->value(mimeType+"/autoword",false).toBool();
}

void SyntaxTextEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(180,200,200,128);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void SyntaxTextEditor::gotoLine(int line, int column)
{
    const int blockNumber = line - 1;
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
        centerCursor();
    }
}

void SyntaxTextEditor::indentBlock(QTextBlock block, bool bIndent)
{
    QTextCursor cursor(block);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.removeSelectedText();
    if (bIndent) {
        cursor.insertText("\t");
    } else {
        QString text = block.text();
        if (!text.isEmpty() && (text.at(0) == '\t' || text.at(0) == ' ')) {
            cursor.deleteChar();
        }
    }
    cursor.endEditBlock();
}

void SyntaxTextEditor::indentCursor(QTextCursor cur, bool bIndent)
{
    if (bIndent) {
        cur.insertText("\t");
    } else {
        QString text = cur.block().text();
        int pos = cur.position()-cur.block().position()-1;
        int count = text.count();
        if (count > 0 && pos >= 0 && pos < count) {
            if (text.at(pos) == '\t' || text.at(pos) == ' ') {
                cur.deletePreviousChar();
            }
        }
    }
}

void SyntaxTextEditor::indentText(QTextDocument *doc, QTextCursor cur, bool bIndent)
{
    cur.beginEditBlock();
    if (!cur.hasSelection()) {
        indentCursor(cur,bIndent);
    } else {
        QTextBlock block = doc->findBlock(cur.selectionStart());
        QTextBlock end = doc->findBlock(cur.selectionEnd());
        if (!cur.atBlockStart()) {
            end = end.next();
        }
        do {
            indentBlock(block,bIndent);
            block = block.next();
        } while (block.isValid() && block != end);
    }
    cur.endEditBlock();
}

void SyntaxTextEditor::indentEnter(QTextCursor cur)
{
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
        if (text.at(0) == ' ') {
            space++;
        }
        else if (text.at(0) == '\t') {
            inText += "\t";
            tab++;
        }
        i++;
    }
    text.trimmed();
    if (!text.isEmpty()) {
        if (pos >= text.size()) {
            const QChar ch = text.at(text.size()-1);
            if (ch == '{' || ch == '(') {
                inText += "\t";
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
                cur.insertText("\t");
                cur.endEditBlock();
                return;
            }
        }
    }
    cur.insertText(inText);
    cur.endEditBlock();
    ensureCursorVisible();
}

