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
// Module: golanglint.cpp
// Creator: Hai Thanh Nguyen <phaikawl@gmail.com>

#include "golanglint.h"
#include "golanglint_global.h"
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
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


GolangLint::GolangLint(LiteApi::IApplication *app,QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_autolint(true),
    m_synclint(false),
    m_timeout(600),
    m_confidence(50)
{
    m_process = new ProcessEx(this);
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(lintOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(started()),this,SLOT(started()));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(lintFinish(bool,int,QString)));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");

    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(m_envManager->currentEnv());
    }
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToSave(LiteApi::IEditor*)),this,SLOT(editorAboutToSave(LiteApi::IEditor*)));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));
    applyOption("option/golanglint");
}

void GolangLint::applyOption(QString id)
{
    if (id != "option/golanglint") {
        return;
    }

    m_confidence = m_liteApp->settings()->value(GOLANGLINT_CONFIDENCE,false).toInt();
    m_synclint = true;
    m_timeout = m_liteApp->settings()->value(GOLANGLINT_SYNCTIMEOUT,500).toInt();
}

void GolangLint::syncLintEditor(LiteApi::IEditor *editor, bool save, bool check, int timeout)
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
    args << QString("-min_confidence=%1").arg(m_confidence/100);
    args << editor->filePath();

    if (timeout < 0) {
        timeout = m_timeout;
    }

    if (save) {
        m_liteApp->editorManager()->saveEditor(editor,false);
    }

    QProcess process;
    process.setEnvironment(LiteApi::getCurrentEnvironment(m_liteApp).toStringList());
    process.start(m_golintCmd,args);

    if (!process.waitForStarted(timeout)) {
        // m_liteApp->appendLog("golint",QString("Timed out after %1ms when starting go code format").arg(timeout),false);
        return;
    }
    if (!process.waitForFinished(timeout)) {
        // m_liteApp->appendLog("golint",QString("Timed out after %1ms while running go code format").arg(timeout),false);
        return;
    }
    LiteApi::ILiteEditor *liteEditor = LiteApi::getLiteEditor(editor);
    // liteEditor->clearAllNavigateMark(LiteApi::EditorNavigateBad, GOLANGLINT_TAG);
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QByteArray error = process.readAllStandardOutput();
    QString errmsg = codec->toUnicode(error);
    if (!errmsg.isEmpty()) {
        //<standard input>:23:1: expected declaration, found 'INT' 1
        foreach(QString msg,errmsg.split("\n")) {
            QRegExp re(":(\\d+):\\d+:");
            int idx = re.indexIn(msg);
            if (idx >= 0) {
                bool ok = false;
                int line = re.cap(1).toInt(&ok);
                if (ok) {
                    liteEditor->insertNavigateMark(line-1,LiteApi::EditorNavigateWarning,msg.mid(idx+re.matchedLength()), GOLANGLINT_TAG);
                }
            }
        }

        liteEditor->setNavigateHead(LiteApi::EditorNavigateNormal,"go code lint output\n"+errmsg);
        m_liteApp->appendLog("go lint suggestions: ",errmsg,false);
        return;
    }
}

void GolangLint::lintEditor(LiteApi::IEditor *editor, bool)
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
    if (m_golintCmd.isEmpty()) {
        return;
    }
    if (m_process->isRunning()) {
        return;
    }
    QPlainTextEdit *edit = LiteApi::getPlainTextEdit(editor);
    if (!edit) {
        return;
    }
    QStringList args;
    args << QString("-min_confidence=%1").arg(m_confidence/100);
    args << editor->filePath();
    m_data.clear();;
    m_errData.clear();
    m_process->start(m_golintCmd,args);
}

void GolangLint::editorAboutToSave(LiteApi::IEditor* editor)
{
    if (!m_autolint) {
        return;
    }
    if (m_synclint) {
        syncLintEditor(editor,true);
    } else {
        lintEditor(editor,true);
    }
}

void GolangLint::currentEnvChanged(LiteApi::IEnv*)
{
    QProcessEnvironment env = m_envManager->currentEnvironment();

    m_golintCmd = FileUtil::lookupGoBin("golint",m_liteApp,false);

    if (m_golintCmd.isEmpty()) {
        m_liteApp->appendLog("GolangLint",QString("Could not find %1").arg(m_golintCmd),false);
    } else {
        m_liteApp->appendLog("GolangLint",QString("Found %1").arg(m_golintCmd),false);
    }
    m_process->setProcessEnvironment(env);
}

void GolangLint::golint()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    m_liteApp->editorManager()->saveEditor(editor,false);
    syncLintEditor(editor,false,true,-1);
}

void GolangLint::started()
{
    // QString text = m_process->userData(1).toString();
    // m_process->write(text.toUtf8());
    // m_process->closeWriteChannel();
}

void GolangLint::lintOutput(QByteArray data,bool)
{
    m_errData.append(data);
    m_data.append(data);
}

void GolangLint::lintFinish(bool error,int code,QString)
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

    if (ed->document()->isModified()) {
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    if (!error && code == 0) {
        liteEditor->setNavigateHead(LiteApi::EditorNavigateNormal,"go code lint success");

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
                        liteEditor->insertNavigateMark(line-1,LiteApi::EditorNavigateWarning,msg.mid(16),GOLANGLINT_TAG);
                    }
                }
            }
        }
        QString log = errmsg;
    }
    m_data.clear();
}
