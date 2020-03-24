/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE. All rights reserved.
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
// Module: syntaxtexteditor.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef SYNTAXTEXTEDITOR_H
#define SYNTAXTEXTEDITOR_H

#include <QPlainTextEdit>
#include <QTextEdit>
#include <QSettings>
#include "syntaxcompleter.h"

class SyntaxTextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    void indentEnter(QTextCursor cur);
    void indentText(QTextDocument *doc, QTextCursor cur, bool bIndent);
    void indentBlock(QTextBlock block, bool bIndent);
    void indentCursor(QTextCursor cur, bool bIndent);
    void gotoLine(int line, int column);
    QString wordUnderCursor() const;
    QString textUnderCursor() const;
    void setCompleter(SyntaxCompleter *c);
    SyntaxCompleter *completer() const;
    bool autoIndent;
    bool autoBlock;
    bool autoWord;
    void areaPaintEvent(QPaintEvent *event);
    SyntaxTextEditor(QWidget *parent = 0);
    void newFile();
    void reload();
    QString currentText() { return curText; }
    bool loadFile(const QString &fileName);
    QByteArray data() const;
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
signals:
    void update();
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent *e);
public:
    int editorAreaWidth();
public slots:
    void loadConfig(QSettings *settings, const QString &mimeType);
protected slots:
    void highlightCurrentLine();
    void insertCompletion(const QString& completion);
    void updateAreaWidth(int newBlockCount);
    void updateEditorArea(const QRect &, int);
public:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    QString curText;
    QFont   curFont;

    bool isUntitled;
    QWidget *editorArea;
    SyntaxCompleter *editCompleter;
};

class SyntaxTextEditorArea : public QWidget
{
public:
    SyntaxTextEditorArea(SyntaxTextEditor *ed) : QWidget(ed) {
        editor = ed;
    }

    QSize sizeHint() const {
        return QSize(editor->editorAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {        
        editor->areaPaintEvent(event);
    }

private:
    SyntaxTextEditor *editor;
};


#endif // SYNTAXTEXTEDITOR_H
