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
#include "liteenvapi/liteenvapi.h"
#include "editorutil/editorutil.h"
#include <qjson/include/QJson/Parser>
#include <qjson/include/QJson/Serializer>
#include <QPlainTextEdit>
#include <QProcess>
#include <QToolBar>
#include <QToolButton>
#include <QTextCodec>
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

JsonEdit::JsonEdit(LiteApi::IApplication *app, QObject *parent)
    : QObject(parent), m_liteApp(app)
{
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"JsonEdit");

    m_verifyAct = new QAction(tr("Verify"),this);
    actionContext->regAction(m_verifyAct,"VerifyJson","");
    m_formatAct = new QAction(tr("Format Json"),this);
    actionContext->regAction(m_formatAct,"FormatJson","Ctrl+I");
    m_compactAct = new QAction(tr("Compact Json"),this);
    actionContext->regAction(m_compactAct,"CompactJson","");

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(m_verifyAct,SIGNAL(triggered()),this,SLOT(verify()));
    connect(m_formatAct,SIGNAL(triggered()),this,SLOT(format()));
    connect(m_compactAct,SIGNAL(triggered()),this,SLOT(compact()));
}

//void JsonEdit::indentNone()
//{
//    format(QJson::IndentNone);
//}

//void JsonEdit::indentCompact()
//{
//    format(QJson::IndentCompact);
//}

//void JsonEdit::indentMinimum()
//{
//    format(QJson::IndentMinimum);
//}

//void JsonEdit::indentMedium()
//{
//    format(QJson::IndentMedium);
//}
//void JsonEdit::indentFull()
//{
//    format(QJson::IndentFull);
//}

bool JsonEdit::verifyJson(LiteApi::IEditor *editor)
{
    LiteApi::ILiteEditor *ed = LiteApi::getLiteEditor(editor);
    if (!ed) {
        return false;
    }
    QJson::Parser parser;
    bool ok = true;
    parser.parse(ed->utf8Data(),&ok);
    if (!ok) {
        ed->setNavigateHead(LiteApi::EditorNavigateError,"json verify error");
        if (parser.errorLine() > 0) {
            ed->insertNavigateMark(parser.errorLine()-1,LiteApi::EditorNavigateError,parser.errorString(),"Json");
        }
    } else {
        ed->setNavigateHead(LiteApi::EditorNavigateNormal,"json verify success");
        ed->clearAllNavigateMark(LiteApi::EditorNavigateError,"Json");
    }
    return ok;
}

void JsonEdit::verify()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor || editor->mimeType() != "application/json") {
        return;
    }
    verifyJson(editor);
}

void JsonEdit::format()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor || editor->mimeType() != "application/json") {
        return;
    }
    if (!verifyJson(editor)) {
        return;
    }
    fmtEditor(editor,false,false,true,1000);
}

void JsonEdit::compact()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor || editor->mimeType() != "application/json") {
        return;
    }
    if (!verifyJson(editor)) {
        return;
    }
    fmtEditor(editor,true,false,true,1000);
}

void JsonEdit::fmtEditor(LiteApi::IEditor *editor, bool compact, bool tabs, bool diff,int timeout)
{
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    bool save = !editor->isModified();

    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }

    QString text = ed->toPlainText();
    if (text.isEmpty()) {
        return;
    }

    QProcess process;
    process.setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());

    QStringList args;

    args << "jsonfmt";
    if (compact) {
        args << "-c";
    }
    if (tabs) {
        args << "-tabs";
    }
    if (diff) {
        args << "-d";
    }
    QString cmd = LiteApi::getGotools(m_liteApp);
    process.start(cmd,args);
    if (!process.waitForStarted(timeout)) {
        m_liteApp->appendLog("jsonfmt",QString("Timed out after %1ms when starting json format").arg(timeout),false);
        return;
    }
    process.write(text.toUtf8());
    process.closeWriteChannel();
    if (!process.waitForFinished(timeout*4)) {
        m_liteApp->appendLog("jsonfmt",QString("Timed out after %1ms while running json format").arg(timeout*4),false);
        return;
    }
    LiteApi::ILiteEditor *liteEditor = LiteApi::getLiteEditor(editor);
    liteEditor->clearAllNavigateMark(LiteApi::EditorNavigateBad, "Json");
    QTextCodec *codec = QTextCodec::codecForName("utf-8");

    if (process.exitCode() != 0) {
        m_liteApp->appendLog("jsonfmt",QString::fromUtf8(process.readAll()),true);
        return;
    }

    QByteArray data = process.readAllStandardOutput();
    /*
    int vpos = -1;
    QScrollBar *bar = ed->verticalScrollBar();
    if (bar) {
        vpos = bar->sliderPosition();
    }
    */
    QByteArray state = editor->saveState();
    QTextCursor cur = ed->textCursor();
    //int pos = cur.position();
    cur.beginEditBlock();
    if (diff) {
        EditorUtil::loadDiff(cur,codec->toUnicode(data));
    } else {
        cur.select(QTextCursor::Document);
        cur.removeSelectedText();
        cur.insertText(codec->toUnicode(data));
    }
    //cur.setPosition(pos);
    cur.endEditBlock();
    ed->setTextCursor(cur);
    editor->restoreState(state);

    //ed->setTextCursor(cur);
    //if (vpos != -1) {
    //    bar->setSliderPosition(vpos);
    //}
    if (save) {
        m_liteApp->editorManager()->saveEditor(editor,false);
    }
}

void JsonEdit::editorCreated(LiteApi::IEditor *editor)
{
    if (editor->mimeType() != "application/json") {
        return;
    }
    LiteApi::ILiteEditor *ed = LiteApi::getLiteEditor(editor);
    if (!ed) {
        return;
    }
    ed->setWordWrap(true);
    ed->setEnableAutoIndentAction(false);

    QMenu *menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_verifyAct);
        menu->addAction(m_formatAct);
        menu->addAction(m_compactAct);
    }
    menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_verifyAct);
        menu->addAction(m_formatAct);
        menu->addAction(m_compactAct);
    }
}

void JsonEdit::editorSaved(LiteApi::IEditor *editor)
{
    if (editor->mimeType() != "application/json") {
        return;
    }
    verifyJson(editor);
}

//void JsonEdit::format(QJson::IndentMode mode)
//{
//    QJson::Parser parser;
//    bool ok = true;
//    QVariant json = parser.parse(m_editor->utf8Data(),&ok);
//    if (!ok) {
//        return;
//    }
//    QJson::Serializer stream;
//    stream.setIndentMode(mode);
//    QByteArray data = stream.serialize(json);
//    QPlainTextEdit *ed = LiteApi::getPlainTextEdit(m_editor);
//    if (ed) {
//        QTextCursor cursor = ed->textCursor();
//        cursor.beginEditBlock();
//        cursor.select(QTextCursor::Document);
//        cursor.removeSelectedText();
//        cursor.insertText(QString::fromUtf8(data));
//        cursor.endEditBlock();
//        ed->setTextCursor(cursor);
//    }
//}
