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
// Module: golangcode.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangcode.h"
#include "golangcode_global.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include <QProcess>
#include <QTextDocument>
#include <QAbstractItemView>
#include <QApplication>
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

int GolangCode::g_gocodeInstCount = 0;

GolangCode::GolangCode(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_completer(0),
    m_closeOnExit(true),
    m_autoUpdatePkg(false)
{
    g_gocodeInstCount++;
    m_gocodeProcess = new QProcess(this);
    m_updatePkgProcess = new QProcess(this);
    m_breset = false;
    connect(m_gocodeProcess,SIGNAL(started()),this,SLOT(started()));
    connect(m_gocodeProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(m_envManager->currentEnv());
    }
    m_golangAst = LiteApi::findExtensionObject<LiteApi::IGolangAst*>(m_liteApp,"LiteApi.IGolangAst");
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_liteApp,SIGNAL(broadcast(QString,QString,QString)),this,SLOT(broadcast(QString,QString,QString)));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));
    applyOption("option/golangcode");
}

void GolangCode::applyOption(QString id)
{
    if (id != "option/golangcode") return;
    m_closeOnExit = m_liteApp->settings()->value(GOLANGCODE_EXITCLOSE,true).toBool();
    m_autoUpdatePkg = m_liteApp->settings()->value(GOLANGCODE_AUTOUPPKG,false).toBool();
}

void GolangCode::broadcast(QString module,QString id,QString)
{
    if (module == "golangpackage" && id == "reloadgopath") {
        resetGocode();
    }
}

GolangCode::~GolangCode()
{
    g_gocodeInstCount--;
    if (g_gocodeInstCount == 0 && m_closeOnExit && !m_gocodeCmd.isEmpty()) {
        ProcessEx::startDetachedEx(m_gocodeCmd,QStringList() << "close");
    }
    delete m_gocodeProcess;
}

void GolangCode::resetGocode()
{
    if (!m_gocodeCmd.isEmpty()) {
        m_breset = true;
        m_gocodeProcess->setWorkingDirectory(m_liteApp->applicationPath());
        m_gocodeProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
        m_gocodeProcess->start(m_gocodeCmd,QStringList() << "close");
    }
}

void GolangCode::currentEnvChanged(LiteApi::IEnv*)
{    
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    m_gocodeCmd = FileUtil::lookupGoBin("gocode",m_liteApp,true);
    m_gobinCmd = FileUtil::lookupGoBin("go",m_liteApp,false);
    m_updatePkgProcess->setProcessEnvironment(env);
    m_gocodeProcess->setProcessEnvironment(env);

    if (m_gocodeCmd.isEmpty()) {
         m_liteApp->appendLog("GolangCode","Could not find gocode (hint: is gocode installed?)",true);
    } else {
         m_liteApp->appendLog("GolangCode",QString("Found gocode at %1").arg(m_gocodeCmd));
    }
    resetGocode();
}

void GolangCode::currentEditorChanged(LiteApi::IEditor *editor)
{
    LiteApi::ITextEditor *ed = LiteApi::getTextEditor(editor);
    if (!ed) {
        return;
    }
    QString filePath = ed->filePath();
    if (filePath.isEmpty()) {
        return;
    }
    m_fileInfo.setFile(filePath);
    m_gocodeProcess->setWorkingDirectory(m_fileInfo.absolutePath());
    m_updatePkgProcess->setWorkingDirectory(m_fileInfo.absolutePath());
}

void GolangCode::setCompleter(LiteApi::ICompleter *completer)
{
    if (m_completer) {
        disconnect(m_completer,0,this,0);
    }
    m_completer = completer;
    if (m_completer) {
        if (!m_gocodeCmd.isEmpty()) {
            m_completer->setSearchSeparator(false);
            m_completer->setExternalMode(true);
            connect(m_completer,SIGNAL(prefixChanged(QTextCursor,QString,bool)),this,SLOT(prefixChanged(QTextCursor,QString,bool)));
            connect(m_completer,SIGNAL(wordCompleted(QString,QString)),this,SLOT(wordCompleted(QString,QString)));
        } else {
            m_completer->setSearchSeparator(true);
            m_completer->setExternalMode(false);
        }
    }
}

