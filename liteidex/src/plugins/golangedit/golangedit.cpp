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
// Module: golangedit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangedit.h"
#include "golangtextlexer.h"
#include "golangedit_global.h"
#include "qtc_editutil/uncommentselection.h"
#include "litebuildapi/litebuildapi.h"
#include "golangdocapi/golangdocapi.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "goaddtagsdialog.h"
#include "goremovetagsdialog.h"

#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTextStream>
#include <QApplication>
#include <QToolTip>
#include <QRegExp>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

//type gotools.s struct{}
//type command.Command struct{Run func(cmd *Command, args []string);Short string;Long string;Flag flag.FlagSet;CustomFlags bool}
QString formatInfo(const QString &info)
{
    if (!info.startsWith("type")) {
        return info;
    }
    QRegExp re("([\\w\\s\\.]+)\\{(.+)\\}");
    if (re.indexIn(info) == 0) {
        if (re.matchedLength() == info.length()) {
            QString str = re.cap(1)+" {\n";
            foreach (QString item, re.cap(2).split(";",QString::SkipEmptyParts)) {
                str += "\t"+item.trimmed()+"\n";
            }
            str += "}";
            return str;
        }
    }
    return info;
}

GolangEdit::GolangEdit(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent), m_liteApp(app), m_gorootSourceReadOnly(false)
{
    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GolangEdit");

    m_viewGodocAct = new QAction(tr("View import package use godoc"),this);
    actionContext->regAction(m_viewGodocAct,"ViewGodoc","");

    m_findInfoAct = new QAction(tr("View Expression Information"),this);
    actionContext->regAction(m_findInfoAct,"ViewInfo","CTRL+SHIFT+I;F1");

    m_jumpDeclAct = new QAction(tr("Jump to Declaration"),this);
    actionContext->regAction(m_jumpDeclAct,"JumpToDeclaration","CTRL+SHIFT+J;F2");

    m_findUseAct = new QAction(tr("Find Usages"),this);
    actionContext->regAction(m_findUseAct,"FindUsages","CTRL+SHIFT+U");

    m_renameSymbolAct = new QAction(tr("Rename Symbol Under Cursor"),this);
    actionContext->regAction(m_renameSymbolAct,"RenameSymbol","CTRL+SHIFT+R");

    m_findUseGlobalAct = new QAction(QString("%1 (GOPATH)").arg(tr("Find Usages")),this);
    actionContext->regAction(m_findUseGlobalAct,"FindUsagesGOPATH","CTRL+ALT+U");

    m_renameSymbolGlobalAct = new QAction(QString("%1 (GOPATH)").arg(tr("Rename Symbol Under Cursor")),this);
    actionContext->regAction(m_renameSymbolGlobalAct,"RenameSymbolGOPATH","");


    m_fileSearch = new GolangFileSearch(app,m_liteApp);
    LiteApi::IFileSearchManager *manager = LiteApi::getFileSearchManager(app);
    if (manager) {
        manager->addFileSearch(m_fileSearch);
    }

    m_findDefProcess = new Process(this);
    m_findInfoProcess = new Process(this);
    m_findLinkProcess = new Process(this);
    m_sourceQueryProcess = new Process(this);
    m_enableMouseUnderInfo = true;
    m_enableMouseNavigation = true;

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));

    connect(m_viewGodocAct,SIGNAL(triggered()),this,SLOT(editorViewGodoc()));
    connect(m_findInfoAct,SIGNAL(triggered()),this,SLOT(editorFindInfo()));
    connect(m_jumpDeclAct,SIGNAL(triggered()),this,SLOT(editorJumpToDecl()));
    connect(m_findUseAct,SIGNAL(triggered()),this,SLOT(editorFindUsages()));
    connect(m_renameSymbolAct,SIGNAL(triggered()),this,SLOT(editorRenameSymbol()));
    connect(m_findUseGlobalAct,SIGNAL(triggered()),this,SLOT(editorFindUsagesGlobal()));
    connect(m_renameSymbolGlobalAct,SIGNAL(triggered()),this,SLOT(editorRenameSymbolGlobal()));
    connect(m_findDefProcess,SIGNAL(started()),this,SLOT(findDefStarted()));
    //connect(m_findDefProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findDefOutput(QByteArray,bool)));
    connect(m_findDefProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(findDefFinish(int,QProcess::ExitStatus)));
    connect(m_findInfoProcess,SIGNAL(started()),this,SLOT(findInfoStarted()));
    //connect(m_findInfoProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findInfoOutput(QByteArray,bool)));
    connect(m_findInfoProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(findInfoFinish(int,QProcess::ExitStatus)));
    connect(m_findLinkProcess,SIGNAL(started()),this,SLOT(findLinkStarted()));
    connect(m_findLinkProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(findLinkFinish(int,QProcess::ExitStatus)));

    connect(&m_findInfoGopher,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findInfoGopherOutput(QByteArray,bool)));

    if (m_fileSearch) {
        connect(m_fileSearch,SIGNAL(searchTextChanged(QString)),this,SLOT(searchTextChanged(QString)));
    }

    connect(m_sourceQueryProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(sourceQueryFinished(int,QProcess::ExitStatus)));
    connect(m_sourceQueryProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(sourcequeryError(QProcess::ProcessError)));

    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));

    m_sourceQueryOutput = new TextOutput(m_liteApp,true);
    m_sourceQueryOutput->setLineWrap(false);

    m_stopSourceQueryAct = new QAction(tr("Stop"),this);
    m_stopSourceQueryAct->setIcon(QIcon("icon:litebuild/images/stopaction.png"));

    connect(m_sourceQueryOutput,SIGNAL(dbclickEvent(QTextCursor)),this,SLOT(dbclickSourceQueryOutput(QTextCursor)));

    m_sourceQueryOutputAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,m_sourceQueryOutput,"GoSourceQuery",tr("Go Source Query"),true,
                                                                           QList<QAction*>() << m_stopSourceQueryAct);
    connect(m_sourceQueryProcess,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(sourceQueryStateChanged(QProcess::ProcessState)));
    connect(m_stopSourceQueryAct,SIGNAL(triggered()),this,SLOT(stopSourceQueryProcess()));

    m_sourceWhatAct = new QAction(tr("SourceQuery What"),this);
    actionContext->regAction(m_sourceWhatAct,"SourceQueryWhat","Ctrl+Shift+H");
    connect(m_sourceWhatAct,SIGNAL(triggered()),this,SLOT(sourceWhat()));

    m_sourceCalleesAct = new QAction(tr("Callees"),this);
    actionContext->regAction(m_sourceCalleesAct,"SourceQueryCallees","");
    connect(m_sourceCalleesAct,SIGNAL(triggered()),this,SLOT(sourceCallees()));

    m_sourceCallersAct = new QAction(tr("Callers"),this);
    actionContext->regAction(m_sourceCallersAct,"SourceQueryCallers","");
    connect(m_sourceCallersAct,SIGNAL(triggered()),this,SLOT(sourceCallers()));

    m_sourceCallstackAct = new QAction(tr("Callstack"),this);
    actionContext->regAction(m_sourceCallstackAct,"SourceQueryCallstack","");
    connect(m_sourceCallstackAct,SIGNAL(triggered()),this,SLOT(sourceCallstack()));

    m_sourceDefinitionAct = new QAction(tr("Definition"),this);
    actionContext->regAction(m_sourceDefinitionAct,"SourceQueryDefinition","");
    connect(m_sourceDefinitionAct,SIGNAL(triggered()),this,SLOT(sourceDefinition()));

    m_sourceDescribeAct = new QAction(tr("Describe"),this);
    actionContext->regAction(m_sourceDescribeAct,"SourceQueryDescribe","");
    connect(m_sourceDescribeAct,SIGNAL(triggered()),this,SLOT(sourceDescribe()));

    m_sourceFreevarsAct = new QAction(tr("Freevars"),this);
    actionContext->regAction(m_sourceFreevarsAct,"SourceQueryFreevars","");
    connect(m_sourceFreevarsAct,SIGNAL(triggered()),this,SLOT(sourceFreevars()));

    m_sourceImplementsAct = new QAction(tr("Implements"),this);
    actionContext->regAction(m_sourceImplementsAct,"SourceQueryImplements","");
    connect(m_sourceImplementsAct,SIGNAL(triggered()),this,SLOT(sourceImplements()));

    m_sourceImplementsGopathAct = new QAction(tr("Implements(GOPATH)"),this);
    actionContext->regAction(m_sourceImplementsGopathAct,"SourceQueryImplementsGopath","");
    connect(m_sourceImplementsGopathAct,SIGNAL(triggered()),this,SLOT(sourceImplementsGopath()));

    m_sourcePeersAct = new QAction(tr("Peers"),this);
    actionContext->regAction(m_sourcePeersAct,"SourceQueryPeers","");
    connect(m_sourcePeersAct,SIGNAL(triggered()),this,SLOT(sourcePeers()));

    m_sourceReferrersAct = new QAction(tr("Referrers"),this);
    actionContext->regAction(m_sourceReferrersAct,"SourceQueryReferrers","");
    connect(m_sourceReferrersAct,SIGNAL(triggered()),this,SLOT(sourceReferrers()));

    m_sourcePointstoAct = new QAction(tr("Pointsto"),this);
    actionContext->regAction(m_sourcePointstoAct,"SourceQueryPointsto","");
    connect(m_sourcePointstoAct,SIGNAL(triggered()),this,SLOT(sourcePointsto()));

    m_sourceWhicherrs = new QAction(tr("Whicherrs"),this);
    actionContext->regAction(m_sourceWhicherrs,"SourceQueryWhicherrs","");
    connect(m_sourceWhicherrs,SIGNAL(triggered()),this,SLOT(sourceWhicherrs()));

    m_goAddTagsAct = new QAction(tr("Add Tags To Struct Field"),this);
    actionContext->regAction(m_goAddTagsAct,"GoAddTags","");
    connect(m_goAddTagsAct,SIGNAL(triggered()),this,SLOT(goAddTags()));

    m_goRemoveTagAct = new QAction(tr("Remove Tags From Struct Field"),this);
    actionContext->regAction(m_goRemoveTagAct,"GoRemoveTags","");
    connect(m_goRemoveTagAct,SIGNAL(triggered()),this,SLOT(goRemoveTags()));

    m_addTagsDlg = 0;
    m_removeTagsDlg = 0;

    this->applyOption(OPTION_GOLANGEDIT);
}

