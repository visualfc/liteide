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
// Module: golangfmt.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangfmt.h"
#include "golangfmt_global.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "litebuildapi/litebuildapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "qtc_texteditor/basetextdocumentlayout.h"

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
    m_fixImports(false),
    m_autofmt(true),
    m_syncfmt(false),
    m_timeout(600)
{
    m_process = new ProcessEx(this);
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(fmtOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(started()),this,SLOT(fmtStarted()));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(fmtFinish(bool,int,QString)));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
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
    m_fixImports = m_liteApp->settings()->value(GOLANGFMT_FIXIMPORTS,false).toBool();
    m_autofmt = m_liteApp->settings()->value(GOLANGFMT_AUTOFMT,true).toBool();
    m_syncfmt = m_liteApp->settings()->value(GOLANGFMT_USESYNCFMT,true).toBool();
    m_timeout = m_liteApp->settings()->value(GOLANGFMT_SYNCTIMEOUT,500).toInt();
}

void GolangFmt::syncfmtEditor(LiteApi::IEditor *editor, bool save, bool check, int timeout, int fmtStyle)
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

    if (timeout < 0) {
        timeout = m_timeout;
    }

    QProcess process;
    process.setProcessEnvironment(LiteApi::getGoEnvironment(m_liteApp));

    QStringList args;
    args << "gofmt";
    //format style 0: auto, 1: gofmt 2: fiximports
    if (fmtStyle == 0) {
        if (m_fixImports) {
            args << "-fiximports";
        }
    } else if (fmtStyle == 1) {
    } else if (fmtStyle == 2) {
        args << "-fiximports";
    }
    if (m_diff) {
        args << "-d";
    }

    QString cmd = LiteApi::getGotools(m_liteApp);
    process.start(cmd,args);

    if (!process.waitForStarted(timeout)) {
        m_liteApp->appendLog("gofmt",QString("Timed out after %1ms when starting go code format").arg(timeout),false);
        return;
    }
    process.write(text.toUtf8());
    process.closeWriteChannel();
    if (!process.waitForFinished(timeout*4)) {
        m_liteApp->appendLog("gofmt",QString("Timed out after %1ms while running go code format").arg(timeout*4),false);
        return;
    }
    LiteApi::ILiteEditor *liteEditor = LiteApi::getLiteEditor(editor);
    liteEditor->clearAllNavigateMark(LiteApi::EditorNavigateBad, GOLANGFMT_TAG);
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    if (process.exitCode() != 0) {
        QByteArray error = process.readAllStandardError();
        QString errmsg = codec->toUnicode(error);
        if (!errmsg.isEmpty()) {
            //<standard input>:23:1: expected declaration, found 'INT' 1
             foreach(QString msg,errmsg.split("\n")) {
                QRegExp re(":(\\d+):");
                if (re.indexIn(msg,16) >= 0) {
                    bool ok = false;
                    int line = re.cap(1).toInt(&ok);
                    if (ok) {
                        liteEditor->insertNavigateMark(line-1,LiteApi::EditorNavigateError,msg.mid(16), GOLANGFMT_TAG);
                    }
                }
            }
        }
        QString log = errmsg;
        errmsg.replace("<standard input>","");
        liteEditor->setNavigateHead(LiteApi::EditorNavigateError,"go code format error\n"+errmsg);
        log.replace("<standard input>",info.filePath());
        m_liteApp->appendLog("go code format error",log,fmtStyle == 0 ? false: true);
        return;
    }
    liteEditor->setNavigateHead(LiteApi::EditorNavigateNormal,"go code format success");

    QByteArray data = process.readAllStandardOutput();

//    QByteArray state = editor->saveState();
//    QTextCursor cur = ed->textCursor();
//    cur.beginEditBlock();
//    bool bUseState = true;
//    if (m_diff) {
//        bUseState = false;
//        loadDiff(cur,codec->toUnicode(data));
//    } else {
//        cur.select(QTextCursor::Document);
//        cur.removeSelectedText();
//        cur.insertText(codec->toUnicode(data));
//    }
//    cur.endEditBlock();
//    ed->setTextCursor(cur);
//    if (bUseState) {
//        editor->restoreState(state);
//    }
    liteEditor->loadDiff(codec->toUnicode(data));

    if (save) {
        m_liteApp->editorManager()->saveEditor(editor,false);
    }
}