void GolangCode::prefixChanged(QTextCursor cur,QString pre,bool force)
{
    if (m_gocodeCmd.isEmpty()) {
        return;
    }

//    if (m_completer->completer()->completionPrefix().startsWith(pre)) {
//       // qDebug() << pre << m_completer->completer()->completionPrefix();
//       // return;
//    }
    if (m_gocodeProcess->state() != QProcess::NotRunning) {
        return;
    }

    if (pre.endsWith('.')) {
        m_preWord = pre;
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
    }

    m_prefix = pre;
    m_lastPrefix = m_prefix;

    if (!m_preWord.isEmpty()) {
        m_completer->clearItemChilds(m_preWord);
    }

    QString src = cur.document()->toPlainText();
    src = src.replace("\r\n","\n");
    m_writeData = src.left(cur.position()).toUtf8();
    QStringList args;
    args << "-in" << "" << "-f" << "csv" << "autocomplete" << m_fileInfo.fileName() << QString::number(m_writeData.length());
    m_writeData = src.toUtf8();
    m_breset = false;
    m_gocodeProcess->setWorkingDirectory(m_fileInfo.path());
    m_gocodeProcess->start(m_gocodeCmd,args);
}

void GolangCode::wordCompleted(QString,QString)
{
    m_prefix.clear();
}

void GolangCode::started()
{
    if (m_writeData.isEmpty()) {
        m_gocodeProcess->closeWriteChannel();
        return;
    }
    m_gocodeProcess->write(m_writeData);
    m_gocodeProcess->closeWriteChannel();
    m_writeData.clear();
}

void GolangCode::finished(int code,QProcess::ExitStatus)
{
    if (code != 0) {
        return;
    }

    if (m_breset) {
        m_breset = false;
        m_gocodeProcess->setWorkingDirectory(m_liteApp->applicationPath());
        m_gocodeProcess->start(m_gocodeCmd);
        return;
    }

    if (m_prefix.isEmpty()) {
        return;
    }

    if (m_prefix != m_lastPrefix) {
        m_prefix.clear();
        return;
    }

    QByteArray read = m_gocodeProcess->readAllStandardOutput();
    QList<QByteArray> all = read.split('\n');
    //func,,Fprint,,func(w io.Writer, a ...interface{}) (n int, error os.Error)
    //type,,Formatter,,interface
    //const,,ModeExclusive,,
    //var,,Args,,[]string
    int n = 0;
    QIcon icon;
    QStandardItem *root= m_completer->findRoot(m_preWord);
    foreach (QByteArray bs, all) {
        QStringList word = QString::fromUtf8(bs,bs.size()).split(",,");
        if (word.count() != 3) {
            continue;
        }
        if (word.at(0) == "PANIC") {
            continue;
        }
        LiteApi::ASTTAG_ENUM tag = LiteApi::TagNone;
        QString kind = word.at(0);
        QString info = word.at(2);
        if (kind == "package") {
            tag = LiteApi::TagPackage;
        } else if (kind == "func") {
            tag = LiteApi::TagFunc;
        } else if (kind == "var") {
            tag = LiteApi::TagValue;
        } else if (kind == "const") {
            tag = LiteApi::TagConst;
        } else if (kind == "type") {
            if (info == "interface") {
                tag = LiteApi::TagInterface;
            } else if (info == "struct") {
                tag = LiteApi::TagStruct;
            } else {
                tag = LiteApi::TagType;
            }
        }

        if (m_golangAst) {
            icon = m_golangAst->iconFromTagEnum(tag,true);
        }
        //m_completer->appendItemEx(m_preWord+word.at(1),kind,info,icon,true);
        m_completer->appendChildItem(root,word.at(1),kind,info,icon,true);
        n++;
    }
    m_prefix.clear();
    if (n >= 1) {
        m_completer->show();
    } else if (m_autoUpdatePkg && !m_gobinCmd.isEmpty()){
        if (m_updatePkgProcess->state() != QProcess::NotRunning) {
            return;
        }
        m_updatePkgProcess->start(m_gobinCmd,QStringList() << "get");
    }
}
