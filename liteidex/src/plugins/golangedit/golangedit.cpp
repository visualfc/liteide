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

    m_findUseAct = new QAction(tr("Find Usages"),this);
    actionContext->regAction(m_findUseAct,"FindUsages","");

    m_fileSearch = new GolangFileSearch(app,this);

    LiteApi::IFileSearchManager *manager = LiteApi::getFileSearchManager(app);
    if (manager) {
        manager->addFileSearch(m_fileSearch);
    }

    m_findDefProcess = new ProcessEx(this);
    m_findInfoProcess = new ProcessEx(this);
    m_findLinkProcess = new ProcessEx(this);

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    connect(m_findInfoAct,SIGNAL(triggered()),this,SLOT(editorFindInfo()));
    connect(m_jumpDeclAct,SIGNAL(triggered()),this,SLOT(editorJumpToDecl()));
    connect(m_findUseAct,SIGNAL(triggered()),this,SLOT(editorFindUsages()));
    connect(m_findDefProcess,SIGNAL(started()),this,SLOT(findDefStarted()));
    connect(m_findDefProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findDefOutput(QByteArray,bool)));
    connect(m_findDefProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findDefFinish(bool,int,QString)));
    connect(m_findInfoProcess,SIGNAL(started()),this,SLOT(findInfoStarted()));
    connect(m_findInfoProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findInfoOutput(QByteArray,bool)));
    connect(m_findInfoProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findInfoFinish(bool,int,QString)));
    connect(m_findLinkProcess,SIGNAL(started()),this,SLOT(findLinkStarted()));
    connect(m_findLinkProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findLinkOutput(QByteArray,bool)));
    connect(m_findLinkProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findLinkFinish(bool,int,QString)));
}

//bool GolangEdit::eventFilter(QObject *obj, QEvent *event)
//{
//    if (obj == m_plainTextEdit && event->type() == QEvent::ShortcutOverride) {
//        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
////        int key = keyEvent->key();
////        Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
////        if (modifiers & Qt::ShiftModifier) {
////            key += Qt::SHIFT;
////        }
////        if (modifiers & Qt::ControlModifier) {
////            key += Qt::CTRL;
////        }
////        if (modifiers & Qt::AltModifier) {
////            key += Qt::ALT;
////        }
////        QKeySequence ks(key);
////        if (m_findInfoAct->shortcuts().contains(ks)) {
////            event->accept();
////        }
//        if ((mouseEvent->modifiers() & Qt::ControlModifier)) {
//            //findEditorCursorInfo(m_editor,this->textCursorForPos(QCursor::pos()));
//            return true;
//        }
//    }
//    return QObject::eventFilter(obj,event);
//}

QTextCursor GolangEdit::textCursorForPos(const QPoint &globalPos)
{
    QPoint pos = m_plainTextEdit->viewport()->mapFromGlobal(globalPos);
    QTextCursor cur = m_plainTextEdit->textCursor();
    QRect rc = m_plainTextEdit->cursorRect(cur);
    if (rc.contains(pos)) {
        return cur;
    }
    return m_plainTextEdit->cursorForPosition(pos);
}

void GolangEdit::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor || editor->mimeType() != "text/x-gosrc") {
        return;
    }
    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_findInfoAct);
        menu->addAction(m_jumpDeclAct);
        menu->addAction(m_findUseAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_findInfoAct);
        menu->addAction(m_jumpDeclAct);
        menu->addAction(m_findUseAct);
    }
    m_editor = LiteApi::getLiteEditor(editor);
    if (m_editor) {
        connect(m_editor,SIGNAL(updateLink(QTextCursor)),this,SLOT(updateLink(QTextCursor)));
    }
}

void GolangEdit::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!editor || editor->mimeType() != "text/x-gosrc") {
        return;
    }
    m_editor = LiteApi::getLiteEditor(editor);
    m_plainTextEdit = LiteApi::getPlainTextEdit(editor);
}

void GolangEdit::updateLink(const QTextCursor &_cursor)
{
    QTextCursor cursor = _cursor;
    LiteApi::selectWordUnderCursor(cursor);

    if (cursor.selectionStart() == cursor.selectionEnd()) {
        return;
    }

    if (m_linkCursor.selectionStart() == cursor.selectionStart() &&
            m_linkCursor.selectionEnd() == cursor.selectionEnd()) {
        if (m_lastLink.hasValidTarget()) {
            m_editor->showLink(m_lastLink);
        }
        return;
    }
    if (m_findLinkProcess->isRunning()) {
        return;
    }
    m_linkCursor = cursor;
    m_lastLink = LiteApi::Link();
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    QString src = cursor.document()->toPlainText();
    m_srcData = src.toUtf8();
    int offset = src.left(cursor.selectionStart()).length();
    QFileInfo info(m_editor->filePath());
    m_findLinkProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findLinkProcess->setWorkingDirectory(info.path());
    m_findLinkProcess->startEx(cmd,QString("type -cursor %1:%2 -cursor_stdin -def -info .").
                             arg(info.fileName()).
                               arg(offset));
}

