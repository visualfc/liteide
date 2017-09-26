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
#include "diff_match_patch/diff_match_patch.h"
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
    m_sorImports(true),
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
    m_sorImports = m_liteApp->settings()->value(GOLANGFMT_SORTIMPORTS,true).toBool();
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
    if (m_sorImports) {
        args << "-sortimports";
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

    QByteArray state = editor->saveState();
    QTextCursor cur = ed->textCursor();
    cur.beginEditBlock();
    bool bUseState = true;
    if (m_diff) {
        bUseState = false;
        loadDiff(cur,codec->toUnicode(data));        
    } else {
        cur.select(QTextCursor::Document);
        cur.removeSelectedText();
        cur.insertText(codec->toUnicode(data));
    }
    cur.endEditBlock();
    ed->setTextCursor(cur);
    if (bUseState) {
        editor->restoreState(state);
    }

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
    if (m_sorImports) {
        args << "-sortimports";
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

        QByteArray state = editor->saveState();
        QTextCursor cur = ed->textCursor();
        cur.beginEditBlock();
        bool bUseState = true;
        if (m_diff) {
            bUseState = false;
            loadDiff(cur,codec->toUnicode(m_data));
        } else {
            cur.select(QTextCursor::Document);
            cur.removeSelectedText();
            cur.insertText(codec->toUnicode(m_data));
        }
        cur.endEditBlock();
        ed->setTextCursor(cur);
        if (bUseState) {
            editor->restoreState(state);
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

// use diff_match_patch
int findBlockPos(const QString &orgText, const QString &newText, int pos )
{
    diff_match_patch dmp;
    QList<Diff> diffs = dmp.diff_main(orgText,newText,false);
    return dmp.diff_xIndex(diffs,pos);
}

int findBlockNumber(const QList<int> &offsetList, int offsetBase, int blockNumber)
{
    for (int i = offsetList.size()-1; i>=0; i--) {
        int iv = offsetList[i];
        if (iv == -1) {
            continue;
        }
        if (blockNumber >= iv) {
            if (blockNumber == iv) {
                return offsetBase+i;
            } else {
                if (i == offsetList.size()-1) {
                    return offsetBase+i+blockNumber-iv;
                }
                int offset = i;
                int v0 = iv;
                for (int j = i+1; j < offsetList.size(); j++) {
                    if (offsetList[j] != -1) {
                        break;
                    }
                    offset++;
                    v0++;
                    if (v0 == blockNumber) {
                        break;
                    }
                }
                return offsetBase+offset;
            }
        }
    }
    return blockNumber;
}

void GolangFmt::loadDiff(QTextCursor &cursor, const QString &diff)
{
    //save org block
    int orgBlockNumber = cursor.blockNumber();
    int orgPosInBlock = cursor.positionInBlock();
    QString orgBlockText = cursor.block().text();
    int curBlockNumber = orgBlockNumber;

    //load diff
    QRegExp reg("@@\\s+\\-(\\d+),?(\\d+)?\\s+\\+(\\d+),?(\\d+)?\\s+@@");
    QTextBlock block;
    int line = -1;
    int line_add = 0;
    int block_number = 0;

    QList<int> offsetList;
    int offsetBase = 0;

    QStringList diffList = diff.split("\n");
    QString s;
    int size = diffList.size();

    for (int i = 0; i < size; i++) {
        s = diffList[i];
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
                //block = cursor.document()->findBlockByNumber(line-1);
                line_add += n2-s2;//n2+n1-(s2+s1);
                block_number = line-1;

                //find block number
                curBlockNumber = findBlockNumber(offsetList,offsetBase,curBlockNumber);
                offsetBase = block_number;
                offsetList.clear();
                for (int i = 0; i <= s2; i++) {
                    offsetList.append(offsetBase+i);
                }
                continue;
            }
        }
        if (line == -1) {
            continue;
        }
        if (ch == '+') {
            offsetList.insert(block_number-offsetBase,-1);
            block = cursor.document()->findBlockByNumber(block_number);
            if (!block.isValid()) {
                cursor.movePosition(QTextCursor::End);
                cursor.insertBlock();
                cursor.insertText(s.mid(1));
            } else {
                cursor.setPosition(block.position());
                cursor.insertText(s.mid(1));
                cursor.insertBlock();
            }
            block_number++;
        } else if (ch == '-') {
            //check modify current block text
            if ((i < (size-1)) && diffList[i+1].startsWith("+")) {
                block = cursor.document()->findBlockByNumber(block_number);
                QString nextText = diffList[i+1].mid(1);
                int nSameOfHead = 0;
                bool checkSame = checkTowStringHead(nextText.simplified(),block.text().simplified(),nSameOfHead);
                if (checkSame || (nSameOfHead >= 4) ) {
                    cursor.setPosition(block.position());
                    cursor.insertText(nextText);
                    cursor.setPosition(block.position()+nextText.length());
                    cursor.setPosition(block.position()+block.text().length(), QTextCursor::KeepAnchor);
                    cursor.removeSelectedText();
                    i++;
                    block_number++;
                    continue;
                }
            }

            offsetList.removeAt(block_number-offsetBase);
            block = cursor.document()->findBlockByNumber(block_number);
            cursor.setPosition(block.position());
            if (block.next().isValid()) {
                cursor.setPosition(block.next().position(), QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
            } else {
                cursor.movePosition(QTextCursor::EndOfBlock);
                cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
            }
        } else if (ch == ' ') {
            block_number++;
        } else if (ch == '\\') {
            //skip comment
        }
    }
    //find block number
    curBlockNumber = findBlockNumber(offsetList,offsetBase,curBlockNumber);
    //load cur block
    block = cursor.document()->findBlockByNumber(curBlockNumber);
    if (block.isValid()) {
        cursor.setPosition(block.position());
        int column = findBlockPos(orgBlockText,block.text(),orgPosInBlock);
        if (column > 0) {
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
        }
    }
}