GolangEdit::~GolangEdit()
{
    if (m_sourceQueryOutput) {
        delete m_sourceQueryOutput;
    }
}

//bool GolangEdit::eventFilter(QObject *obj, QEvent *event)
//{
//    if (obj == m_plainTextEdit && event->type() == QEvent::ShortcutOverride) {
//        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
////        int key = keyEvent->key();
////        Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
////        if (modifiers & Qt::ShiftModifier) {
////            key += Qt::SHIFT;
////        }
////        if (modifiers & Qt::ControlModifier) {
////            key += Qt::CTRL;
////        }
////        if (modifiers & Qt::AltModifier) {
////            key += Qt::ALT;
////        }
////        QKeySequence ks(key);
////        if (m_findInfoAct->shortcuts().contains(ks)) {
////            event->accept();
////        }
//        if ((mouseEvent->modifiers() & Qt::ControlModifier)) {
//            //findEditorCursorInfo(m_editor,this->textCursorForPos(QCursor::pos()));
//            return true;
//        }
//    }
//    return QObject::eventFilter(obj,event);
//}

QTextCursor GolangEdit::textCursorForPos(const QPoint &globalPos)
{
    QPoint pos = m_plainTextEdit->viewport()->mapFromGlobal(globalPos);
    QTextCursor cur = m_plainTextEdit->textCursor();
    QRect rc = m_plainTextEdit->cursorRect(cur);
    if (rc.contains(pos)) {
        return cur;
    }
    return m_plainTextEdit->cursorForPosition(pos);
}

