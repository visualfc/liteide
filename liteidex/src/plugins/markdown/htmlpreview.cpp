/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: htmlpreview.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "htmlpreview.h"
#include "sundown/mdtohtml.h"
#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QFileInfo>
#include <QTextCodec>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolTip>
#include <QTimer>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintPreviewDialog>
#endif

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

HtmlPreview::HtmlPreview(LiteApi::IApplication *app,QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_curEditor = 0;
    m_curTextEditor = 0;
    m_widget = new QWidget;
    m_htmlWidget = 0;
    m_bWebkit = false;

    m_cssMenu = new QMenu(tr("Page Style"));
    m_cssMenu->setIcon(QIcon("icon:/markdown/images/css.png"));

    m_cssActGroup = new QActionGroup(this);

    m_reloadAct = new QAction(QIcon("icon:images/reload.png"),tr("Reload"),this);
    m_exportHtmlAct = new QAction(QIcon("icon:liteeditor/images/exporthtml.png"),tr("Export Html"),this);
    m_exportPdfAct = new QAction(QIcon("icon:liteeditor/images/exportpdf.png"),tr("Export PDF"),this);
    m_printPreviewAct = new QAction(QIcon("icon:liteeditor/images/fileprintpreview.png"),tr("Print Preview"),this);
    m_printPreviewAct->setVisible(false);

    m_configAct = new QAction(QIcon("icon:markdown/images/config.png"),tr("Configuration"),this);


    m_syncScrollAct = new QAction(tr("Synchronize preview and code scrollbars"),this);
    m_syncScrollAct->setCheckable(true);

    m_syncSwitchAct = new QAction(tr("Automatically display preview"),this);
    m_syncSwitchAct->setCheckable(true);

    m_configMenu = new QMenu(m_widget);
    m_configMenu->addAction(m_syncSwitchAct);
    m_configMenu->addAction(m_syncScrollAct);
    m_configAct->setMenu(m_configMenu);

    QList<QAction*> actions;
    actions << m_configMenu->menuAction() << m_reloadAct << m_exportHtmlAct << m_exportPdfAct << m_printPreviewAct << m_cssMenu->menuAction();

    m_toolAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::RightDockWidgetArea,
                                                  m_widget,
                                                  QString("HtmlPreview"),
                                                  QString(tr("HTML Preview")),
                                                  false,
                                                  actions);


    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_toolAct,SIGNAL(toggled(bool)),this,SLOT(triggeredTool(bool)));
    connect(m_exportHtmlAct,SIGNAL(triggered()),this,SLOT(exportHtml()));
    connect(m_exportPdfAct,SIGNAL(triggered()),this,SLOT(exportPdf()));
    connect(m_printPreviewAct,SIGNAL(triggered()),this,SLOT(printPreview()));
    connect(m_cssActGroup,SIGNAL(triggered(QAction*)),this,SLOT(cssTtriggered(QAction*)));
    connect(m_syncSwitchAct,SIGNAL(toggled(bool)),this,SLOT(toggledSyncSwitch(bool)));
    connect(m_syncScrollAct,SIGNAL(toggled(bool)),this,SLOT(toggledSyncScroll(bool)));
    connect(m_reloadAct,SIGNAL(triggered()),this,SLOT(reload()));

    m_syncScrollAct->setChecked(m_liteApp->settings()->value("markdown/syncscroll",true).toBool());
    m_syncSwitchAct->setChecked(m_liteApp->settings()->value("markdown/syncswitch",true).toBool());
}

HtmlPreview::~HtmlPreview()
{
    QAction *act = m_cssActGroup->checkedAction();
    if (act != 0) {
        m_liteApp->settings()->setValue("markdown/css",act->text());
    }
    m_liteApp->settings()->setValue("markdown/syncscroll",m_syncScrollAct->isChecked());
    m_liteApp->settings()->setValue("markdown/syncswitch",m_syncSwitchAct->isChecked());
    delete m_configMenu;
    delete m_cssMenu;
}

