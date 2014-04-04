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
// Module: golangedit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangedit.h"
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTextStream>
#include <QApplication>
#include <QToolTip>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangEdit::GolangEdit(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent), m_liteApp(app)
{
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GolangEdit");

    m_findInfoAct = new QAction(tr("View Expression Information"),this);
    actionContext->regAction(m_findInfoAct,"ViewInfo",QKeySequence::HelpContents);

    m_jumpDeclAct = new QAction(tr("Jump to Declaration"),this);
    actionContext->regAction(m_jumpDeclAct,"JumpToDeclaration","F2");

    m_findDefProcess = new ProcessEx(this);
    m_findInfoProcess = new ProcessEx(this);

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    connect(m_findInfoAct,SIGNAL(triggered()),this,SLOT(editorFindInfo()));
    connect(m_jumpDeclAct,SIGNAL(triggered()),this,SLOT(editorJumpToDecl()));
    connect(m_findDefProcess,SIGNAL(started()),this,SLOT(findDefStarted()));
    connect(m_findDefProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findDefOutput(QByteArray,bool)));
    connect(m_findDefProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findDefFinish(bool,int,QString)));
    connect(m_findInfoProcess,SIGNAL(started()),this,SLOT(findInfoStarted()));
    connect(m_findInfoProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findInfoOutput(QByteArray,bool)));
    connect(m_findInfoProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findInfoFinish(bool,int,QString)));
//    foreach (QKeySequence ks, m_findInfoAct->shortcuts()) {
//        connect(ks,SIGNAL())
//    }
}

bool GolangEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_plainTextEdit && event->type() == QEvent::ShortcutOverride) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
//        int key = keyEvent->key();
//        Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
//        if (modifiers & Qt::ShiftModifier) {
//            key += Qt::SHIFT;
//        }
//        if (modifiers & Qt::ControlModifier) {
//            key += Qt::CTRL;
//        }
//        if (modifiers & Qt::AltModifier) {
//            key += Qt::ALT;
//        }
//        QKeySequence ks(key);
//        if (m_findInfoAct->shortcuts().contains(ks)) {
//            event->accept();
//        }
        if ((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->text().isEmpty()) {
            findEditorCursorInfo(m_editor,this->textCursorForPos(QCursor::pos()));
        }
        return true;
    }
    return QObject::eventFilter(obj,event);
}

void GolangEdit::findEditorCursorInfo(LiteApi::ITextEditor *editor, const QTextCursor &cursor)
{
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    QString src = cursor.document()->toPlainText();
    m_srcData = src.toUtf8();
    int offset = src.left(cursor.position()).length();
    m_lastCursor = cursor;
    QFileInfo info(editor->filePath());
    m_findInfoProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findInfoProcess->setWorkingDirectory(info.path());
    m_findInfoProcess->startEx(cmd,QString("type -cursor %1:%2 -cursor_stdin -info .").
                             arg(info.fileName()).
                               arg(offset));
}

QTextCursor GolangEdit::textCursorForPos(const QPoint &globalPos)
{
    QPoint pos = m_plainTextEdit->viewport()->mapFromGlobal(globalPos);
    if (!m_plainTextEdit->viewport()->rect().contains(pos)) {
        return m_plainTextEdit->textCursor();
    }

    QTextCursor cur = m_plainTextEdit->textCursor();
    QRect rc = m_plainTextEdit->cursorRect(cur);
    if (rc.contains(pos)) {
        return cur;
    }
    cur = m_plainTextEdit->cursorForPosition(pos);
    if (!cur.isNull()) {
        return cur;
    }
    return m_plainTextEdit->textCursor();
}

void GolangEdit::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor || editor->mimeType() != "text/x-gosrc") {
        return;
    }
    QPlainTextEdit *ed = LiteApi::getPlainTextEdit(editor);
    if (ed) {
        ed->installEventFilter(this);
    }

    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_findInfoAct);
        menu->addAction(m_jumpDeclAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_findInfoAct);
        menu->addAction(m_jumpDeclAct);
    }
}

void GolangEdit::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!editor || editor->mimeType() != "text/x-gosrc") {
        return;
    }
    m_editor = LiteApi::getTextEditor(editor);
    m_plainTextEdit = LiteApi::getPlainTextEdit(editor);
}

void GolangEdit::editorJumpToDecl()
{
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    m_srcData = m_editor->utf8Data();
    m_findDefData.clear();
    QFileInfo info(m_editor->filePath());
    m_findDefProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findDefProcess->setWorkingDirectory(info.path());
    m_findDefProcess->startEx(cmd,QString("type -cursor %1:%2 -cursor_stdin -def .").
                             arg(info.fileName()).
                             arg(m_editor->utf8Position()));
}

void GolangEdit::editorFindInfo()
{
    QTextCursor cursor = this->textCursorForPos(QCursor::pos());
    if (cursor.isNull()) {
        return;
    }
    this->findEditorCursorInfo(m_editor,m_plainTextEdit->textCursor());
}

void GolangEdit::findDefStarted()
{
    m_findDefProcess->write(m_srcData);
    m_findDefProcess->closeWriteChannel();
}

void GolangEdit::findDefOutput(QByteArray data, bool bStdErr)
{
    if (bStdErr) {
        return;
    }
    QString info = QString::fromUtf8(data).trimmed();
    QRegExp reg(":(\\d+):(\\d+)");
    int pos = reg.lastIndexIn(info);
    if (pos >= 0) {
        QString fileName = info.left(pos);
        int line = reg.cap(1).toInt();
        int col = reg.cap(2).toInt();
        LiteApi::gotoLine(m_liteApp,fileName,line-1,col-1);
    }
}

void GolangEdit::findDefFinish(bool error, int code, QString)
{
//    if (!error && code == 0) {
//        QTextStream s(&m_findDefData);
//        while (!s.atEnd()) {
//            QString line = s.readLine();
//            if (line.startsWith("pos,")) {
//                QString info = line.mid(4).trimmed();
//                QRegExp reg(":(\\d+):(\\d+)");
//                int pos = reg.lastIndexIn(info);
//                if (pos >= 0) {
//                    QString fileName = info.left(pos);
//                    int line = reg.cap(1).toInt();
//                    int col = reg.cap(2).toInt();
//                    LiteApi::gotoLine(m_liteApp,fileName,line-1,col-1);
//                }
//            }
//        }
//    }
}

void GolangEdit::findInfoStarted()
{
    m_findInfoProcess->write(m_srcData);
    m_findInfoProcess->closeWriteChannel();
}

void GolangEdit::findInfoOutput(QByteArray data, bool bStdErr)
{
    if (bStdErr) {
        return;
    }

    if ( m_editor == m_liteApp->editorManager()->currentEditor()) {
        QTextCursor cur = this->textCursorForPos(QCursor::pos());
        if (cur == m_lastCursor || m_plainTextEdit->textCursor() == m_lastCursor) {
            QString info = QString::fromUtf8(data).trimmed();
            QRect rc = m_plainTextEdit->cursorRect(m_lastCursor);
            QPoint pt = m_plainTextEdit->mapToGlobal(rc.topRight());
            QToolTip::showText(pt,info,m_plainTextEdit);
        }
    }
}

void GolangEdit::findInfoFinish(bool error, int code, QString)
{
//    if (!error && code == 0) {
//    }
}