void GolangFmt::fmtEditor(LiteApi::IEditor *editor, bool save)
{
    if (!editor) {
        return;
    }
    if (editor->filePath().isEmpty()) {
        return;
    }
    QFileInfo info(editor->filePath());
    if (info.suffix() != "go") {
        return;
    }
    if (!m_process->isStop()) {
        return;
    }
    QPlainTextEdit *edit = LiteApi::getPlainTextEdit(editor);
    if (!edit) {
        return;
    }
    m_data.clear();;
    m_errData.clear();
    m_process->setUserData(0,editor->filePath());
    m_process->setUserData(1,edit->toPlainText());
    m_process->setUserData(2,save);

    QStringList args;
    args << "gofmt";
    if (m_fixImports) {
        args << "-fiximports";
    }
    if (m_diff) {
        args << "-d";
    }
    QString cmd = LiteApi::getGotools(m_liteApp);
    m_process->start(cmd,args);
}

void GolangFmt::editorAboutToSave(LiteApi::IEditor* editor)
{
    if (!m_autofmt) {
        return;
    }
    if (m_syncfmt) {
        syncfmtEditor(editor,true);
    } else {
        fmtEditor(editor,true);
    }
}

void GolangFmt::currentEnvChanged(LiteApi::IEnv*)
{
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    m_process->setProcessEnvironment(env);

//    if (!LiteApi::hasGoEnv(env)) {
//        m_liteApp->appendLog("GolangFmt","Could not find go env", false);
//        return;
//    }
//    m_liteApp->appendLog("GolangFmt","go environment changed");
//    m_gofmtCmd = FileUtil::lookupGoBin("gofmt",m_liteApp,false);
//    if (m_gofmtCmd.isEmpty()) {
//        m_liteApp->appendLog("GolangFmt",QString("Could not find %1").arg(m_gofmtCmd),false);
//    }
}

void GolangFmt::gofmt()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    m_liteApp->editorManager()->saveEditor(editor,false);
    syncfmtEditor(editor,false,true,30000,1);
}

void GolangFmt::goimports()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    m_liteApp->editorManager()->saveEditor(editor,false);
    syncfmtEditor(editor,false,true,30000,2);
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
    QString fileName = m_process->userData(0).toString();
    if (fileName.isEmpty()) {
        return;
    }
    bool save = m_process->userData(2).toBool();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->findEditor(fileName,true);
    if (!editor) {
        return;
    }
    LiteApi::ILiteEditor *liteEditor = LiteApi::getLiteEditor(editor);
    if (!liteEditor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::getPlainTextEdit(editor);
    if (!ed) {
        return;
    }
    //qDebug() << ed->document()->isModified();
    if (ed->document()->isModified()) {
        return;
    }
    liteEditor->clearAllNavigateMark(LiteApi::EditorNavigateBad, GOLANGFMT_TAG);
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    if (!error && code == 0) {
        liteEditor->setNavigateHead(LiteApi::EditorNavigateNormal,"go code format success");

        liteEditor->loadDiff(codec->toUnicode(m_data));

        if (save) {
            m_liteApp->editorManager()->saveEditor(editor,false);
        }
    } else if (!m_errData.isEmpty()) {
        QString errmsg = codec->toUnicode(m_errData);
        if (!errmsg.isEmpty()) {
            //<standard input>:23:1: expected declaration, found 'INT' 1
            foreach(QString msg,errmsg.split("\n")) {
                QRegExp re(":(\\d+):");
                if (re.indexIn(msg,16) >= 0) {
                    bool ok = false;
                    int line = re.cap(1).toInt(&ok);
                    if (ok) {
                        liteEditor->insertNavigateMark(line-1,LiteApi::EditorNavigateError,msg.mid(16), GOLANGFMT_TAG);
                    }
                }
            }
        }
        QString log = errmsg;
        errmsg.replace("<standard input>","");
        liteEditor->setNavigateHead(LiteApi::EditorNavigateError,"go code format error\n"+errmsg);
        log.replace("<standard input>",fileName);
        m_liteApp->appendLog("go code format error",log,false);
    }
    m_data.clear();
}
