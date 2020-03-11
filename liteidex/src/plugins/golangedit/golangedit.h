/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
#include "textoutput/textoutput.h"
#include "golangfilesearch.h"
/*
    a tool for answering questions about Go source code.
    callees	  	show possible targets of selected function call
    callers	  	show possible callers of selected function
    callstack 	show path from callgraph root to selected function
    definition	show declaration of selected identifier
    describe  	describe selected syntax: definition, methods, etc
    freevars  	show free variables of selection
    implements	show 'implements' relation for selected type or method
    peers     	show send/receive corresponding to selected channel op
    referrers 	show all refs to entity denoted by selected identifier
    what		show basic information about the selected syntax node
    pointsto
    whicherrs
*/

struct SourceQuery
{
    QString cmdName;
    QString workPath;
    QString filePath;
    QString fileName;
    QString action;
    QString output;
    QString mode;
    int     offset;
    int     offset2;
    bool    success;
};

class GoAddTagsDialog;
class GoRemoveTagsDialog;

class GolangEdit : public QObject
{
    Q_OBJECT
public:
    explicit GolangEdit(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~GolangEdit();
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
    void editorFindUsagesGlobal();
    void editorFindUsagesSkipGoroot();
    void editorRenameSymbol();
    void editorRenameSymbolGlobal();
    void editorRenameSymbolSkipGoroot();
    void editorComment();
    void findDefStarted();
    void findDefFinish(int code,QProcess::ExitStatus status);
    void findInfoStarted();
    void findInfoFinish(int, QProcess::ExitStatus);
    void findInfoGopherOutput(const QByteArray &data,bool bError);
    void findLinkStarted();
    void findLinkFinish(int, QProcess::ExitStatus);
    void searchTextChanged(const QString &word);
    void sourceQueryFinished(int code, QProcess::ExitStatus status);
    void sourcequeryError(QProcess::ProcessError code);
    //void updateOracleInfo(const QString &action, const QString &text);
    void dbclickSourceQueryOutput(const QTextCursor &cursor);
    void runSourceQueryAction(const QString &action, const QString &scope = ".");
    void runSourceQueryByInfo(const QString &action, const QString &scope = ".");
    void sourceWhat();
    void sourceCallees();
    void sourceCallers();
    void sourceCallstack();
    void sourceDefinition();
    void sourceDescribe();
    void sourceFreevars();
    void sourceImplements();
    void sourceImplementsGopath();
    void sourcePeers();
    void sourceReferrers();
    void sourcePointsto();
    void sourceWhicherrs();
    void sourceQueryStateChanged(QProcess::ProcessState state);
    void stopSourceQueryProcess();
    void goAddTags();
    void goRemoveTags();
    void execGoModifyTags(const QString &args);
    QString getGoModifyTagsInfo() const;
    int byteOffsetToColumn(const QString &fileName, int line, int col);
protected:
    LiteApi::IApplication *m_liteApp;
    LiteApi::ILiteEditor  *m_editor;
    QPlainTextEdit        *m_plainTextEdit;
    TextOutput            *m_sourceQueryOutput;
    QAction               *m_sourceQueryOutputAct;
    QAction               *m_stopSourceQueryAct;
    bool m_enableMouseUnderInfo;
    bool m_enableMouseNavigation;
    bool m_gorootSourceReadOnly;
    bool m_useGocodeInfo;
    QTextCursor m_linkCursor;
    LiteApi::Link m_lastLink;
    QAction *m_findInfoAct;
    QAction *m_jumpDeclAct;
    QAction *m_jumpDeclToolBarAct;
    QAction *m_findUseAct;
    QAction *m_findAllUseWithGorootAct;
    QAction *m_findAllUseSkipGorootAct;
    QAction *m_renameSymbolAct;
    QAction *m_renameAllSymbolWithGorootAct;
    QAction *m_renameAllSymbolSkipGorootAct;
    QAction *m_viewGodocAct;
    QAction *m_sourceWhatAct;
    QAction *m_sourceCalleesAct;
    QAction *m_sourceCallersAct;
    QAction *m_sourceCallstackAct;
    QAction *m_sourceDefinitionAct;
    QAction *m_sourceDescribeAct;
    QAction *m_sourceFreevarsAct;
    QAction *m_sourceImplementsAct;
    QAction *m_sourceImplementsGopathAct;
    QAction *m_sourcePeersAct;
    QAction *m_sourceReferrersAct;
    QAction *m_sourcePointstoAct;
    QAction *m_sourceWhicherrs;
    QAction *m_goAddTagsAct;
    QAction *m_goRemoveTagAct;
    Process  *m_findDefProcess;
    Process  *m_findInfoProcess;
    Process  *m_findLinkProcess;
    Process  *m_sourceQueryProcess;
    //GopherLib m_findInfoGopher;
    SourceQuery m_sourceQueryInfo;
    QByteArray  m_findDefData;
    QByteArray  m_srcData;
    QTextCursor m_lastCursor;
    QTextCursor m_findLastCursor;
    QByteArray  m_findInfoData;
    GolangFileSearch *m_fileSearch;
    GoAddTagsDialog  *m_addTagsDlg;
    GoRemoveTagsDialog *m_removeTagsDlg;
};

#endif // GOLANGEDIT_H