void GolangEdit::applyOption(const QString &option)
{
    if (option != OPTION_GOLANGEDIT) {
        return;
    }
    m_enableMouseUnderInfo = m_liteApp->settings()->value(GOLANGEDIT_MOUSEINFO,true).toBool();
    m_enableMouseNavigation = m_liteApp->settings()->value(GOLANGEDIT_MOUSENAVIGATIOIN,true).toBool();
    bool gorootSourceReadOnly = m_liteApp->settings()->value(GOLANGEDIT_GOROOTSOURCEREADONLY,false).toBool();
    if (gorootSourceReadOnly != m_gorootSourceReadOnly) {
        m_gorootSourceReadOnly = gorootSourceReadOnly;
        QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
        QString goroot = env.value("GOROOT");
        if (!goroot.isEmpty()) {
            foreach (LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
                if (!editor) {
                    continue;
                }
                QString filePath = editor->filePath();
                if (filePath.isEmpty()) {
                    continue;
                }
                if (QDir::fromNativeSeparators(filePath).startsWith(QDir::fromNativeSeparators(goroot))) {
                    editor->setReadOnly(m_gorootSourceReadOnly);
                }
            }
        }
    }
}

void GolangEdit::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    if (m_gorootSourceReadOnly) {
        QString path = editor->filePath();
        if ( !path.isEmpty()) {
            QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
            QString goroot = env.value("GOROOT");
            if (!goroot.isEmpty()) {
                if (QDir::fromNativeSeparators(path).startsWith(QDir::fromNativeSeparators(goroot))) {
                    editor->setReadOnly(true);
                }
            }
        }
    }
    if (editor->mimeType() != "text/x-gosrc") {
        return;
    }

    //editor->widget()->addAction(m_commentAct);
    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_viewGodocAct);
        menu->addSeparator();
        menu->addAction(m_findInfoAct);
        menu->addAction(m_jumpDeclAct);
        menu->addAction(m_findUseAct);
        menu->addAction(m_findUseGlobalAct);
        menu->addSeparator();
        QMenu *sub = menu->addMenu(tr("Refactor"));
        sub->addAction(m_renameSymbolAct);
        sub->addAction(m_renameSymbolGlobalAct);

        menu->addSeparator();
        menu->addAction(m_sourceWhatAct);
        sub = menu->addMenu(tr("SourceQuery"));
        sub->addAction(m_sourceCalleesAct);
        sub->addAction(m_sourceCallersAct);
        sub->addAction(m_sourceCallstackAct);
        sub->addAction(m_sourceDefinitionAct);
        sub->addAction(m_sourceDescribeAct);
        sub->addAction(m_sourceFreevarsAct);
        sub->addAction(m_sourceImplementsAct);
        sub->addAction(m_sourceImplementsGopathAct);
        sub->addAction(m_sourcePeersAct);
        sub->addAction(m_sourcePointstoAct);
        sub->addAction(m_sourceReferrersAct);
        sub->addAction(m_sourceWhicherrs);

        menu->addSeparator();
        menu->addAction(m_goAddTagsAct);
        menu->addAction(m_goRemoveTagAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_viewGodocAct);
        menu->addSeparator();
        menu->addAction(m_findInfoAct);
        menu->addAction(m_jumpDeclAct);
        menu->addAction(m_findUseAct);
        menu->addAction(m_findUseGlobalAct);
        menu->addSeparator();
        QMenu *sub = menu->addMenu(tr("Refactor"));
        sub->addAction(m_renameSymbolAct);
        sub->addAction(m_renameSymbolGlobalAct);
        connect(menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowContextMenu()));

        menu->addSeparator();
        menu->addAction(m_sourceWhatAct);
        sub = menu->addMenu(tr("SourceQuery"));
        sub->addAction(m_sourceCalleesAct);
        sub->addAction(m_sourceCallersAct);
        sub->addAction(m_sourceCallstackAct);
        sub->addAction(m_sourceDefinitionAct);
        sub->addAction(m_sourceDescribeAct);
        sub->addAction(m_sourceFreevarsAct);
        sub->addAction(m_sourceImplementsAct);
        sub->addAction(m_sourceImplementsGopathAct);
        sub->addAction(m_sourcePeersAct);
        sub->addAction(m_sourcePointstoAct);
        sub->addAction(m_sourceReferrersAct);
        sub->addAction(m_sourceWhicherrs);

        menu->addSeparator();
        menu->addAction(m_goAddTagsAct);
        menu->addAction(m_goRemoveTagAct);
    }
    m_editor = LiteApi::getLiteEditor(editor);
    if (m_editor) {
        m_editor->setTextLexer(new GolangTextLexer());
        connect(m_editor,SIGNAL(updateLink(QTextCursor,QPoint,bool)),this,SLOT(updateLink(QTextCursor,QPoint,bool)));
        //new go src for unix line end
        if (m_editor->document()->isEmpty()) {
            m_editor->setLineEndUnix(true);
        }
    }
}

void GolangEdit::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (!editor || editor->mimeType() != "text/x-gosrc") {
        return;
    }
    m_editor = LiteApi::getLiteEditor(editor);
    m_plainTextEdit = LiteApi::getPlainTextEdit(editor);
}