static QByteArray defcss_data =
"@media print {pre,code {word-wrap: break-word;}";

static QByteArray export_data =
"<html>"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>"
"<style type=\"text/css\">"
"__MARKDOWN_CSS__"
"</style>"
"<title>__MARKDOWN_TITLE__</title>"
"</head>"
"<body>"
"__MARKDOWN_CONTENT__"
"</body>"
"</html>";


void HtmlPreview::appLoaded()
{
    m_htmlWidget = m_liteApp->htmlWidgetManager()->create(this);
    if (m_htmlWidget->className() == "QWebView") {
        m_bWebkit = true;
    } else {
        m_bWebkit = false;
    }

    //connect(m_htmlWidget,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
    connect(m_htmlWidget,SIGNAL(contentsSizeChanged()),this,SLOT(htmlContentsSizeChanged()));
    connect(m_htmlWidget,SIGNAL(linkClicked(QUrl)),this,SLOT(linkClicked(QUrl)));
    connect(m_htmlWidget,SIGNAL(linkHovered(QUrl)),this,SLOT(linkHovered(QUrl)));

    QAction *nocssAct = new QAction(tr("Plain HTML"),this);
    nocssAct->setCheckable(true);
    nocssAct->setObjectName("nocss");
    m_cssActGroup->addAction(nocssAct);
    QAction *sep = new QAction(this);
    sep->setSeparator(true);
    m_cssActGroup->addAction(sep);

    QFile file(m_liteApp->resourcePath()+"/markdown/export.html");
    if (file.open(QFile::ReadOnly)) {
        m_exportOrgTemple = file.readAll();
    } else {
        m_exportOrgTemple = export_data;
    }

    m_exportTemple = m_exportOrgTemple;
    m_exportTemple.replace("__MARKDOWN_CSS__",defcss_data);

    QString defcss;

    if (m_bWebkit) {
        QDir dir(m_liteApp->resourcePath()+"/markdown/css");
        foreach (QFileInfo info, dir.entryInfoList(QStringList()<<"*.css",QDir::Files)) {
            QAction *act = new QAction(info.fileName(),this);
            act->setCheckable(true);
            m_cssActGroup->addAction(act);
        }
        defcss = "github.css";
    } else {
        QAction *act = new QAction("textbrowser.css",this);
        act->setCheckable(true);
        m_cssActGroup->addAction(act);
        fix_qt_textbrowser(true);
        defcss = "textbrowser.css";
    }

    m_cssMenu->addActions(m_cssActGroup->actions());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_htmlWidget->widget(),1);
    m_widget->setLayout(layout);

    QString css = m_liteApp->settings()->value("markdown/css",defcss).toString();
    foreach (QAction *act, m_cssActGroup->actions()) {
        if (act->text() == css) {
            act->trigger();
            break;
        }
    }
}

QByteArray HtmlPreview::loadCssData(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        return f.readAll();
    }
    return QByteArray();
}

void HtmlPreview::currentEditorChanged(LiteApi::IEditor *editor)
{       
    if (m_curEditor != 0) {
        m_curEditor->disconnect(this);
    }    
    if (m_curTextEditor != 0) {
        m_curTextEditor->verticalScrollBar()->disconnect(this);
    }

    if (editor &&
        ( (editor->mimeType() == "text/x-markdown") ||
            (editor->mimeType() == "text/html")) )  {
        if (m_syncSwitchAct->isChecked()) {
            m_toolAct->setChecked(true);
        }
        QPlainTextEdit *textEdit = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
        if (textEdit) {
            m_curTextEditor = textEdit;
            connect(m_curTextEditor->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(scrollValueChanged()));
        }
        LiteApi::ITextEditor *ed = LiteApi::getTextEditor(editor);
        if (ed) {
            m_curEditor = ed;
            connect(m_curEditor,SIGNAL(contentsChanged()),this,SLOT(contentsChanged()));
        }
        m_bFileChanged = true;
        editorHtmlPrivew(true);
    } else {
        if (m_syncSwitchAct->isChecked()) {
            m_toolAct->setChecked(false);
        }
        m_curEditor = 0;
        m_curTextEditor = 0;
        if (m_htmlWidget && !m_lastData.isEmpty()) {
            m_htmlWidget->clear();
        }
        m_lastData.clear();
    }
}

