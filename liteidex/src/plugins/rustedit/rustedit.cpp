/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: rustedit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "rustedit.h"
#include "fileutil/fileutil.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDir>
#include <QFile>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

RustEdit::RustEdit(LiteApi::IApplication *app, QObject *parent)
    : QObject(parent), m_liteApp(app)
{
    m_completer = 0;
    m_editor = 0;
    m_racerCmd = "racer";
    m_process = new QProcess(this);
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    LiteApi::IEnvManager *env = LiteApi::getEnvManager(m_liteApp);
    if (env) {
        connect(env,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
    }
}

void RustEdit::editorCreated(LiteApi::IEditor *editor)
{
    if (editor->mimeType() != "text/x-rust") {
        return;
    }
    LiteApi::ILiteEditor *ed = LiteApi::getLiteEditor(editor);
    if (ed) {
        ed->setTabOption(4,true);
        ed->setEnableAutoIndentAction(true);
    }
}

void RustEdit::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    if (editor->mimeType() != "text/x-rust") {
        return;
    }
    LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
    if (completer) {
        setCompleter(completer);
    }
    m_editor = LiteApi::getLiteEditor(editor);
}

void RustEdit::prefixChanged(const QTextCursor &/*cur*/, const QString &pre, bool /*force*/)
{
    if (pre.endsWith("::")) {
        m_completer->setSeparator("::");
    } else if (pre.endsWith(".")) {
        m_completer->setSeparator(".");
    }
    if (m_completer->completionContext() != LiteApi::CompleterCodeContext) {
        return;
    }

    if (m_racerCmd.isEmpty()) {
        return;
    }

    if (m_process->state() != QProcess::NotRunning) {
        return;
    }
    /*
    int offset = -1;
    if (pre.endsWith('.')) {
        m_preWord = pre;
        offset = 0;
    } else if (pre.length() == 1) {
        m_preWord.clear();
    } else {
        if (!force) {
            return;
        }
        m_preWord.clear();
        int index = pre.lastIndexOf(".");
        if (index != -1) {
            m_preWord = pre.left(index);
        }
    }*/
    if (!(pre.endsWith("::") || pre.endsWith("."))) {
        return;
    }

    m_prefix = pre;
    m_lastPrefix = m_prefix;

    QStringList args;
    args << "complete";
    args << QString("%1").arg(m_editor->line()+1);
    args << QString("%1").arg(m_editor->column());
    args << "racer_temp.rs";
    QString path = QDir::tempPath();
    QFile file(path+"//racer_temp.rs");
    if (file.open(QFile::WriteOnly|QFile::Truncate)) {
        file.write(m_editor->utf8Data());
        file.close();
    }
    m_process->setWorkingDirectory(path);
    m_process->start(m_racerCmd,args);
}