void GolangEdit::updateLink(const QTextCursor &cursor, const QPoint &pos, bool nav)
{
    if (nav) {
        if (!m_enableMouseNavigation) {
            return;
        }
    } else {
        if (!m_enableMouseUnderInfo) {
            return;
        }
    }

    QString text = cursor.selectedText();
    //hack
    if (text == "(") {
        text.clear();
    }
    if (text.isEmpty()) {
        m_lastLink.clear();
        m_editor->clearLink();
        return;
    }

    if (m_lastLink.linkTextStart == cursor.selectionStart() &&
            m_lastLink.linkTextEnd == cursor.selectionEnd()) {
        if (m_lastLink.hasValidTarget()) {
            m_lastLink.cursorPos = pos;
            m_lastLink.showTip = true;
            m_lastLink.showNav = nav;
            m_editor->showLink(m_lastLink);
            return;
        }
        return;
    }
    if (!m_findLinkProcess->isStop()) {
        m_findLinkProcess->stopAndWait(100,200);
    }

    m_lastLink.clear();
    m_lastLink.showTip = true;
    m_lastLink.showNav = nav;
    m_lastLink.linkTextStart = cursor.selectionStart();
    m_lastLink.linkTextEnd = cursor.selectionEnd();
    m_lastLink.cursorPos = pos;

    QString cmd = LiteApi::getGotools(m_liteApp);

    m_srcData = m_editor->utf8Data();
    int offset = m_editor->utf8Position(false,cursor.selectionStart());

    QFileInfo info(m_editor->filePath());
    m_findLinkProcess->setEnvironment(LiteApi::getCustomGoEnvironment(m_liteApp,m_editor).toStringList());
    m_findLinkProcess->setWorkingDirectory(info.path());

    QStringList args;
    args << "types";
    QString tags = LiteApi::getGoBuildFlagsArgument(m_liteApp,m_editor,"-tags");
    if (!tags.isEmpty()) {
        args << "-tags";
        args << tags;
    }
    args << "-b";
    args << "-pos";
    args << QString("\"%1:%2\"").arg(info.fileName()).arg(offset);
    args << "-stdin";
    args << "-info";
    args << "-def";
    args << "-doc";
    args << ".";

    m_findLinkProcess->startEx(cmd,args.join(" "));
}

void GolangEdit::aboutToShowContextMenu()
{
    LiteApi::ITextLexer *textLexer = LiteApi::getTextLexer(m_editor);
    if (!textLexer) {
        return;
    }
    QTextCursor cursor = m_editor->textCursor();
    bool b = textLexer->isInImport(cursor);
    m_viewGodocAct->setVisible(b);
}

QString parser_import(const QString &text)
{
    QString sep = "\"";
    int start = text.indexOf(sep);
    if (start < 0) {
        sep = "`";
        start = text.indexOf(sep);
    }
    if (start >= 0) {
        int end = text.indexOf(sep,start+1);
        if (end > 0) {
            return text.mid(start+1,end-start-1);
        }
    }
    return QString();
}

void GolangEdit::editorViewGodoc()
{
    LiteApi::ITextLexer *textLexer = LiteApi::getTextLexer(m_editor);
    if (!textLexer) {
        return;
    }
    QTextCursor cursor = m_editor->textCursor();
    bool b = textLexer->isInImport(cursor);
    if (!b) {
        return;
    }
    QString pkg = parser_import(cursor.block().text());
    if (pkg.isEmpty()) {
        return;
    }
    LiteApi::IGolangDoc *doc = LiteApi::getGolangDoc(m_liteApp);
    if (!doc) {
        return;
    }
    QUrl url;
    url.setScheme("pdoc");
    url.setPath(pkg);
    doc->openUrl(url);
    doc->activeBrowser();
}

void GolangEdit::editorJumpToDecl()
{
    bool moveLeft = false;
    int selectStart = 0;
    QString text = LiteApi::wordUnderCursor(m_plainTextEdit->textCursor(),&moveLeft,&selectStart);
    if (text.isEmpty() || text.contains(" ")) {
        return;
    }

    if (!m_findDefProcess->isStop()) {
        m_findDefProcess->stopAndWait(100,200);
    }

    m_lastCursor = m_plainTextEdit->textCursor();
    int offset = m_editor->utf8Position(false,selectStart);
    QString cmd = LiteApi::getGotools(m_liteApp);
    m_srcData = m_editor->utf8Data();
    QFileInfo info(m_editor->filePath());
    m_findDefProcess->setEnvironment(LiteApi::getCustomGoEnvironment(m_liteApp,m_editor).toStringList());
    m_findDefProcess->setWorkingDirectory(info.path());

    QStringList args;
    args << "types";
    QString tags = LiteApi::getGoBuildFlagsArgument(m_liteApp,m_editor,"-tags");
    if (!tags.isEmpty()) {
        args << "-tags";
        args << tags;
    }
    args << "-pos";
    args << QString("\"%1:%2\"").arg(info.fileName()).arg(offset);
    args << "-stdin";
    args << "-def";
    args << ".";


    m_findDefProcess->startEx(cmd,args.join(" "));
}

void GolangEdit::editorFindUsages()
{
    QTextCursor cursor = m_plainTextEdit->textCursor();
    m_fileSearch->findUsages(m_editor,cursor,false,false);
}

void GolangEdit::editorFindUsagesGlobal()
{
    QTextCursor cursor = m_plainTextEdit->textCursor();
    m_fileSearch->findUsages(m_editor,cursor,true,false);
}

void GolangEdit::editorRenameSymbol()
{
    QTextCursor cursor = m_plainTextEdit->textCursor();
    m_fileSearch->findUsages(m_editor,cursor,false,true);
}

void GolangEdit::editorRenameSymbolGlobal()
{
    QTextCursor cursor = m_plainTextEdit->textCursor();
    m_fileSearch->findUsages(m_editor,cursor,true,true);
}

void GolangEdit::editorComment()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *textEdit = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!textEdit) {
        return;
    }
    Utils::CommentDefinition cd;
    cd.setAfterWhiteSpaces(false);
    Utils::unCommentSelection(textEdit,Utils::AutoComment,cd);
}