void HtmlPreview::contentsChanged()
{
    editorHtmlPrivew();
}

void HtmlPreview::scrollValueChanged()
{
    if (m_syncScrollAct->isChecked()) {
        syncScrollValue();
    }
}

void HtmlPreview::syncScrollValue()
{
    if (!m_curTextEditor || !m_htmlWidget || !m_toolAct->isChecked()) {
        return;
    }

    int max0 = m_curTextEditor->verticalScrollBar()->maximum();
    int min0 = m_curTextEditor->verticalScrollBar()->minimum();
    int value0 = m_curTextEditor->verticalScrollBar()->value();
    if ((max0-min0) == 0) {
        return;
    }
    int max1 = m_htmlWidget->scrollBarMaximum(Qt::Vertical);
    int min1 = m_htmlWidget->scrollBarMinimum(Qt::Vertical);
    int value1 = 1.0*value0*(max1-min1)/(max0-min0);
    m_htmlWidget->setScrollBarValue(Qt::Vertical,value1);
}

void HtmlPreview::toggledSyncSwitch(bool b)
{
    if (b) {
        //this->editorHtmlPrivew(true);
    }
}

void HtmlPreview::toggledSyncScroll(bool b)
{
    if (b) {
        this->syncScrollValue();
    }
}

void HtmlPreview::editorHtmlPrivew(bool force)
{
    if (!m_curEditor || !m_htmlWidget || !m_toolAct->isChecked()) {
        return;
    }

    QByteArray data = m_curEditor->utf8Data();
    if (!force && (m_lastData == data)) {
        return;
    }    
    loadHtmlData(data,QFileInfo(m_curEditor->filePath()).fileName().toUtf8(),m_curEditor->mimeType(),QUrl::fromLocalFile(m_curEditor->filePath()));
}

void HtmlPreview::loadHtmlData(const QByteArray &data, const QByteArray &title, const QString &mime, const QUrl &url)
{
    m_lastData = data;

//    int h = m_htmlWidget->scrollBarValue(Qt::Horizontal);
//    int v = m_htmlWidget->scrollBarValue(Qt::Vertical);
//    m_prevPos = QPoint(h,v);
    if (mime == "text/html") {
        QTextCodec *codec = QTextCodec::codecForHtml(data,QTextCodec::codecForName("utf-8"));
        m_htmlWidget->setHtml(codec->toUnicode(data),QUrl::fromLocalFile(m_curEditor->filePath()));
    } else if (mime == "text/x-markdown") {
        m_exportHtml = m_exportTemple;
        m_exportHtml.replace("__MARKDOWN_TITLE__",title);
        m_exportHtml.replace("__MARKDOWN_CONTENT__",md2html(data));
        m_htmlWidget->setHtml(QString::fromUtf8(m_exportHtml),url);
    } else {
        m_exportHtml = m_exportTemple;
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        m_exportHtml.replace("__MARKDOWN_TITLE__",title);
#if QT_VERSION >= 0x050000
        m_exportHtml.replace("__MARKDOWN_CONTENT__","<pre>"+codec->toUnicode(data).toHtmlEscaped().toUtf8()+"</pre>");
#else
        m_exportHtml.replace("__MARKDOWN_CONTENT__","<pre>"+Qt::escape(codec->toUnicode(data)).toUtf8()+"</pre>");
#endif
        m_htmlWidget->setHtml(QString::fromUtf8(m_exportHtml),url);
    }
}

