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
// Module: golangpresentedit.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangpresentedit.h"
#include "editorutil/editorutil.h"
#include "fileutil/fileutil.h"
#include "liteenvapi/liteenvapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "exportdialog.h"
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QTextCursor>
#include <QTextBlock>
#include <QProcess>
#include <QFileDialog>
#include <QDesktopServices>
#include <QRegExp>
#include <QDebug>
#include "memory.h"

GolangPresentEdit::GolangPresentEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent) :
    QObject(parent), m_liteApp(app), m_htmldoc(0), m_process(0)
{
    m_editor = LiteApi::getLiteEditor(editor);
    if (!m_editor) {
        return;
    }
    m_ed = LiteApi::getPlainTextEdit(editor);
    m_editor->setWordWrap(true);

    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GoSlide");

    QAction *s1 = new QAction(QIcon("icon:golangpresent/images/s1.png"),tr("Section (s1)"),this);
    actionContext->regAction(s1,"Section","Ctrl+1");

    QAction *s2 = new QAction(QIcon("icon:golangpresent/images/s2.png"),tr("Subsection (s2)"),this);
    actionContext->regAction(s2,"Subsection","Ctrl+2");

    QAction *s3 = new QAction(QIcon("icon:golangpresent/images/s3.png"),tr("Sub-subsection (s3)"),this);
    actionContext->regAction(s3,"Sub-subsection","Ctrl+3");

    QAction *bold = new QAction(QIcon("icon:golangpresent/images/bold.png"),tr("Bold"),this);
    actionContext->regAction(bold,"Bold",QKeySequence::Bold);

    QAction *italic = new QAction(QIcon("icon:golangpresent/images/italic.png"),tr("Italic"),this);
    actionContext->regAction(italic,"Italic",QKeySequence::Italic);

    QAction *code = new QAction(QIcon("icon:golangpresent/images/code.png"),tr("Inline Code"),this);
    actionContext->regAction(code,"InlineCode","Ctrl+K");

    QAction *bullets = new QAction(QIcon("icon:golangpresent/images/bullets.png"),tr("Switch Bullets"),this);
    actionContext->regAction(bullets,"Switch Bullets","Ctrl+Shift+U");

    QAction *comment = new QAction(tr("Comment/Uncomment Selection"),this);
    actionContext->regAction(comment,"Comment","Ctrl+/");

    QAction *exportHtml = new QAction(QIcon("icon:golangpresent/images/exporthtml.png"),tr("Export HTML"),this);
    actionContext->regAction(exportHtml,"Export HTML","");

    QAction *verify = new QAction(QIcon("icon:golangpresent/images/verify.png"),tr("Verify Present"),this);
    actionContext->regAction(verify,"Verify Present","");

    //QAction *exportPdf = new QAction(QIcon("icon:golangpresent/images/exportpdf.png"),tr("Export PDF"),this);
    //actionContext->regAction(exportPdf,"Export PDF","");

    connect(m_editor,SIGNAL(destroyed()),this,SLOT(deleteLater()));
    connect(s1,SIGNAL(triggered()),this,SLOT(s1()));
    connect(s2,SIGNAL(triggered()),this,SLOT(s2()));
    connect(s3,SIGNAL(triggered()),this,SLOT(s3()));
    connect(bold,SIGNAL(triggered()),this,SLOT(bold()));
    connect(italic,SIGNAL(triggered()),this,SLOT(italic()));
    connect(code,SIGNAL(triggered()),this,SLOT(code()));
    connect(bullets,SIGNAL(triggered()),this,SLOT(bullets()));
    connect(comment,SIGNAL(triggered()),this,SLOT(comment()));
    connect(exportHtml,SIGNAL(triggered()),this,SLOT(exportHtml()));
    connect(verify,SIGNAL(triggered()),this,SLOT(verify()));
    //connect(exportPdf,SIGNAL(triggered()),this,SLOT(exportPdf()));

    QToolBar *toolBar = LiteApi::getEditToolBar(editor);
    if (toolBar) {
        toolBar->addSeparator();
        toolBar->addAction(s1);
        toolBar->addAction(s2);
        toolBar->addAction(s3);
        toolBar->addSeparator();
        toolBar->addAction(bold);
        toolBar->addAction(italic);
        toolBar->addAction(code);
        toolBar->addSeparator();
        toolBar->addAction(bullets);
        toolBar->addSeparator();
        toolBar->addAction(verify);
        toolBar->addSeparator();
        toolBar->addAction(exportHtml);
        //toolBar->addAction(exportPdf);
    }

    QMenu *menu = LiteApi::getEditMenu(editor);
    if (menu) {
        menu->addSeparator();
        menu->addAction(s1);
        menu->addAction(s2);
        menu->addAction(s3);
        menu->addSeparator();
        menu->addAction(bold);
        menu->addAction(italic);
        menu->addAction(code);
        menu->addSeparator();
        menu->addAction(bullets);
        menu->addSeparator();
        menu->addAction(comment);
        menu->addSeparator();
        menu->addAction(verify);
        menu->addSeparator();
        menu->addAction(exportHtml);
        //menu->addAction(exportPdf);
    }

    menu = LiteApi::getContextMenu(editor);
    if (menu) {
       menu->addSeparator();
       menu->addAction(s1);
       menu->addAction(s2);
       menu->addAction(s3);
       menu->addSeparator();
       menu->addAction(bold);
       menu->addAction(italic);
       menu->addAction(code);
       menu->addSeparator();
       menu->addAction(bullets);
       menu->addSeparator();
       menu->addAction(comment);
    }
}