void GolangEdit::editorFindInfo()
{
    QString cmd = LiteApi::getGotools(m_liteApp);
    m_srcData = m_editor->utf8Data();
    QFileInfo info(m_editor->filePath());
    bool moveLeft = false;
    int selectStart = 0;
    QString text = LiteApi::wordUnderCursor(m_plainTextEdit->textCursor(),&moveLeft,&selectStart);
    if (text.isEmpty() || text.contains(" ")) {
        return;
    }

    m_lastCursor = m_plainTextEdit->textCursor();
    int offset = m_editor->utf8Position(false,selectStart);

    QStringList args;
    args << "types";
    QString tags = LiteApi::getGoBuildFlagsArgument(m_liteApp,m_editor,"-tags");
    if (!tags.isEmpty()) {
        args << "-tags";
        args << tags;
    }
    args << "-pos";
    args << QString("\"%1:%2\"").arg(info.fileName()).arg(offset);
    args << "-stdin";
    args << "-info";
    args << "-def";
    args << "-doc";
    args << ".";

    if (0 && isUseGopher(m_liteApp) && m_findInfoGopher.isValid()) {
        QStringList args;
        args << "types";
        QString tags = LiteApi::getGoBuildFlagsArgument(m_liteApp,m_editor,"-tags");
        if (!tags.isEmpty()) {
            args << "-tags";
            args << tags;
        }
        args << "-pos";
        args << QString("%1:%2").arg(info.fileName()).arg(offset);
        args << "-stdin";
        args << "-info";
        args << "-def";
        args << "-doc";
        args << info.path();
        m_findInfoGopher.setEnvironment(LiteApi::getCustomGoEnvironment(m_liteApp,m_editor));
        m_findInfoGopher.invokeAsyncArgsData(args,m_editor->utf8Data());
    }

    if (!m_findInfoProcess->isStop()) {
        m_findInfoProcess->stopAndWait(100,200);
    }
    m_findInfoProcess->setEnvironment(LiteApi::getCustomGoEnvironment(m_liteApp,m_editor).toStringList());
    m_findInfoProcess->setWorkingDirectory(info.path());
    m_findInfoProcess->startEx(cmd,args.join(" "));
}

void GolangEdit::findDefStarted()
{
    m_findDefProcess->write(m_srcData);
    m_findDefProcess->closeWriteChannel();
}

void GolangEdit::findDefFinish(int code,QProcess::ExitStatus status)
{
    if (code != 0) {
        QString err = ProcessEx::exitStatusText(code,status);
        m_liteApp->appendLog("find def error",err,true);
        return;
    }
    QByteArray data = m_findDefProcess->readAllStandardOutput();
    if (data.isEmpty()) {
        return;
    }
    QString info = QString::fromUtf8(data).trimmed();
    QRegExp reg(":(\\d+):(\\d+)");
    int pos = reg.lastIndexIn(info);
    if (pos >= 0) {
        QString fileName = info.left(pos);
        int line = reg.cap(1).toInt();
        int col = reg.cap(2).toInt();
        LiteApi::gotoLine(m_liteApp,fileName,line-1,col-1,true,false);
    }
}

void GolangEdit::findInfoStarted()
{
    m_findInfoProcess->write(m_srcData);
    m_findInfoProcess->closeWriteChannel();
}

void GolangEdit::findInfoFinish(int code, QProcess::ExitStatus)
{
    if (code != 0) {
        return;
    }
    QByteArray data = m_findInfoProcess->readAllStandardOutput();
    if (data.isEmpty()) {
        return;
    }
    if ( m_editor == m_liteApp->editorManager()->currentEditor()) {
        if (m_plainTextEdit->textCursor() == m_lastCursor) {
            QStringList lines = QString::fromUtf8(data).trimmed().split("\n");
            QString info;
            if (lines.size() >= 2) {
                info = formatInfo(lines[1]);
                if (lines.size() >= 3) {
                    info += "\n";
                    for (int i = 2; i < lines.size(); i++) {
                        info += "\n"+lines.at(i);
                    }
                }
            }
            QRect rc = m_plainTextEdit->cursorRect(m_lastCursor);
            QPoint pt = m_plainTextEdit->mapToGlobal(rc.topRight());
            m_editor->showToolTipInfo(pt,info);
        }
    }
}

void GolangEdit::findInfoGopherOutput(const QByteArray &data, bool bError)
{
    qDebug() << bError << data;
}

void GolangEdit::findLinkStarted()
{
    m_findLinkProcess->write(m_srcData);
    m_findLinkProcess->closeWriteChannel();
}

static QStringList FindSourceInfo(LiteApi::IApplication *app, const QString &fileName, int line, int maxLine) {
    QStringList lines;
    LiteApi::IEditor *edit = app->editorManager()->findEditor(fileName,true);
    int digits = 2;
    int max = line+maxLine;
    while (max >= 100) {
        max /= 10;
        ++digits;
    }
    if (edit) {
        QPlainTextEdit *ed = LiteApi::getPlainTextEdit(edit);
        if (ed) {
            QTextBlock block = ed->document()->findBlockByLineNumber(line);
            int index = 0;
            while (block.isValid() && index < maxLine) {
                index++;
                lines.append(QString("%1 %2").arg(line+index,digits).arg(block.text()));
                block = block.next();
            }
        }
    } else {
        QFile f(fileName);
        if (f.open(QFile::ReadOnly)) {
            QTextStream stream(&f);
            stream.setCodec("utf-8");
            int curLine = 0;
            QString text;
            while(!stream.atEnd() && (curLine < (line+maxLine)) ) {
                text = stream.readLine();
                if (curLine >= line) {
                    lines.append(QString("%1 %2").arg(curLine,digits).arg(text));
                }
                curLine++;
            }
        }
    }
    return lines;
}

