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

GolangEdit::GolangEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent) :
    QObject(parent), m_liteApp(app)
{
    m_editor = LiteApi::getTextEditor(editor);
    m_plainTextEdit = LiteApi::getPlainTextEdit(editor);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GolangEdit");

    m_findInfoAct = new QAction(tr("View Expression Information"),this);
    actionContext->regAction(m_findInfoAct,"ViewInfo",QKeySequence::HelpContents);

    m_jumpDeclAct = new QAction(tr("Jump to Declaration"),this);
    actionContext->regAction(m_jumpDeclAct,"JumpToDeclaration","F2");

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

    m_findDefProcess = new ProcessEx(this);
    m_findInfoProcess = new ProcessEx(this);

    connect(m_findInfoAct,SIGNAL(triggered()),this,SLOT(editorFindInfo()));
    connect(m_jumpDeclAct,SIGNAL(triggered()),this,SLOT(editorJumpToDecl()));
    connect(m_findDefProcess,SIGNAL(started()),this,SLOT(findDefStarted()));
    connect(m_findDefProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findDefOutput(QByteArray,bool)));
    connect(m_findDefProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findDefFinish(bool,int,QString)));
    connect(m_findInfoProcess,SIGNAL(started()),this,SLOT(findInfoStarted()));
    connect(m_findInfoProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findInfoOutput(QByteArray,bool)));
    connect(m_findInfoProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findInfoFinish(bool,int,QString)));
}

void GolangEdit::editorJumpToDecl()
{
    //m_liteApp->editorManager()->saveEditor(editor,false);
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
    QString cmd = LiteApi::liteide_stub_cmd(m_liteApp);
    m_srcData = m_editor->utf8Data();
    m_lastCursor = m_plainTextEdit->textCursor();
    m_findInfoData.clear();
    QFileInfo info(m_editor->filePath());
    m_findInfoProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findInfoProcess->setWorkingDirectory(info.path());
    m_findInfoProcess->startEx(cmd,QString("type -cursor %1:%2 -cursor_stdin -info .").
                             arg(info.fileName()).
                             arg(m_editor->utf8Position()));
}

void GolangEdit::findDefStarted()
{
    m_findDefProcess->write(m_srcData);
    m_findDefProcess->closeWriteChannel();
}

void GolangEdit::findDefOutput(QByteArray data, bool bStdErr)
{
    if (!bStdErr) {
        //m_findDefData.append(data);
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
    if (!bStdErr) {
        QString info = QString::fromUtf8(data).trimmed();
        if (m_plainTextEdit->textCursor() == m_lastCursor) {
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
