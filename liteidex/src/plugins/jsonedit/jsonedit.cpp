/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: jsonedit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "jsonedit.h"
#include <qjson/include/QJson/Parser>
#include <qjson/include/QJson/Serializer>
#include <QToolBar>
#include <QToolButton>
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

JsonEdit::JsonEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent)
    : QObject(parent), m_liteApp(app)
{
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"JsonEdit");

    QAction *verify = new QAction(QIcon("icon:golangpresent/images/verify.png"),tr("Verify"),this);
    actionContext->regAction(verify,"Verify Json","");

    /*
    IndentNone,
    IndentCompact,
    IndentMinimum,
    IndentMedium,
    IndentFull
     */
    QAction *indentNone = new QAction(tr("IndentNone"),this);
    QAction *indentCompact = new QAction(tr("IndentCompact"),this);
    QAction *indentMinimum = new QAction(tr("IndentMinimum"),this);
    QAction *indentMedium = new QAction(tr("IndentMedium"),this);
    QAction *indentFull = new QAction(tr("IndentFull"),this);

    QMenu *format = new QMenu(tr("Format"));
    format->addAction(indentNone);
    format->addAction(indentCompact);
    format->addAction(indentMinimum);
    format->addAction(indentMedium);
    format->addAction(indentFull);

    QToolBar *toolBar = LiteApi::getEditToolBar(editor);
    if (toolBar) {
        toolBar->addSeparator();
        toolBar->addAction(verify);
        QToolButton *btn = new QToolButton(toolBar);
        btn->setPopupMode(QToolButton::InstantPopup);
        btn->setText(format->title());
        btn->setMenu(format);
        toolBar->addWidget(btn);
    }

    m_editor = LiteApi::getLiteEditor(editor);
    m_editor->setWordWrap(true);

    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(verify,SIGNAL(triggered()),this,SLOT(verify()));
    connect(indentNone,SIGNAL(triggered()),this,SLOT(indentNone()));
    connect(indentCompact,SIGNAL(triggered()),this,SLOT(indentCompact()));
    connect(indentMinimum,SIGNAL(triggered()),this,SLOT(indentMinimum()));
    connect(indentMedium,SIGNAL(triggered()),this,SLOT(indentMedium()));
    connect(indentFull,SIGNAL(triggered()),this,SLOT(indentFull()));
}

void JsonEdit::indentNone()
{
    format(QJson::IndentNone);
}

void JsonEdit::indentCompact()
{
    format(QJson::IndentCompact);
}

void JsonEdit::indentMinimum()
{
    format(QJson::IndentMinimum);
}

void JsonEdit::indentMedium()
{
    format(QJson::IndentMedium);
}
void JsonEdit::indentFull()
{
    format(QJson::IndentFull);
}

void JsonEdit::editorSaved(LiteApi::IEditor *editor)
{
    if (editor == m_editor) {
        verify();
    }
}

void JsonEdit::verify()
{
    QJson::Parser parser;
    bool ok = true;
    parser.parse(m_editor->utf8Data(),&ok);
    if (!ok) {
        m_editor->setNavigateHead(LiteApi::EditorNavigateError,"json verify error");
        if (parser.errorLine() > 0) {
            m_editor->insertNavigateMark(parser.errorLine()-1,LiteApi::EditorNavigateError,parser.errorString(),"Json");
        }
    } else {
        m_editor->setNavigateHead(LiteApi::EditorNavigateNormal,"json verify success");
        m_editor->clearAllNavigateMark(LiteApi::EditorNavigateError,"Json");
    }
}

void JsonEdit::format(QJson::IndentMode mode)
{
    QJson::Parser parser;
    bool ok = true;
    QVariant json = parser.parse(m_editor->utf8Data(),&ok);
    if (!ok) {
        return;
    }
    QJson::Serializer stream;
    stream.setIndentMode(mode);
    QByteArray data = stream.serialize(json);
    QPlainTextEdit *ed = LiteApi::getPlainTextEdit(m_editor);
    if (ed) {
        QTextCursor cursor = ed->textCursor();
        cursor.beginEditBlock();
        cursor.select(QTextCursor::Document);
        cursor.removeSelectedText();
        cursor.insertText(QString::fromUtf8(data));
        cursor.endEditBlock();
        ed->setTextCursor(cursor);
    }
}