void GolangEdit::findLinkFinish(int code,QProcess::ExitStatus)
{
    if (code != 0) {
        return;
    }
    QByteArray data = m_findLinkProcess->readAllStandardOutput();
    if ( m_editor == m_liteApp->editorManager()->currentEditor()) {
        if (m_lastLink.hasValidLinkText()) {
            QStringList info = QString::fromUtf8(data).trimmed().split("\n");
            if (info.size() >= 2) {
                if (info[0] != "-") {
                    QRegExp reg(":(\\d+):(\\d+)");
                    int pos = reg.lastIndexIn(info[0]);
                    if (pos >= 0) {
                        QString fileName = info[0].left(pos);
                        int line = reg.cap(1).toInt();
                        int col = reg.cap(2).toInt();
                        m_lastLink.targetFileName = fileName;
                        m_lastLink.targetLine = line-1;
                        m_lastLink.targetColumn = col-1;
                        m_lastLink.targetInfo = formatInfo(info[1]);
                        m_lastLink.sourceInfo = QString("%1\n\n> %2:%3").arg(formatInfo(info[1])).arg(fileName).arg(line);
                        if (m_lastLink.showNav) {
                            int n = 7;
                            if (info.size() >= 3) {
                                for (int i = 2; i < info.size(); i++) {
                                    m_lastLink.sourceInfo += "\n// "+info.at(i);
                                    n--;
                                    if (i >= 5) {
                                        if (i+1 < info.size()) {
                                            m_lastLink.sourceInfo += "\t...";
                                        }
                                        break;
                                    }
                                }
                            }
                            m_lastLink.sourceInfo += "\n\n";
                            m_lastLink.sourceInfo += FindSourceInfo(m_liteApp,fileName,line-1,n).join("\n").replace("\t","    ");
                        }
                        if (m_lastLink.showTip && (info.size() >= 3) ) {
                            m_lastLink.targetInfo += "\n";
                            for (int i = 2; i < info.size(); i++) {
                                m_lastLink.targetInfo += "\n"+info.at(i);
                            }
                        }
                        m_editor->showLink(m_lastLink);
                    }
                } else if (info[0] == "-") {
                    m_lastLink.targetInfo = info[1];
                    m_lastLink.sourceInfo = info[1];
                    m_editor->showLink(m_lastLink);
                } else {
                    m_editor->clearLink();
                }
            }
        }
    }
}

void GolangEdit::searchTextChanged(const QString &/*word*/)
{

}

void GolangEdit::sourceQueryFinished(int code, QProcess::ExitStatus /*status*/)
{
    m_sourceQueryOutputAct->setChecked(true);
   // m_oracleOutput->clear();
    m_sourceQueryOutput->updateExistsTextColor(true);
    if (code != 0) {
        QByteArray data = m_sourceQueryProcess->readAllStandardError();
        m_sourceQueryOutput->append(data,Qt::red);
        return;
    }
    QByteArray data = m_sourceQueryProcess->readAllStandardOutput();
    if (data.isEmpty()) {
        data = m_sourceQueryProcess->readAllStandardError();
    }
    if (data.isEmpty()) {
        m_sourceQueryOutput->append(QString("oracle \"%1\" output is nothing.").arg(m_sourceQueryInfo.action));
        return;
    }
    m_sourceQueryInfo.output = QString::fromUtf8(data);
    m_sourceQueryInfo.success = true;
   // m_oracleOutput->append(m_oracleInfo.output);
    //remove referrers action
    foreach (QString line, QString::fromUtf8(data).split("\n")) {
        //-: modes: [callees callers callstack definition describe implements pointsto referrers]
        if (line.startsWith("-: modes:")) {
            QString mode = line.mid(9);
           // mode.remove(QRegExp("\\s?\\breferrers\\b"));
            if (mode.contains("implements")) {
                mode.replace("implements","implements implements_GOPATH");
            }
            m_sourceQueryInfo.mode = mode;
        }
        m_sourceQueryOutput->append(line+"\n");
    }
    if (!m_sourceQueryInfo.mode.isEmpty()) {
        m_sourceQueryOutput->appendTag(m_sourceQueryInfo.mode+"\n");
    }
}

void GolangEdit::sourcequeryError(QProcess::ProcessError code)
{
    QString data = ProcessEx::processErrorText(code);
    m_sourceQueryOutput->append(data,Qt::red);
}

//void GolangEdit::updateOracleInfo(const QString &action, const QString &text)
//{
//    //if (action == "what") {
//        QRegExp reg("((?:[a-zA-Z]:)?[\\w\\d_\\-\\\\/\\.]+):(\\d+)[\\.:]?(\\d+)?\\-?(\\d+)?\\.?(\\d+)?\\b");
//        foreach (QString line, text.split("\n")) {
//            if (reg.indexIn(line) >= 0) {
//                //qDebug() << reg.capturedTexts();
//                QString text = line.mid(reg.capturedTexts().at(0).length());
//                QString html = QString("<a href=\"file://%1\">%2</a> %3").arg(reg.capturedTexts().at(0)).arg(reg.capturedTexts().at(0)).arg(text);
//                m_oracleOutput->appendHtml(html);
//            }
//        }
//    //}
//    m_oracleOutput->append(text);
//}

void GolangEdit::dbclickSourceQueryOutput(const QTextCursor &cursor)
{
    QTextCursor cur = cursor;
    cur.select(QTextCursor::LineUnderCursor);
    QString text = cur.selectedText();
    if (text.isEmpty()) {
        return;
    }

    bool hasGotoLine = false;
    QRegExp reg("((?:[a-zA-Z]:)?[\\w\\d_\\s\\-\\\\/\\.]+):(\\d+)[\\.:]?(\\d+)?\\-?(\\d+)?\\.?(\\d+)?\\b");
    if (reg.indexIn(text) >= 0) {
        hasGotoLine = true;
    }

    if (!hasGotoLine) {
        QTextCursor cur = cursor;
        cur.select(QTextCursor::WordUnderCursor);
        QString text = cur.selectedText();
        QStringList actions;
        actions << "callees" << "callers" << "callstack" << "definition" << "describe" << "freevars" << "implements" << "implements_GOPATH" << "peers" << "referrers" << "pointsto" << "whicherrs";
        if (actions.contains(text)) {
            if (text.endsWith("_GOPATH")) {
                runSourceQueryByInfo(text.replace("_GOPATH",""),"...");
            } else {
                runSourceQueryByInfo(text);
            }
        }
        return;
    }

    QStringList capList = reg.capturedTexts();
    if (capList.count() < 5) {
        return;
    }
    QString fileName = capList[1];
    QString fileLine = capList[2];
    QString fileCol = capList[3];

    bool ok = false;
    int line = fileLine.toInt(&ok);
    if (!ok)
        return;
    int col = fileCol.toInt(&ok);
    if (ok) {
        col--;
    } else {
        col = 0;
    }

    QDir dir(m_sourceQueryInfo.workPath);
    if (!QFileInfo(fileName).isAbsolute()) {
        fileName = dir.filePath(fileName);
    }
    if (LiteApi::gotoLine(m_liteApp,fileName,line-1,col,true,true)) {
        m_sourceQueryOutput->setTextCursor(cur);
    }
}

