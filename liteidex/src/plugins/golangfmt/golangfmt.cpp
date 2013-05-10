/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: golangfmt.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangfmt.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "litebuildapi/litebuildapi.h"

#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QPlainTextEdit>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


GolangFmt::GolangFmt(LiteApi::IApplication *app,QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_diff(true),
    m_autofmt(true),
    m_timeout(500)
{
    m_process = new ProcessEx(this);
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(fmtOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(started()),this,SLOT(fmtStarted()));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(fmtFinish(bool,int,QString)));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(m_envManager->currentEnv());
    }
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToSave(LiteApi::IEditor*)),this,SLOT(editorAboutToSave(LiteApi::IEditor*)));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));
    applyOption("option/golangfmt");
}

void GolangFmt::applyOption(QString id)
{
    if (id != "option/golangfmt") {
        return;
    }
    m_diff = m_liteApp->settings()->value("golangfmt/diff",true).toBool();
    m_autofmt = m_liteApp->settings()->value("golangfmt/autofmt",true).toBool();
    m_autopop = m_liteApp->settings()->value("golangfmt/autopop",false).toBool();
    if (!m_diff) {
        m_autofmt = false;
    }
    m_timeout = m_liteApp->settings()->value("golangfmt/timeout",500).toInt();
}

void GolangFmt::syncfmtEditor(LiteApi::IEditor *editor, bool save, bool check, int timeout)
{
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    QString fileName = editor->filePath();
    if (fileName.isEmpty() && check) {
        return;
    }

    QFileInfo info(fileName);
    if (info.suffix() != "go" && check) {
        return;
    }

    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }

    QString text = ed->toPlainText();
    if (text.isEmpty()) {
        return;
    }

    QStringList args;
    if (m_diff) {
        args << "-d";
    }

    if (timeout < 0) {
        timeout = m_timeout;
    }

    QProcess process;
    process.setEnvironment(LiteApi::getCurrentEnvironment(m_liteApp).toStringList());
    process.start(m_gofmtCmd,args);
    if (!process.waitForStarted(timeout)) {
        m_liteApp->appendLog("gofmt",QString("Timed out after %1ms when starting gofmt").arg(timeout),false);
        return;
    }
    process.write(text.toUtf8());
    process.closeWriteChannel();
    if (!process.waitForFinished(timeout)) {
        m_liteApp->appendLog("gofmt",QString("Timed out after %1ms while running gofmt").arg(timeout),false);
        return;
    }
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QByteArray error = process.readAllStandardError();
    if (!error.isEmpty()) {
        QString data = codec->toUnicode(error);
        data.replace("<standard input>",fileName);
        m_liteApp->appendLog("gofmt error","\n"+data,m_autopop);
        return;
        //goto error line
//        QRegExp rep("([\\w\\d_\\\\/\\.]+):(\\d+):");

//        int index = rep.indexIn(data);
//        if (index < 0)
//            return;
//        QStringList capList = rep.capturedTexts();

//        if (capList.count() < 3)
//            return;
//        //QString fileName = capList[1];
//        QString fileLine = capList[2];

//        bool ok = false;
//        int line = fileLine.toInt(&ok);
//        if (!ok)
//            return;
        //textEditor->gotoLine(line-1,0,true);
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
    if (m_diff) {
        loadDiff(cur,codec->toUnicode(data));
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

void GolangFmt::fmtEditor(LiteApi::IEditor *editor, bool save)
{
    if (!editor) {
        return;
    }
    QString fileName = editor->filePath();
    if (fileName.isEmpty()) {
        return;
    }

    QFileInfo info(fileName);
    if (info.suffix() != "go") {
        return;
    }

    if (m_process->isRuning()) {
        return;
    }

    if (m_gofmtCmd.isEmpty()) {
        return;
    }

    QPlainTextEdit *edit = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!edit) {
        return;
    }

    QString text = edit->toPlainText();

    QStringList args;
    if (m_diff) {
        args << "-d";
    }
    m_data.clear();;
    m_errData.clear();
    m_process->setUserData(0,fileName);
    m_process->setUserData(1,text);
    m_process->setUserData(2,save);
    m_process->start(m_gofmtCmd,args);
}

void GolangFmt::editorAboutToSave(LiteApi::IEditor* editor)
{
    if (!m_autofmt) {
        return;
    }
    //fmtEditor(editor,true);
    syncfmtEditor(editor,true);
}

void GolangFmt::currentEnvChanged(LiteApi::IEnv*)
{
    QProcessEnvironment env = m_envManager->currentEnvironment();
    m_gofmtCmd = FileUtil::lookupGoBin("gofmt",m_liteApp);
    m_process->setProcessEnvironment(env);
}

void GolangFmt::gofmt()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    //fmtEditor(editor,false);
    syncfmtEditor(editor,false,true,1500);
}

