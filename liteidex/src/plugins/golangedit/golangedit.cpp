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

    m_findUseGlobalAct = new QAction(QString("%1 (Global)").arg(tr("Find Usages")),this);
    actionContext->regAction(m_findUseGlobalAct,"FindUsagesGOPATH","");

    m_renameSymbolGlobalAct = new QAction(QString("%1 (Global)").arg(tr("Rename Symbol Under Cursor")),this);
    actionContext->regAction(m_renameSymbolGlobalAct,"RenameSymbolGOPATH","");

    m_fileSearch = new GolangFileSearch(app,this);

    LiteApi::IFileSearchManager *manager = LiteApi::getFileSearchManager(app);
    if (manager) {
        manager->addFileSearch(m_fileSearch);
    }

    m_findDefProcess = new Process(this);
    m_findInfoProcess = new Process(this);
    m_findLinkProcess = new Process(this);
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

    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));
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
        menu->addSeparator();
        menu->addAction(m_findUseAct);
        menu->addAction(m_renameSymbolAct);
        menu->addSeparator();
        QMenu *sub = menu->addMenu("GoTools");
        sub->addAction(m_findUseGlobalAct);
        sub->addAction(m_renameSymbolGlobalAct);
    }
    menu = LiteApi::getContextMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(m_viewGodocAct);
        menu->addSeparator();
        menu->addAction(m_findInfoAct);
        menu->addAction(m_jumpDeclAct);
        menu->addSeparator();
        menu->addAction(m_findUseAct);
        menu->addAction(m_renameSymbolAct);
        menu->addSeparator();
        QMenu *sub = menu->addMenu("GoTools");
        sub->addAction(m_findUseGlobalAct);
        sub->addAction(m_renameSymbolGlobalAct);
        connect(menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowContextMenu()));
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
    m_lastLink.showTip = true;
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