void GolangEdit::runSourceQueryAction(const QString &action, const QString &scope)
{
    QTextCursor cursor = m_plainTextEdit->textCursor();
    if (!m_sourceQueryProcess->isStop()) {
        m_sourceQueryProcess->stopAndWait(100,200);
    }
    int offset = -1;
    int offset2 = -1;
    if (cursor.hasSelection()) {
        offset = m_editor->utf8Position(true,cursor.selectionStart());
        offset2 = m_editor->utf8Position(true,cursor.selectionEnd());
    } else {
        bool moveLeft = false;
        int selectStart = 0;
        QString text = LiteApi::wordUnderCursor(cursor,&moveLeft,&selectStart);
        if (text.isEmpty() || text.contains(" ")) {
            return;
        }
        m_liteApp->editorManager()->saveAllEditors(false);
        offset = m_editor->utf8Position(true,selectStart);
    }

    QString cmd;
    QString cmdName;

    QString guruFilePath = FileUtil::lookupGoBin("guru",m_liteApp,true);

    if (!guruFilePath.isEmpty()) {
        cmd = guruFilePath;
        cmdName = "guru";
    } else {
        cmd = LiteApi::getGotools(m_liteApp);
        cmdName = "oracle";
    }

    m_sourceQueryOutputAct->setChecked(true);

    m_sourceQueryOutput->clear();
    m_sourceQueryOutput->append(QString("\nwait for source query, command \"%1\" action \"%2\" scope \"%3\"\n\n").arg(cmdName).arg(action).arg(scope));

    QFileInfo info(m_editor->filePath());

    m_sourceQueryInfo.cmdName = cmdName;
    m_sourceQueryInfo.action = action;
    m_sourceQueryInfo.workPath = info.path();
    m_sourceQueryInfo.filePath = info.filePath();
    m_sourceQueryInfo.fileName = info.fileName();
    m_sourceQueryInfo.output.clear();
    m_sourceQueryInfo.success = false;
    m_sourceQueryInfo.mode.clear();
    m_sourceQueryInfo.offset = offset;
    m_sourceQueryInfo.offset2 = offset2;


    m_sourceQueryProcess->setEnvironment(LiteApi::getCustomGoEnvironment(m_liteApp,m_editor).toStringList());
    m_sourceQueryProcess->setWorkingDirectory(info.path());

    QString fileName = info.fileName();
    QStringList args;
    if (!guruFilePath.isEmpty()) {
        args << "-scope" << scope;
        args << action;
        if (offset2 -= 1) {
            args << QString("\"%1:#%2\"").arg(fileName).arg(offset);
        } else {
            args << QString("\"%1:#%2,#%3\"").arg(fileName).arg(offset).arg(offset2);
        }
    } else {
        args << "oracle";
        if (offset2 -= 1) {
            args << QString("-pos \"%1:#%2\"").arg(fileName).arg(offset);
        } else {
            args << QString("-pos \"%1:#%2,#%3\"").arg(fileName).arg(offset).arg(offset2);
        }
        args << action;
        args << scope;
    }
    m_sourceQueryProcess->startEx(cmd,args.join(" "));
}

void GolangEdit::runSourceQueryByInfo(const QString &action, const QString &scope)
{
    if (!m_sourceQueryProcess->isStop()) {
        m_sourceQueryProcess->stopAndWait(100,200);
    }

    QString cmd;
    QString cmdName;

    QString guruFilePath = FileUtil::lookupGoBin("guru",m_liteApp,true);

    if (!guruFilePath.isEmpty()) {
        cmd = guruFilePath;
        cmdName = "guru";
    } else {
        cmd = LiteApi::getGotools(m_liteApp);
        cmdName = "oracle";
    }

    m_sourceQueryInfo.cmdName = cmdName;
    int offset = m_sourceQueryInfo.offset;
    int offset2 = m_sourceQueryInfo.offset2;

    m_sourceQueryOutput->append(QString("\nwait for source query, command \"%1\" action \"%2\" scope \"%3\" \n\n").arg(cmdName).arg(action).arg(scope));

    m_sourceQueryProcess->setEnvironment(LiteApi::getCustomGoEnvironment(m_liteApp,m_editor).toStringList());
    m_sourceQueryProcess->setWorkingDirectory(m_sourceQueryInfo.workPath);

    QString fileName = m_sourceQueryInfo.fileName;
    QStringList args;
    if (!guruFilePath.isEmpty()) {
        args << "-scope" << scope;
        args << action;
        if (offset2 -= 1) {
            args << QString("\"%1:#%2\"").arg(fileName).arg(offset);
        } else {
            args << QString("\"%1:#%2,#%3\"").arg(fileName).arg(offset).arg(offset2);
        }
    } else {
        args << "oracle";
        if (offset2 -= 1) {
            args << QString("-pos \"%1:#%2\"").arg(fileName).arg(offset);
        } else {
            args << QString("-pos \"%1:#%2,#%3\"").arg(fileName).arg(offset).arg(offset2);
        }
        args << action;
        args << scope;
    }
    m_sourceQueryProcess->startEx(cmd,args.join(" "));
}

