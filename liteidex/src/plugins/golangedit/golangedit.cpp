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
// Module: golangedit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangedit.h"
#include "golangtextlexer.h"
#include "golangedit_global.h"
#include "qtc_editutil/uncommentselection.h"
#include "litebuildapi/litebuildapi.h"
#include "golangdocapi/golangdocapi.h"
#include "fileutil/fileutil.h"
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTextStream>
#include <QApplication>
#include <QToolTip>
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
    QObject(parent), m_liteApp(app)
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

    m_fileSearch = new GolangFileSearch(app,this);

    LiteApi::IFileSearchManager *manager = LiteApi::getFileSearchManager(app);
    if (manager) {
        manager->addFileSearch(m_fileSearch);
    }

    m_findDefProcess = new Process(this);
    m_findInfoProcess = new Process(this);
    m_findLinkProcess = new Process(this);
    m_oracleProcess = new Process(this);
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
    connect(m_fileSearch,SIGNAL(searchTextChanged(QString)),this,SLOT(searchTextChanged(QString)));

    connect(m_oracleProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(oracleFinished(int,QProcess::ExitStatus)));

    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));

    m_oracleOutput = new TextOutput(m_liteApp,true);

    connect(m_oracleOutput,SIGNAL(dbclickEvent(QTextCursor)),this,SLOT(dbclickOracleOutput(QTextCursor)));

    m_oracleOutputAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,m_oracleOutput,"oracle",tr("Oracle"),true);

    m_oracleWhatAct = new QAction(tr("Oracle What"),this);
    actionContext->regAction(m_oracleWhatAct,"OracleWhat","Ctrl+Shift+H");
    connect(m_oracleWhatAct,SIGNAL(triggered()),this,SLOT(oracleWhat()));

    m_oracleCalleesAct = new QAction(tr("Callees"),this);
    actionContext->regAction(m_oracleCalleesAct,"OracleCallees","");
    connect(m_oracleCalleesAct,SIGNAL(triggered()),this,SLOT(oracleCallees()));

    m_oracleCallersAct = new QAction(tr("Callers"),this);
    actionContext->regAction(m_oracleCallersAct,"OracleCallers","");
    connect(m_oracleCallersAct,SIGNAL(triggered()),this,SLOT(oracleCallers()));

    m_oracleCallstackAct = new QAction(tr("Callstack"),this);
    actionContext->regAction(m_oracleCallstackAct,"OracleCallstack","");
    connect(m_oracleCallstackAct,SIGNAL(triggered()),this,SLOT(oracleCallstack()));

    m_oracleDefinitionAct = new QAction(tr("Definition"),this);
    actionContext->regAction(m_oracleDefinitionAct,"OracleDefinition","");
    connect(m_oracleDefinitionAct,SIGNAL(triggered()),this,SLOT(oracleDefinition()));

    m_oracleDescribeAct = new QAction(tr("Describe"),this);
    actionContext->regAction(m_oracleDescribeAct,"OracleDescribe","");
    connect(m_oracleDescribeAct,SIGNAL(triggered()),this,SLOT(oracleDescribe()));

    m_oracleFreevarsAct = new QAction(tr("Freevars"),this);
    actionContext->regAction(m_oracleFreevarsAct,"OracleFreevars","");
    connect(m_oracleFreevarsAct,SIGNAL(triggered()),this,SLOT(oracleFreevars()));

    m_oracleImplementsAct = new QAction(tr("Implements"),this);
    actionContext->regAction(m_oracleImplementsAct,"OracleImplements","");
    connect(m_oracleImplementsAct,SIGNAL(triggered()),this,SLOT(oracleImplements()));

    m_oraclePeersAct = new QAction(tr("Peers"),this);
    actionContext->regAction(m_oraclePeersAct,"OraclePeers","");
    connect(m_oraclePeersAct,SIGNAL(triggered()),this,SLOT(oraclePeers()));

    m_oracleReferrersAct = new QAction(tr("Referrers"),this);
    actionContext->regAction(m_oracleReferrersAct,"OracleReferrers","");
    connect(m_oracleReferrersAct,SIGNAL(triggered()),this,SLOT(oracleReferrers()));

    m_oraclePointstoAct = new QAction(tr("Pointsto"),this);
    actionContext->regAction(m_oraclePointstoAct,"OraclePointsto","");
    connect(m_oraclePointstoAct,SIGNAL(triggered()),this,SLOT(oraclePointsto()));

    m_oracleWhicherrs = new QAction(tr("Whicherrs"),this);
    actionContext->regAction(m_oracleWhicherrs,"OracleWhicherrs","");
    connect(m_oracleWhicherrs,SIGNAL(triggered()),this,SLOT(oracleWhicherrs()));

    this->applyOption(OPTION_GOLANGEDIT);
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
}