void HtmlPreview::triggeredTool(bool b)
{
    if (b) {
        currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    }
}

void HtmlPreview::reload()
{
    this->editorHtmlPrivew(true);
    this->syncScrollValue();
}

void HtmlPreview::exportHtml()
{
    if (m_curEditor == 0) {
        return;
    }
    QFileInfo info(m_curEditor->filePath());
    QString path = info.path()+"//"+info.completeBaseName();

    QString fileName = QFileDialog::getSaveFileName(m_widget, tr("Export Html"),
                                                    path, "*.html");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".html");
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QMessageBox::critical(m_widget,
                                  tr("Export Failed"),
                                  QString(tr("Could not open %1 for writing!")).arg(fileName)
                                  );
            return;
        }
        if (m_curEditor->mimeType() == "text/html") {
            file.write(m_lastData);
        } else {
            file.write(m_exportHtml);
        }
        file.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(fileName).path()));
    }
}

void HtmlPreview::exportPdf()
{
    if (m_curEditor == 0) {
        return;
    }
#ifndef QT_NO_PRINTER
    QFileInfo info(m_curEditor->filePath());
    QString path = info.path()+"//"+info.completeBaseName();

    QString fileName = QFileDialog::getSaveFileName(m_widget, tr("Export PDF"),
                                                    path, "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setCreator("LiteIDE");
        printer.setOutputFileName(fileName);
        m_htmlWidget->print(&printer);
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(fileName).path()));
    }
#endif
}

void HtmlPreview::printPreview()
{
    if (m_curEditor == 0) {
        return;
    }
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageMargins(10,10,10,10,QPrinter::Millimeter);
    printer.setPageSize(QPrinter::A4);
    QPrintPreviewDialog dlg(&printer,m_widget);
    connect(&dlg,SIGNAL(paintRequested(QPrinter*)),m_htmlWidget,SLOT(print(QPrinter*)));
    dlg.exec();
#endif
}

void HtmlPreview::cssTtriggered(QAction *act)
{
    if (!act) {
        return;
    }
    QByteArray cssData;
    if (act->objectName() != "nocss") {
        QString fileName;
        if (!m_bWebkit) {
            fileName = ":/markdown/css/"+act->text();
        } else {
            fileName = m_liteApp->resourcePath()+"/markdown/css/"+act->text();
        }
        cssData = this->loadCssData(fileName);
    }
    if (cssData.isEmpty()) {
        cssData = "@media print {pre,code {word-wrap: break-word;}";
    }
    m_exportTemple = m_exportOrgTemple;
    m_exportTemple.replace("__MARKDOWN_CSS__",cssData);

    this->editorHtmlPrivew(true);
}

void HtmlPreview::linkClicked(const QUrl &url)
{
    m_liteApp->mainWindow()->statusBar()->clearMessage();

    if (url.scheme() == "file") {
        QFileInfo info(url.toLocalFile());
        QFile f(info.filePath());
        if (!f.open(QFile::ReadOnly)) {
            return;
        }
        QByteArray data = f.readAll();
        QString mime = m_liteApp->mimeTypeManager()->findMimeTypeByFile(info.filePath());
        loadHtmlData(data,info.fileName().toUtf8(),mime,url);
    } else {
        QDesktopServices::openUrl(url);
    }
}

void HtmlPreview::linkHovered(const QUrl &url)
{
    m_liteApp->mainWindow()->statusBar()->showMessage(url.toString());
}

void HtmlPreview::loadFinished(bool b)
{
    if (!b) {
        return;
    }
    if (m_bFileChanged) {
        this->syncScrollValue();
        this->m_bFileChanged = false;
    } else {
        m_htmlWidget->setScrollBarValue(Qt::Horizontal,m_prevPos.x());
        m_htmlWidget->setScrollBarValue(Qt::Vertical,m_prevPos.y());
    }
}

void HtmlPreview::htmlContentsSizeChanged()
{
    this->syncScrollValue();
}