void GolangEdit::sourceWhat()
{
    runSourceQueryAction("what");
}

void GolangEdit::sourceCallees()
{
    runSourceQueryAction("callees");
}

void GolangEdit::sourceCallers()
{
    runSourceQueryAction("callers");
}

void GolangEdit::sourceCallstack()
{
    runSourceQueryAction("callstack");
}

void GolangEdit::sourceDefinition()
{
    runSourceQueryAction("definition");
}

void GolangEdit::sourceDescribe()
{
    runSourceQueryAction("describe");
}

void GolangEdit::sourceFreevars()
{
    runSourceQueryAction("freevars");
}

void GolangEdit::sourceImplements()
{
    runSourceQueryAction("implements");
}

void GolangEdit::sourceImplementsGopath()
{
    runSourceQueryAction("implements","...");
}

void GolangEdit::sourcePeers()
{
    runSourceQueryAction("peers");
}

void GolangEdit::sourceReferrers()
{
    runSourceQueryAction("referrers");
}

void GolangEdit::sourcePointsto()
{
    runSourceQueryAction("pointsto");
}

void GolangEdit::sourceWhicherrs()
{
    runSourceQueryAction("whicherrs");
}

void GolangEdit::sourceQueryStateChanged(QProcess::ProcessState state)
{
    m_stopSourceQueryAct->setEnabled(state == QProcess::Running);
}

void GolangEdit::stopSourceQueryProcess()
{
    if (m_sourceQueryProcess->isRunning()) {
        m_sourceQueryProcess->stop(200);
    }
}

QString GolangEdit::getGoModifyTagsInfo() const
{
    QString text;
    QString fileName = QFileInfo(m_editor->filePath()).fileName();
    QTextCursor cursor = m_plainTextEdit->textCursor();
    if (cursor.hasSelection()) {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();
        int line1 = cursor.document()->findBlock(start).blockNumber()+1;
        int line2 = cursor.document()->findBlock(end).blockNumber()+1;
        if (line1 == line2) {
            text = QString("gomodifytags -file %1 -line %2").arg(fileName).arg(line1);
        } else {
            text = QString("gomodifytags -file %1 -line %2,%3").arg(fileName).arg(line1).arg(line2);
        }
    } else {
        text = QString("gomodifytags -file %1 -offset %2 (Inside a valid structure under the cursor)").arg(fileName).arg(m_editor->utf8Position(true));
    }
    return text;
}

void GolangEdit::execGoModifyTags(const QString &args)
{
    if (args.isEmpty()) {
        return;
    }
    QString cmd = FileUtil::lookupGoBin("gomodifytags",m_liteApp,true);
    if (cmd.isEmpty()) {
         m_liteApp->appendLog("GolangEdit","Could not find gomodifytags (hint: is gomodifytags installed?)",true);
         return;
    }
    QProcessEnvironment env = LiteApi::getCurrentEnvironment(m_liteApp);
    QFileInfo info(m_editor->filePath());
    Process process(this);
    process.setEnvironment(env.toStringList());
    process.setWorkingDirectory(info.path());
    QString cmdArgs;
    QTextCursor cursor = m_plainTextEdit->textCursor();
    if (cursor.hasSelection()) {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();
        int line1 = cursor.document()->findBlock(start).blockNumber()+1;
        int line2 = cursor.document()->findBlock(end).blockNumber()+1;
        if (line1 == line2) {
            cmdArgs = QString("-file %1 -line %2 %3").arg(info.fileName()).arg(line1).arg(args);
        } else {
            cmdArgs = QString("-file %1 -line %2,%3 %4").arg(info.fileName()).arg(line1).arg(line2).arg(args);
        }
    } else {
        cmdArgs = QString("-file %1 -offset %2 %3").arg(info.fileName()).arg(m_editor->utf8Position(true)).arg(args);
    }
    process.startEx(cmd,cmdArgs);
    if (!process.waitForStarted(30000)) {
        m_liteApp->appendLog("GolangEdit","wait for gomodifytags started timeout",true);
        return;
    }
    if (!process.waitForFinished(30000)) {
        process.kill();
        m_liteApp->appendLog("GolangEdit","wait for gomodifytags finished timeout",true);
        return;
    }
    int code = process.exitCode();
    if (code != 0) {
        QByteArray error = process.readAllStandardError();
        m_liteApp->appendLog("GolangEdit",QString("gomodifytags exit code %1, %2").arg(code).arg(QString::fromUtf8(error)),true);
        return;
    }
    QByteArray data = process.readAllStandardOutput();
    if (data.isEmpty()) {
        return;
    }
    m_editor->loadTextUseDiff(QString::fromUtf8(data));
}

void GolangEdit::goAddTags()
{
    if (m_editor->isModified()) {
        m_liteApp->editorManager()->saveEditor(m_editor);
    }
    if (!m_addTagsDlg) {
        m_addTagsDlg = new GoAddTagsDialog(m_liteApp->mainWindow());
    }
    m_addTagsDlg->setInfo(getGoModifyTagsInfo());
    if (m_addTagsDlg->exec() != QDialog::Accepted) {
        return;
    }
    QString args = m_addTagsDlg->arguments();
    if (args.isEmpty()) {
        return;
    }
    execGoModifyTags(args);
}

void GolangEdit::goRemoveTags()
{
    if (m_editor->isModified()) {
        m_liteApp->editorManager()->saveEditor(m_editor);
    }
    if (!m_removeTagsDlg) {
        m_removeTagsDlg = new GoRemoveTagsDialog(m_liteApp->mainWindow());
    }
    m_removeTagsDlg->setInfo(getGoModifyTagsInfo());
    if (m_removeTagsDlg->exec() != QDialog::Accepted) {
        return;
    }
    QString args = m_removeTagsDlg->arguments();
    if (args.isEmpty()) {
        return;
    }
    execGoModifyTags(args);
}

