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
// Module: golangfmt.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangfmt.h"
#include "golangfmt_global.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "litebuildapi/litebuildapi.h"
#include "liteeditorapi/liteeditorapi.h"

#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QPlainTextEdit>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
#include "memory.h"

GolangFmt::GolangFmt(LiteApi::IApplication *app,QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_goimports(false),
    m_diff(true),
    m_autofmt(true),
    m_syncfmt(false),
    m_timeout(600)
{
    m_process = new ProcessEx(this);
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(fmtOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(started()),this,SLOT(fmtStarted()));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(fmtFinish(bool,int,QString)));

    m_goimports = m_liteApp->settings()->value(GOLANGFMT_USEGOIMPORTS,false).toBool();
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
    bool goimports = m_liteApp->settings()->value(GOLANGFMT_USEGOIMPORTS,false).toBool();
    m_diff = m_liteApp->settings()->value(GOLANGFMT_USEDIFF,true).toBool();
    m_autofmt = m_liteApp->settings()->value(GOLANGFMT_AUTOFMT,true).toBool();
    if (!m_diff) {
        m_autofmt = false;
    }
    if (goimports != m_goimports) {
        m_goimports = goimports;
        currentEnvChanged(0);
    }
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
    process.setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());

    QStringList args;
    //format style
    if (fmtStyle == 0) {
        if (m_goimports) {
            args << "goimports";
        }
        if (m_diff) {
            args << "-d";
        }
        if (m_goimports) {
            QString cmd = LiteApi::getGotools(m_liteApp);
            process.start(cmd,args);
        } else {
            process.start(m_gofmtCmd,args);
        }
    } else if (fmtStyle == 1) {
        if (m_diff) {
            args << "-d";
        }
        process.start(m_gofmtCmd,args);
    } else if (fmtStyle == 2) {
        args << "goimports";
        if (m_diff) {
            args << "-d";
        }
        QString cmd = LiteApi::getGotools(m_liteApp);
        process.start(cmd,args);
    }
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
    if (editor->filePath().isEmpty()) {
        return;
    }
    QFileInfo info(editor->filePath());
    if (info.suffix() != "go") {
        return;
    }
    if (m_process->isRunning()) {
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
    if (m_goimports) {
        args << "goimports";
    }
    if (m_diff) {
        args << "-d";
    }
    if (m_goimports) {
        QString cmd = LiteApi::getGotools(m_liteApp);
        m_process->start(cmd,args);
    } else {
        m_process->start(m_gofmtCmd,args);
    }
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
    QProcessEnvironment env = m_envManager->currentEnvironment();
    m_gofmtCmd = FileUtil::lookupGoBin("gofmt",m_liteApp,false);
    if (m_gofmtCmd.isEmpty()) {
        m_liteApp->appendLog("GolangFmt",QString("Could not find %1").arg(m_gofmtCmd),false);
    }
    m_process->setProcessEnvironment(env);
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