void RustEdit::wordCompleted(QString, QString, QString)
{   

}
//            PREFIX 21,21,
//            MATCH Arc,115,11,C:\Rust\rust-nightly\src\liballoc\arc.rs,Struct,pub struct Arc<T> {
//            MATCH Weak,131,11,C:\Rust\rust-nightly\src\liballoc\arc.rs,Struct,pub struct Weak<T> {
//            MATCH weak_count,205,7,C:\Rust\rust-nightly\src\liballoc\arc.rs,Function,pub fn weak_count<T>(this: &Arc<T>) -> uint { this.inner().weak.load(SeqCst) - 1 }
//            MATCH strong_count,210,7,C:\Rust\rust-nightly\src\liballoc\arc.rs,Function,pub fn strong_count<T>(this: &Arc<T>) -> uint { this.inner().strong.load(SeqCst) }
//            PREFIX 16,16,
//            MATCH core,1,0,C:\Rust\rust-nightly\src\libcore\lib.rs,Module,C:\Rust\rust-nightly\src\libcore\lib.rs
//            MATCH libc,1,0,C:\Rust\rust-nightly\src\liblibc\lib.rs,Module,C:\Rust\rust-nightly\src\liblibc\lib.rs
//            MATCH std,1,0,C:\Rust\rust-nightly\src\libstd\lib.rs,Module,C:\Rust\rust-nightly\src\libstd\lib.rs
//            MATCH log,1,0,C:\Rust\rust-nightly\src\liblog\lib.rs,Module,C:\Rust\rust-nightly\src\liblog\lib.rs
//            MATCH boxed,1,0,C:\Rust\rust-nightly\src\liballoc\boxed.rs,Module,C:\Rust\rust-nightly\src\liballoc\boxed.rs
//            MATCH heap,1,0,C:\Rust\rust-nightly\src\liballoc\heap.rs,Module,C:\Rust\rust-nightly\src\liballoc\heap.rs
//            MATCH boxed,1,0,C:\Rust\rust-nightly\src\liballoc\boxed.rs,Module,C:\Rust\rust-nightly\src\liballoc\boxed.rs
//            MATCH arc,1,0,C:\Rust\rust-nightly\src\liballoc\arc.rs,Module,C:\Rust\rust-nightly\src\liballoc\arc.rs
//            MATCH rc,1,0,C:\Rust\rust-nightly\src\liballoc\rc.rs,Module,C:\Rust\rust-nightly\src\liballoc\rc.rs
//            MATCH oom,100,7,C:\Rust\rust-nightly\src\liballoc\lib.rs,Function,pub fn oom() -> ! {
//            MATCH fixme_14344_be_sure_to_link_to_collections,120,7,C:\Rust\rust-nightly\src\liballoc\lib.rs,Function,pub fn fixme_14344_be_sure_to_link_to_collections() {}
//            PREFIX 6,6,
//            MATCH value,10,4,main.rs,StructField,value
//            PREFIX 26,29,new
//            MATCH new,161,11,C:\Rust\rust-nightly\src\liballoc\arc.rs,Function,pub fn new(data: T) -> Arc<T> {
void RustEdit::finished(int code, QProcess::ExitStatus)
{
    if (code != 0) {
        m_liteApp->appendLog("racer",QString::fromUtf8(m_process->readAll()),false);
        return;
    }
    QByteArray data = m_process->readAllStandardOutput();
    int count = 0;
    foreach (QString line, QString::fromUtf8(data).split("\n")) {
        if (line.startsWith("MATCH")) {
            QStringList array = line.mid(5).split(",",QString::SkipEmptyParts);
            if (array.size() > 0) {
                count++;
                m_completer->appendItem(m_prefix+array[0].trimmed(),QIcon(),true);
            }
        }
    }
    if (count > 0) {
        m_completer->updateCompleterModel();
        m_completer->showPopup();
    }
}

void RustEdit::setCompleter(LiteApi::ICompleter *completer)
{
    if (m_completer) {
        disconnect(m_completer,0,this,0);
    }
    m_completer = completer;
    if (m_completer) {
        if (!m_racerCmd.isEmpty()) {
            m_completer->setSearchSeparator(false);
            m_completer->setExternalMode(false);
            m_completer->setSeparator(".");
            connect(m_completer,SIGNAL(prefixChanged(QTextCursor,QString,bool)),this,SLOT(prefixChanged(QTextCursor,QString,bool)));
            connect(m_completer,SIGNAL(wordCompleted(QString,QString,QString)),this,SLOT(wordCompleted(QString,QString,QString)));
        } else {
            m_completer->setSearchSeparator(true);
            m_completer->setExternalMode(false);
        }
    }
}

void RustEdit::currentEnvChanged(LiteApi::IEnv*)
{
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    m_racerCmd = FileUtil::lookPath("racer",env,false);
    if (m_racerCmd.isEmpty()) {
#ifdef Q_OS_WIN
        m_racerCmd = "c:\\rust\\bin\\racer.exe";
#endif
    }
    m_process->setProcessEnvironment(env);
}
