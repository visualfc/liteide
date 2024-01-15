/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2024 LiteIDE. All rights reserved.
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
// Module: goplsfilesearch.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "goplsfilesearch.h"
#include "liteeditorapi/liteeditorapi.h"
#include "liteenvapi/liteenvapi.h"
#include "fileutil/fileutil.h"
#include <QTextBlock>
#include <qalgorithms.h>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif
//lite_memory_check_end

#include <QDebug>


static QByteArray trimmedRight(const QByteArray &d)
{
    if (d.size() == 0) {
        return d;
    }
    const char *s = d.data();
    int end = d.size() - 1;
    while (end && isspace(uchar(s[end])))           // skip white space from end
        end--;
    return d.left(end+1);
}

GoplsFileSearch::GoplsFileSearch(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IFileSearch(parent), m_liteApp(app)
{
    m_process = new ProcessEx(this);
    m_replaceMode = false;
    m_readOnly = true;
    m_bParserHead = true;
    //    connect(m_process,SIGNAL(started()),this,SLOT(findUsagesStarted()));
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findUsagesOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findUsagesFinish(bool,int,QString)));
}

GoplsFileSearch::~GoplsFileSearch()
{
}

QString GoplsFileSearch::mimeType() const
{
    return "find/goplsfileserch";
}

QWidget *GoplsFileSearch::widget() const
{
    return 0;
}

void GoplsFileSearch::start()
{

}

void GoplsFileSearch::cancel()
{
}

void GoplsFileSearch::activate()
{

}

QString GoplsFileSearch::searchText() const
{
    return m_searchText;
}

bool GoplsFileSearch::replaceMode() const
{
    return m_replaceMode;
}

void GoplsFileSearch::setSearchInfo(const QString &/*text*/, const QString &/*filter*/, const QString &/*path*/)
{

}

void GoplsFileSearch::findUsages(LiteApi::ITextEditor *editor, QTextCursor cursor, const QString &command, const QStringList &opts, bool replace)
{
    if (!m_process->isStop()) {
        m_process->stopAndWait(100,2000);
    }

    bool moveLeft = false;
    int selectStart = 0;
    m_searchText = LiteApi::wordUnderCursor(cursor,&moveLeft,&selectStart);

    if (m_searchText.isEmpty() || m_searchText.contains(" ")) {
        return;
    }

    m_liteApp->editorManager()->saveAllEditors(false);

    //int offset = moveLeft ? editor->utf8Position(true)-1: editor->utf8Position(true);
    int offset = editor->utf8Position(true,selectStart);

    LiteApi::IFileSearchManager *manager = LiteApi::getFileSearchManager(m_liteApp);
    if (!manager) {
        return;
    }
    this->m_replaceMode = replace;
    manager->setCurrentSearch(this);
    m_results.clear();

    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    FileUtil::lookupGoBin("gopls",m_liteApp,env,false);

    QString gopls = FileUtil::lookupGoBin("gopls",m_liteApp,env,true);
    if (gopls.isEmpty()) {
        gopls = FileUtil::lookPath("dlv",env,false);
    }

    if (gopls.isEmpty()) {
        m_liteApp->appendLog("GolangEdit","gopls was not found on system PATH (hint: is gopls installed?)",true);
        return;
    }
    QFileInfo info(editor->filePath());
    m_process->setEnvironment(LiteApi::getCustomGoEnvironment(m_liteApp,editor).toStringList());
    m_process->setWorkingDirectory(info.path());

    QStringList args;
    args << command;
    if (!opts.isEmpty()) {
        args << opts;
    }
    args << QString("\"%1:#%2\"").arg(info.fileName()).arg(offset);

    emit findStarted();
    m_process->startEx(gopls,args);
}

void GoplsFileSearch::findUsagesStarted()
{
}

void GoplsFileSearch::findUsagesOutput(QByteArray data, bool bStdErr)
{
    if (bStdErr) {
        QString info = QString::fromUtf8(data).trimmed();
        emit findError(info);
        m_liteApp->appendLog("find usage error",info,true);
        return;
    }
    QRegExp reg(":(\\d+):(\\d+)-(\\d*)");
    foreach (QByteArray line, data.split('\n')) {
        QString info = QString::fromUtf8(line).trimmed();
        int pos = reg.lastIndexIn(info);
        if (pos >= 0) {
            gopls_resinfo res;
            res.fileName = info.left(pos);
            res.line = reg.cap(1).toInt();
            res.column = reg.cap(2).toInt();
            res.columnEnd = reg.cap(3).toInt();
            m_results.append(res);
        }
    }
}

struct resultComp
{
    bool operator()(const gopls_resinfo &a,
                    const gopls_resinfo &b) {
        if (a.fileName == b.fileName) {
            if (a.line == b.line) {
                return a.column < b.column;
            }
            return a.line < b.line;
        }
        return a.fileName < b.fileName;
    }
};

void GoplsFileSearch::findUsagesFinish(bool b, int, QString)
{
    if (!m_results.isEmpty()) {
        qSort(m_results.begin(),m_results.end(),resultComp());
    }
    QFile file;
    int lastLine = 0;
    QString lastLineText;
    foreach(gopls_resinfo info, m_results) {
        QString fileName = info.fileName;
        int fileLine = info.line;
        int fileCol = info.column;
        int fileCol2 = info.columnEnd;
        if (file.fileName() != fileName) {
            file.close();
            file.setFileName(fileName);
        }
        if (!file.isOpen()) {
            file.open(QFile::ReadOnly);
            lastLine = 0;
        }
        if (file.isOpen()) {
            if (fileLine != lastLine) {
                while(!file.atEnd()) {
                    QByteArray line = file.readLine();
                    lastLine++;
                    if (fileLine == lastLine) {
                        lastLineText = QString::fromUtf8(trimmedRight(line));
                        if (fileCol > 0) {
                            fileCol = QString::fromUtf8(line.left(fileCol)).length();
                        }
                        break;
                    }
                }
            } else {
                QByteArray line = lastLineText.toUtf8();
                if (fileCol > 0) {
                    fileCol = QString::fromUtf8(line.left(fileCol)).length();
                }
            }
            int length = m_searchText.length();
            if (fileCol2 > fileCol) {
                length = fileCol2-fileCol;
            }
            emit findResult(LiteApi::FileSearchResult(fileName,lastLineText,fileLine,fileCol-1,length));
        }
    }
    m_results.clear();

    emit findFinished(b);
}