void GolangEdit::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor || editor->mimeType() != "text/x-gosrc") {
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
        menu->addAction(m_oracleWhatAct);
        sub = menu->addMenu("Oracle");
        sub->addAction(m_oracleCalleesAct);
        sub->addAction(m_oracleCallersAct);
        sub->addAction(m_oracleCallstackAct);
        sub->addAction(m_oracleDefinitionAct);
        sub->addAction(m_oracleDescribeAct);
        sub->addAction(m_oracleFreevarsAct);
        sub->addAction(m_oracleImplementsAct);
        sub->addAction(m_oraclePeersAct);
        sub->addAction(m_oraclePointstoAct);
        sub->addAction(m_oracleWhicherrs);
        sub->addAction(m_oracleReferrersAct);
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
        menu->addAction(m_oracleWhatAct);
        sub = menu->addMenu("Oracle");
        sub->addAction(m_oracleCalleesAct);
        sub->addAction(m_oracleCallersAct);
        sub->addAction(m_oracleCallstackAct);
        sub->addAction(m_oracleDefinitionAct);
        sub->addAction(m_oracleDescribeAct);
        sub->addAction(m_oracleFreevarsAct);
        sub->addAction(m_oracleImplementsAct);
        sub->addAction(m_oraclePeersAct);
        sub->addAction(m_oraclePointstoAct);
        sub->addAction(m_oracleWhicherrs);
        sub->addAction(m_oracleReferrersAct);
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
            m_lastLink.showTip = !nav;
            m_editor->showLink(m_lastLink);
            return;
        }
        return;
    }
    if (m_findLinkProcess->state() != QProcess::NotRunning) {
        if (!m_findLinkProcess->waitForFinished(100)) {
            m_findLinkProcess->kill();
            if (!m_findLinkProcess->waitForFinished(100)) {
                m_liteApp->appendLog("golang","find link timeout",false);
                return;
            }
        }
    }

    m_lastLink.clear();
    m_lastLink.showTip = !nav;
    m_lastLink.linkTextStart = cursor.selectionStart();
    m_lastLink.linkTextEnd = cursor.selectionEnd();
    m_lastLink.cursorPos = pos;

    QString cmd = LiteApi::getGotools(m_liteApp);

    m_srcData = m_editor->utf8Data();
    int offset = m_editor->utf8Position(false,cursor.selectionStart());

    QFileInfo info(m_editor->filePath());
    m_findLinkProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findLinkProcess->setWorkingDirectory(info.path());
    m_findLinkProcess->startEx(cmd,QString("types -b -pos %1:%2 -stdin -def -info -doc .").
                             arg(info.fileName()).
                               arg(offset));
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
    QString text = LiteApi::wordUnderCursor(m_plainTextEdit->textCursor(),&moveLeft);
    if (text.isEmpty()) {
        return;
    }
    m_lastCursor = m_plainTextEdit->textCursor();
    int offset = moveLeft?m_editor->utf8Position()-1:m_editor->utf8Position();
    QString cmd = LiteApi::getGotools(m_liteApp);
    m_srcData = m_editor->utf8Data();
    QFileInfo info(m_editor->filePath());
    m_findDefProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findDefProcess->setWorkingDirectory(info.path());
    m_findDefProcess->startEx(cmd,QString("types -pos %1:%2 -stdin -def .").
                             arg(info.fileName()).
                              arg(offset));
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
    QString text = LiteApi::wordUnderCursor(m_plainTextEdit->textCursor(),&moveLeft);
    if (text.isEmpty()) {
        return;
    }
    m_lastCursor = m_plainTextEdit->textCursor();
    int offset = moveLeft?m_editor->utf8Position()-1:m_editor->utf8Position();

    m_findInfoProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_findInfoProcess->setWorkingDirectory(info.path());
    m_findInfoProcess->startEx(cmd,QString("types -pos %1:%2 -stdin -info -def -doc .").
                             arg(info.fileName()).
                             arg(offset));
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
            QToolTip::showText(pt,info,m_plainTextEdit);
        }
    }
}