void GolangPresentEdit::editorSaved(LiteApi::IEditor *editor)
{
    if (editor == m_editor) {
        this->verify();
    }
}

void GolangPresentEdit::s1()
{
    EditorUtil::InsertHead(m_ed,"* ");
}

void GolangPresentEdit::s2()
{
    EditorUtil::InsertHead(m_ed,"** ");
}

void GolangPresentEdit::s3()
{
    EditorUtil::InsertHead(m_ed,"*** ");
}

void GolangPresentEdit::bold()
{
    EditorUtil::MarkSelection(m_ed,"*");
}

void GolangPresentEdit::italic()
{
    EditorUtil::MarkSelection(m_ed,"_");
}

void GolangPresentEdit::code()
{
    EditorUtil::MarkSelection(m_ed,"`");
}

void GolangPresentEdit::bullets()
{
    EditorUtil::SwitchHead(m_ed,"- ",QStringList() << "- ");
}

void GolangPresentEdit::comment()
{
    EditorUtil::SwitchHead(m_ed,"# ",QStringList() << "# " << "#");
}

void GolangPresentEdit::verify()
{
    startExportHtmlDoc(EXPORT_TYPE_VERIFY);
}

void GolangPresentEdit::exportHtml()
{
    startExportHtmlDoc(EXPORT_TYPE_HTML);
}

void GolangPresentEdit::exportPdf()
{
    startExportHtmlDoc(EXPORT_TYPE_PDF);
}

void GolangPresentEdit::extOutput(const QByteArray &data, bool bError)
{
    if (!bError) {
        m_exportData.append(data);
    } else {
        QString msg = QString::fromUtf8(data);
        m_editor->setNavigateHead(LiteApi::EditorNavigateError,msg);
        QRegExp re("(\\w?:?[\\w\\d_\\-\\\\/\\.]+):(\\d+):");
        if ((re.indexIn(msg)>=0) && re.captureCount() >= 2) {
            bool ok = false;
            int line = re.cap(2).toInt(&ok);
            if (ok) {
                QString errmsg = re.cap(0)+"\n"+msg.mid(re.cap(0).length()).trimmed();
                m_editor->insertNavigateMark(line-1,LiteApi::EditorNavigateError,errmsg, GOPRESENT_TAG);
                m_errorMsg.append(errmsg);
            }
        } else {
            m_errorMsg.append(msg);
        }
    }
}

