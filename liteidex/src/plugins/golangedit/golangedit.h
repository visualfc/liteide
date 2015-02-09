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
// Module: golangedit.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGEDIT_H
#define GOLANGEDIT_H

#include <liteapi/liteapi.h>
#include <liteeditorapi/liteeditorapi.h>
#include <liteenvapi/liteenvapi.h>
#include "processex/processex.h"
#include "golangfilesearch.h"

class GolangEdit : public QObject
{
    Q_OBJECT
public:
    explicit GolangEdit(LiteApi::IApplication *app, QObject *parent = 0);
    QTextCursor textCursorForPos(const QPoint &globalPos);
public slots:
    void applyOption(const QString &option);
    void editorCreated(LiteApi::IEditor*);
    void currentEditorChanged(LiteApi::IEditor*);
    void updateLink(const QTextCursor &cursor, const QPoint &pos, bool nav);
    void aboutToShowContextMenu();
    void editorViewGodoc();
    void editorFindInfo();
    void editorJumpToDecl();
    void editorFindUsages();
    void editorRenameSymbol();
    void editorComment();
    void findDefStarted();
    void findDefOutput(QByteArray,bool);
    void findDefFinish(bool,int,QString);
    void findInfoStarted();
    void findInfoOutput(QByteArray,bool);
    void findInfoFinish(bool,int,QString);
    void findLinkStarted();
    void findLinkFinish(int, QProcess::ExitStatus);
    void searchTextChanged(const QString &word);
protected:
    LiteApi::IApplication *m_liteApp;
    LiteApi::ILiteEditor  *m_editor;
    QPlainTextEdit        *m_plainTextEdit;
    bool m_enableMouseUnderInfo;
    bool m_enableMouseNavigation;
    QTextCursor m_linkCursor;
    LiteApi::Link m_lastLink;
    QAction *m_findInfoAct;
    QAction *m_jumpDeclAct;
    QAction *m_findUseAct;
    QAction *m_renameSymbolAct;
    QAction *m_viewGodocAct;
    ProcessEx  *m_findDefProcess;
    ProcessEx  *m_findInfoProcess;
    Process  *m_findLinkProcess;
    QByteArray  m_findDefData;
    QByteArray  m_srcData;
    QTextCursor m_lastCursor;
    QTextCursor m_findLastCursor;
    QByteArray  m_findInfoData;
    GolangFileSearch *m_fileSearch;
};

#endif // GOLANGEDIT_H