void GolangEdit::findLinkStarted()
{
    m_findLinkProcess->write(m_srcData);
    m_findLinkProcess->closeWriteChannel();
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
                        if (info.size() >= 3) {
                            m_lastLink.targetInfo += "\n";
                            for (int i = 2; i < info.size(); i++) {
                                m_lastLink.targetInfo += "\n"+info.at(i);
                            }
                        }
                        m_editor->showLink(m_lastLink);
                    }
                } else if (info[0] == "-") {
                    m_lastLink.targetInfo = info[1];
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

void GolangEdit::oracleFinished(int code, QProcess::ExitStatus /*status*/)
{
    m_oracleOutputAct->setChecked(true);
   // m_oracleOutput->clear();
    m_oracleOutput->updateExistsTextColor(true);
    if (code != 0) {
        QByteArray data = m_oracleProcess->readAllStandardError();
        m_oracleOutput->append(data,Qt::red);
        return;
    }
    QByteArray data = m_oracleProcess->readAllStandardOutput();
    if (data.isEmpty()) {
        data = m_oracleProcess->readAllStandardError();
    }
    if (data.isEmpty()) {
        m_oracleOutput->append(QString("oracle \"%1\" output is nothing.").arg(m_oracleInfo.action));
        return;
    }
    m_oracleInfo.output = QString::fromUtf8(data);
    m_oracleInfo.success = true;
   // m_oracleOutput->append(m_oracleInfo.output);
    //remove referrers action
    foreach (QString line, QString::fromUtf8(data).split("\n")) {
        //-: modes: [callees callers callstack definition describe implements pointsto referrers]
        if (line.startsWith("-: modes:")) {
            QString mode = line.mid(9);
           // mode.remove(QRegExp("\\s?\\breferrers\\b"));
            m_oracleInfo.mode = mode;
        }
        m_oracleOutput->append(line+"\n");
    }
    if (!m_oracleInfo.mode.isEmpty()) {
        m_oracleOutput->appendTag(m_oracleInfo.mode+"\n");
    }
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

void GolangEdit::dbclickOracleOutput(const QTextCursor &cursor)
{
    QTextCursor cur = cursor;
    cur.select(QTextCursor::WordUnderCursor);
    QStringList actions;
    actions << "callees" << "callers" << "callstack" << "definition" << "describe" << "freevars" << "implements" << "peers" << "referrers" << "pointsto" << "whicherrs";
    QString text = cur.selectedText();
    if (actions.contains(text)) {
        runOracleByInfo(text);
        return;
    }
    cur.select(QTextCursor::LineUnderCursor);
    text = cur.selectedText();
    if (text.isEmpty()) {
        return;
    }
    QRegExp reg("((?:[a-zA-Z]:)?[\\w\\d_\\-\\\\/\\.]+):(\\d+)[\\.:]?(\\d+)?\\-?(\\d+)?\\.?(\\d+)?\\b");
    if (reg.indexIn(text) < 0) {
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

    QDir dir(m_oracleInfo.workPath);
    if (!QFileInfo(fileName).isAbsolute()) {
        fileName = dir.filePath(fileName);
    }
    if (LiteApi::gotoLine(m_liteApp,fileName,line-1,col,true,true)) {
        m_oracleOutput->setTextCursor(cur);
    }
}

void GolangEdit::runOracle(const QString &action)
{
    QTextCursor cursor = m_plainTextEdit->textCursor();
    if (m_oracleProcess->isRunning()) {
        return;
    }
    int offset = -1;
    int offset2 = -1;
    if (cursor.hasSelection()) {
        offset = m_editor->utf8Position(true,cursor.selectionStart());
        offset2 = m_editor->utf8Position(true,cursor.selectionEnd());
    } else {
        bool moveLeft = false;
        QString text = LiteApi::wordUnderCursor(cursor,&moveLeft);
        if (text.isEmpty()) {
            return;
        }
        m_liteApp->editorManager()->saveAllEditors(false);
        offset = moveLeft ? m_editor->utf8Position(true)-1: m_editor->utf8Position(true);
    }

    m_oracleOutput->clear();
    m_oracleOutput->append(QString("wait for oracle %1 ...\n").arg(action));

    QFileInfo info(m_editor->filePath());

    m_oracleInfo.action = action;
    m_oracleInfo.workPath = info.path();
    m_oracleInfo.filePath = info.filePath();
    m_oracleInfo.fileName = info.fileName();
    m_oracleInfo.output.clear();
    m_oracleInfo.success = false;
    m_oracleInfo.mode.clear();
    m_oracleInfo.offset = offset;
    m_oracleInfo.offset2 = offset2;

    QString cmd = LiteApi::getGotools(m_liteApp);
    m_oracleProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_oracleProcess->setWorkingDirectory(info.path());
    if (offset2 == -1) {
        m_oracleProcess->startEx(cmd,QString("oracle -pos %1:#%2 %3 .").
                                 arg(info.fileName()).arg(offset).arg(action));
    } else {
        m_oracleProcess->startEx(cmd,QString("oracle -pos %1:#%2,#%3 %4 .").
                                 arg(info.fileName()).arg(offset).arg(offset2).arg(action));

    }
}

void GolangEdit::runOracleByInfo(const QString &action)
{
//    if (action == "referrers") {
//        return;
//    }
    if (m_oracleProcess->isRunning()) {
        return;
    }
    QString cmd = LiteApi::getGotools(m_liteApp);

    int offset = m_oracleInfo.offset;
    int offset2 = m_oracleInfo.offset2;

    m_oracleOutput->append(QString("\nwait for oracle %1 ...\n").arg(action));

    m_oracleProcess->setEnvironment(LiteApi::getGoEnvironment(m_liteApp).toStringList());
    m_oracleProcess->setWorkingDirectory(m_oracleInfo.workPath);
    if (offset2 == -1) {
        m_oracleProcess->startEx(cmd,QString("oracle -pos %1:#%2 %3 .").
                                 arg(m_oracleInfo.fileName).arg(offset).arg(action));
    } else {
        m_oracleProcess->startEx(cmd,QString("oracle -pos %1:#%2,#%3 %4 .").
                                 arg(m_oracleInfo.fileName).arg(offset).arg(offset2).arg(action));

    }
}

void GolangEdit::oracleWhat()
{
    runOracle("what");
}

void GolangEdit::oracleCallees()
{
    runOracle("calless");
}

void GolangEdit::oracleCallers()
{
    runOracle("callers");
}

void GolangEdit::oracleCallstack()
{
    runOracle("callstack");
}

void GolangEdit::oracleDefinition()
{
    runOracle("definition");
}

void GolangEdit::oracleDescribe()
{
    runOracle("describe");
}

void GolangEdit::oracleFreevars()
{
    runOracle("freevars");
}

void GolangEdit::oracleImplements()
{
    runOracle("implements");
}

void GolangEdit::oraclePeers()
{
    runOracle("peers");
}

void GolangEdit::oracleReferrers()
{
    runOracle("referrers");
}

void GolangEdit::oraclePointsto()
{
    runOracle("pointsto");
}

void GolangEdit::oracleWhicherrs()
{
    runOracle("whicherrs");
}