void GolangPresentEdit::extFinish(bool error, int code, QString /*msg*/)
{
    int exportType = m_process->userData(0).toInt();
    if (exportType == EXPORT_TYPE_VERIFY) {
        if (!error && code == 0) {
            m_editor->setNavigateHead(LiteApi::EditorNavigateNormal,tr("Present verify success"));
        } else {
            m_editor->setNavigateHead(LiteApi::EditorNavigateError,tr("Present verify false")+"\n"+m_errorMsg.join("\n"));
        }
    } else if (error || code != 0) {
        m_liteApp->appendLog("GolangPresent",m_errorMsg.join(";"),true);
    }
    if (!error && code == 0) {
        if (exportType == EXPORT_TYPE_HTML) {
            QFileInfo info(m_editor->filePath());
            ExportDialog dlg(m_liteApp->mainWindow());
            dlg.setWindowTitle(tr("Export HTML"));
            dlg.setFileName(info.completeBaseName()+".html");
            if (!m_exportName.isEmpty()) {
                dlg.setFileName(m_exportName);
            }
            if (dlg.exec() == QDialog::Rejected) {
                return;
            }
            QString exportFile = dlg.fileName();
            if (exportFile.isEmpty()) {
                return;
            }
            m_exportName = exportFile;
            QFileInfo exportInfo(exportFile);
            QString ext = exportInfo.suffix().toLower();
            if (ext != "html" && ext != "htm") {
                exportInfo = QFileInfo(exportFile += ".html");
            }
            if (!exportInfo.isAbsolute()) {
                exportInfo = QFileInfo(info.absolutePath(),exportInfo.fileName());
            }
            QFile file(exportInfo.filePath());
            if (!file.open(QFile::WriteOnly)) {
                return;
            }
            file.write(m_exportData);
            QDir dir(exportInfo.absolutePath());
            dir.mkdir("static");
            dir.mkdir("js");
            FileUtil::CopyDirectory(m_liteApp->resourcePath()+"/packages/gopresent/static",dir.path()+"/static");
            FileUtil::CopyDirectory(m_liteApp->resourcePath()+"/packages/gopresent/js",dir.path()+"/js");
            m_liteApp->appendLog("GoPresent","export "+exportInfo.filePath(),false);
            if (dlg.isExportAndView()) {
                LiteApi::IWebKitBrowser *browser = LiteApi::getWebKitBrowser(m_liteApp);
                if (browser) {
                    m_liteApp->editorManager()->activeBrowser(browser);
                    browser->openUrl(QUrl::fromLocalFile(exportInfo.filePath()));
                } else {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(exportInfo.filePath()));
                }
            }
        } else if (exportType == EXPORT_TYPE_PDF) {
            QString init = QFileInfo(m_editor->filePath()).absolutePath()+"/"+QFileInfo(m_editor->filePath()).completeBaseName()+".pdf";
            m_pdfFileName = QFileDialog::getSaveFileName(m_liteApp->mainWindow(),tr("Export PDF"),init,"*.pdf");
            if (m_pdfFileName.isEmpty()) {
                return;
            }
            if (!m_htmldoc) {
                m_htmldoc = m_liteApp->htmlWidgetManager()->createDocument(this);
                connect(m_htmldoc,SIGNAL(loadFinished(bool)),this,SLOT(loadHtmlFinished(bool)));
            }
            QUrl url = QUrl::fromLocalFile(m_liteApp->resourcePath()+"/packages/gopresent/export.html");
            m_htmldoc->setHtml(QString::fromUtf8(m_exportData),url);
        }
    }
}

void GolangPresentEdit::loadHtmlFinished(bool b)
{
    if (!b) {
        m_liteApp->appendLog("GolangPresent","Failed export PDF document!");
        return;
    }
#ifndef QT_NO_PRINTER
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setCreator("LiteIDE");
        printer.setOutputFileName(m_pdfFileName);
        m_htmldoc->print(&printer);
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(m_pdfFileName).path()));
#endif
}

bool GolangPresentEdit::startExportHtmlDoc(EXPORT_TYPE type)
{
    m_liteApp->editorManager()->saveEditor(m_editor);
    QString cmd = LiteApi::getGotools(m_liteApp);
    QFileInfo info(m_editor->filePath());
    if (!m_process) {
        m_process = new ProcessEx(this);
        m_process->setWorkingDirectory(info.absolutePath());
        connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(extOutput(QByteArray,bool)));
        connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(extFinish(bool,int,QString)));
    }
    if (m_process->isRunning()) {
        m_process->waitForFinished(3000);
        if (m_process->isRunning()) {
            return false;
        }
    }
    m_exportData.clear();
    m_errorMsg.clear();
    m_process->setUserData(0,type);
    m_editor->clearAllNavigateMark(LiteApi::EditorNavigateBad, GOPRESENT_TAG);
    if (type == EXPORT_TYPE_VERIFY) {
        m_process->startEx(cmd,"gopresent -v -i "+info.fileName().toUtf8());
    } else {
        m_process->startEx(cmd,"gopresent -stdout -i "+info.fileName().toUtf8());
    }
    return true;
}
