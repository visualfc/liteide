/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: syntaxeditor.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "syntaxeditor.h"
#include "syntaxtexteditor.h"
#include <QTextDocument>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QToolBar>
#include <QAction>
#include <QTextCodec>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


TextFile::TextFile(QObject *parent)
    : LiteApi::IFile(parent)
{
    m_codec = QTextCodec::codecForName("UTF-8");
}

QString TextFile::fileName() const
{
    return m_fileName;
}

bool TextFile::save(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return false;
    }
    QString text = m_document->toPlainText();
    if (m_codec) {
        file.write(m_codec->fromUnicode(text));
    } else {
        file.write(text.toLocal8Bit());
    }
    m_document->setModified(false);
    return true;
}

bool TextFile::reload()
{
    if (m_document->isModified()) {
        QString text = QString(tr("Cancel file %1 modify and reload ?")).arg(m_fileName);
        int ret = QMessageBox::question(0,"Question",text,QMessageBox::Yes|QMessageBox::No);
        if (ret != QMessageBox::Ok) {
            return false;
        }
    }
    bool ret = open(m_fileName,m_mimeType);
    if (ret) {
        emit reloaded();
    }
    return ret;
}

QString TextFile::mimeType() const
{
    return m_mimeType;
}

QString TextFile::subType() const
{
    return QString();
}

void TextFile::setDocument(QTextDocument *document)
{
    m_document = document;
}

QTextDocument  *TextFile::document()
{
    return m_document;
}

bool TextFile::open(const QString &fileName, const QString &mimeType)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    m_mimeType = mimeType;
    m_fileName = fileName;
    QByteArray text = file.readAll();
    if (m_codec) {
        m_document->setPlainText(m_codec->toUnicode(text));
    } else {
        m_document->setPlainText(text);
    }
    m_document->setModified(false);

    return true;
}


SyntaxEditor::SyntaxEditor(QObject *parent)
    : LiteApi::IEditor(parent)
{
    m_editor = new SyntaxTextEditor(0);
    m_file = new TextFile(this);
    m_file->setDocument(m_editor->document());

    m_toolBar = new QToolBar(tr("SyntaxEditor ToolBar"));
    m_undoAct = new QAction(QIcon(":/images/undo.png"),tr("Undo"),m_toolBar);
    m_redoAct = new QAction(QIcon(":/images/redo.png"),tr("Redo"),m_toolBar);
    m_toolBar->addAction(m_undoAct);
    m_toolBar->addAction(m_redoAct);

    m_undoAct->setEnabled(false);
    m_redoAct->setEnabled(false);

    connect(m_editor,SIGNAL(textChanged()),this,SIGNAL(contentsChanged()));
    connect(m_editor,SIGNAL(modificationChanged(bool)),this,SIGNAL(modificationChanged(bool)));
    connect(m_undoAct,SIGNAL(triggered()),m_editor,SLOT(undo()));
    connect(m_redoAct,SIGNAL(triggered()),m_editor,SLOT(redo()));
    connect(m_editor,SIGNAL(undoAvailable(bool)),m_undoAct,SLOT(setEnabled(bool)));
    connect(m_editor,SIGNAL(redoAvailable(bool)),m_redoAct,SLOT(setEnabled(bool)));
}

bool SyntaxEditor::autoDelete() const
{
    return true;
}

QWidget *SyntaxEditor::widget()
{
    return m_editor;
}

QToolBar* SyntaxEditor::toolBar() const
{
    return m_toolBar;
}

QString SyntaxEditor::displayName() const
{
    QFileInfo info(m_file->fileName());
    return info.fileName();
}

QIcon SyntaxEditor::icon() const
{
    return QIcon();
}


void SyntaxEditor::setDisplayName(const QString &title)
{

}

bool SyntaxEditor::createNew(const QString &contents)
{
    return true;
}

bool SyntaxEditor::open(const QString &fileName,const QString &mimeType)
{
    if (!m_file->open(fileName,mimeType)) {
        return false;
    }
    m_editor->moveCursor(QTextCursor::Start);
    return true;
}

bool SyntaxEditor::save()
{
    bool ret = m_file->save(m_file->fileName());
    if (ret) {
        m_editor->document()->setModified(false);
    }
    return ret;
}

bool SyntaxEditor::isModified()
{
    return m_editor->document()->isModified();
}

LiteApi::IFile *SyntaxEditor::file()
{
    return m_file;
}

int SyntaxEditor::line() const
{
    return 0;
}
int SyntaxEditor::column() const
{
    return 0;
}
void SyntaxEditor::gotoLine(int line, int column)
{
    m_editor->gotoLine(line,column);
}

SyntaxTextEditor *SyntaxEditor::editor()
{
    return m_editor;
}