void GolangEdit::editorJumpToDecl()
{
    bool moveLeft = false;
    QString text = LiteApi::wordUnderCursor(m_plainTextEdit->textCursor(),&moveLeft);
    if (text.isEmpty()) {
        return;
    }
    m_lastCursor = m_plainTextEdit->textCursor();
    int offset = moveLeft?m_editor->utf8Position()-1:m_editor->utf8Position();
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    m_srcData = m_editor->utf8Data();
    QFileInfo info(m_editor->filePath());
    m_findDefProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findDefProcess->setWorkingDirectory(info.path());
    m_findDefProcess->startEx(cmd,QString("type -cursor %1:%2 -cursor_stdin -def .").
                             arg(info.fileName()).
                              arg(offset));
}

void GolangEdit::editorFindUsages()
{
    QTextCursor cursor = m_plainTextEdit->textCursor();
    m_fileSearch->findUsages(m_editor,cursor);
}

void GolangEdit::editorFindInfo()
{
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    m_srcData = m_editor->utf8Data();
    QFileInfo info(m_editor->filePath());
    bool moveLeft = false;
    QString text = LiteApi::wordUnderCursor(m_plainTextEdit->textCursor(),&moveLeft);
    if (text.isEmpty()) {
        return;
    }
    m_lastCursor = m_plainTextEdit->textCursor();
    int offset = moveLeft?m_editor->utf8Position()-1:m_editor->utf8Position();
    m_findInfoProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findInfoProcess->setWorkingDirectory(info.path());
    m_findInfoProcess->startEx(cmd,QString("type -cursor %1:%2 -cursor_stdin -info .").
                             arg(info.fileName()).
                             arg(offset));
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

void GolangEdit::findDefFinish(bool, int, QString)
{
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
        if (m_plainTextEdit->textCursor() == m_lastCursor) {
            QString info = QString::fromUtf8(data).trimmed();
            QRect rc = m_plainTextEdit->cursorRect(m_lastCursor);
            QPoint pt = m_plainTextEdit->mapToGlobal(rc.topRight());
            QToolTip::showText(pt,info,m_plainTextEdit);
        }
    }
}

void GolangEdit::findInfoFinish(bool /*error*/, int /*code*/, QString)
{
}

void GolangEdit::findLinkStarted()
{
    m_findLinkProcess->write(m_srcData);
    m_findLinkProcess->closeWriteChannel();
}

void GolangEdit::findLinkOutput(QByteArray data, bool bStdErr)
{
    if (bStdErr) {
        return;
    }

    if ( m_editor == m_liteApp->editorManager()->currentEditor()) {
        QTextCursor cur = this->textCursorForPos(QCursor::pos());
        cur.select(QTextCursor::WordUnderCursor);
        if (cur.selectionStart() == m_linkCursor.selectionStart() &&
                cur.selectionEnd() == m_linkCursor.selectionEnd()) {
            QStringList info = QString::fromUtf8(data).trimmed().split("\n");
            if (info.size() == 2) {
                if (info[0] != "-") {
                    QRegExp reg(":(\\d+):(\\d+)");
                    int pos = reg.lastIndexIn(info[0]);
                    if (pos >= 0) {
                        QString fileName = info[0].left(pos);
                        int line = reg.cap(1).toInt();
                        int col = reg.cap(2).toInt();
                        LiteApi::Link link(fileName,line-1,col-1);
                        m_lastCursor.select(QTextCursor::WordUnderCursor);
                        link.linkTextStart = m_linkCursor.selectionStart();
                        link.linkTextEnd = m_linkCursor.selectionEnd();
                        m_lastLink = link;
                        m_editor->showLink(link);
                    }
                }
                QRect rc = m_plainTextEdit->cursorRect(m_linkCursor);
                QPoint pt = m_plainTextEdit->mapToGlobal(rc.topRight());
                QToolTip::showText(pt,info[1],m_plainTextEdit);
            }
        }
    }
}

void GolangEdit::findLinkFinish(bool /*error*/, int /*code*/, QString)
{
}