void GolangFmt::fmtStarted()
{
    QString text = m_process->userData(1).toString();
    m_process->write(text.toUtf8());
    m_process->closeWriteChannel();
}

void GolangFmt::fmtOutput(QByteArray data,bool stdErr)
{
    if (stdErr) {
        m_errData.append(data);
        return;
    }
    m_data.append(data);
}

void GolangFmt::fmtFinish(bool error,int code,QString)
{
    if (!error && code == 0) {
        QString fileName = m_process->userData(0).toString();
        bool save = m_process->userData(2).toBool();
        if (!fileName.isEmpty()) {
            LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,true);
            if (editor) {
                QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
                QTextCodec *codec = QTextCodec::codecForName("utf-8");
                if (ed && codec) {
                    int vpos = -1;
                    QScrollBar *bar = ed->verticalScrollBar();
                    if (bar) {
                        vpos = bar->sliderPosition();
                    }
                    QTextCursor cur = ed->textCursor();
                    int pos = cur.position();
                    cur.beginEditBlock();
                    if (m_diff) {
                        loadDiff(cur,codec->toUnicode(m_data));
                    } else {
                        cur.select(QTextCursor::Document);
                        cur.removeSelectedText();
                        cur.insertText(codec->toUnicode(m_data));
                    }
                    cur.setPosition(pos);
                    cur.endEditBlock();

                    ed->setTextCursor(cur);
                    if (vpos != -1) {
                        bar->setSliderPosition(vpos);
                    }
                }
                if (save) {
                    m_liteApp->editorManager()->saveEditor(editor,false);
                }
            }
        }
    } else if (!m_errData.isEmpty()){
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QString filename = m_process->userData(0).toString();
        QString data = codec->toUnicode(m_errData);
        m_errData.clear();
        data.replace("<standard input>",filename);
        m_liteApp->appendLog("gofmt","\n"+data,true);
        //goto error line
        QRegExp rep("([\\w\\d_\\\\/\\.]+):(\\d+):");

        int index = rep.indexIn(data);
        if (index < 0)
            return;
        QStringList capList = rep.capturedTexts();

        if (capList.count() < 3)
            return;
        QString fileName = capList[1];
        QString fileLine = capList[2];

        bool ok = false;
        int line = fileLine.toInt(&ok);
        if (!ok)
            return;

        LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName);
        if (editor) {
            editor->widget()->setFocus();
            LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
            if (textEditor) {
                textEditor->gotoLine(line,0,true);
            }
        }
    }
    m_data.clear();
}

void GolangFmt::loadDiff(QTextCursor &cursor, const QString &diff)
{
    QRegExp reg("@@\\s+\\-(\\d+),(\\d+)\\s+\\+(\\d+),(\\d+)\\s+@@");
    QTextBlock block;
    int line = -1;
    int line_add = 0;
    foreach(QString s, diff.split('\n')) {
        if (s.length() == 0) {
            continue;
        }
        QChar ch = s.at(0);
        if (ch == '@') {
            if (reg.indexIn(s) == 0) {
                int s1 = reg.cap(1).toInt();
                int s2 = reg.cap(2).toInt();
                //int n1 = reg.cap(3).toInt();
                int n2 = reg.cap(4).toInt();
                line = line_add+s1;
                block = cursor.document()->findBlockByNumber(line-1);
                line_add += n2-s2;//n2+n1-(s2+s1);
                continue;
            }
        }
        if (line == -1) {
            continue;
        }
        if (ch == '+') {
            cursor.setPosition(block.position());
            cursor.insertText(s.right(s.length()-1)+"\n");
            block = cursor.block();
            //break;
        } else if (ch == '-') {
            cursor.setPosition(block.position());
            if (block.next().isValid()) {
                cursor.setPosition(block.next().position(), QTextCursor::KeepAnchor);
            } else {
                cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            }
            cursor.removeSelectedText();
            block = cursor.block();
        } else if (ch == ' ') {
            block = block.next();
        } else if (ch == '\\') {
        }
    }
}
