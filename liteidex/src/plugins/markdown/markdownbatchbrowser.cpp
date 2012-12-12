/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: markdownbatchbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "markdownbatchbrowser.h"
#include "ui_markdownbatchwidget.h"
#include "sundown/mdtohtml.h"
#include <QFileDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileInfo>
#include <QTextCodec>
#include <QUrl>
#ifndef QT_NO_PRINTER
#include <QPrinter>
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

MarkdownBatchBrowser::MarkdownBatchBrowser(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IBrowserEditor(parent),
    m_liteApp(app),
    ui(new Ui::MarkdownBatchWidget)
{
    m_widget = new QWidget;
    m_doc = 0;
    m_mode = 0;
    m_model = new QStandardItemModel(this);
    ui->setupUi(m_widget);
    ui->filesTreeView->setHeaderHidden(true);
    ui->filesTreeView->setModel(m_model);
    connect(ui->addFilesPushButton,SIGNAL(clicked()),this,SLOT(on_addFilesPushButton_clicked()));
    connect(ui->removePushButton,SIGNAL(clicked()),this,SLOT(on_removePushButton_clicked()));
    connect(ui->mergePdfPushButton,SIGNAL(clicked()),this,SLOT(on_mergePdfPushButton_clicked()));
}

MarkdownBatchBrowser::~MarkdownBatchBrowser()
{
    delete ui;
}

QWidget *MarkdownBatchBrowser::widget()
{
    return m_widget;
}

QString MarkdownBatchBrowser::name() const
{
    return tr("Markdown Batch");
}

QString MarkdownBatchBrowser::mimeType() const
{
    return "browser/markdown";
}

QString MarkdownBatchBrowser::markdownFilter() const
{
    QStringList types;
    QStringList filter;
    LiteApi::IMimeType *mimeType = m_liteApp->mimeTypeManager()->findMimeType("text/x-markdown");
    if (mimeType) {
        types.append(mimeType->globPatterns());
        filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(mimeType->globPatterns().join(" ")));
        types.removeDuplicates();
        filter.removeDuplicates();
    }
    filter.append(tr("All Files (*)"));
    return filter.join(";;");
}

void MarkdownBatchBrowser::addFile(const QString &file)
{
    QFileInfo info(file);
    QStandardItem *item = new QStandardItem(info.fileName());
    item->setData(info.filePath());

    m_model->appendRow(QList<QStandardItem*>()
                       << item
                       );
}

static QByteArray head =
"<html>"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>"
"</head>"
"<body>";

static QByteArray end =
"</body>"
"</html>";


void MarkdownBatchBrowser::mergeToPdf(const QStringList &files)
{
    if (files.isEmpty()) {
        return;
    }
    QString htmls = head;
    QByteArray datas;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");

    foreach(QString file, files) {
        QFile f(file);
        if (f.open(QFile::ReadOnly)) {
            this->appendLog("convert "+file+"...");
            QByteArray data = mdtohtml(f.readAll()+"\n***\n");
            datas.append(data);
            htmls.append(codec->toUnicode(data));
            htmls.append("\n");
        }
    }
    htmls.append(end);

    if (m_doc == 0) {
        m_doc = m_liteApp->htmlWidgetManager()->createDocument(this);
        connect(m_doc,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
    }
    m_mode = MODE_PDF;
    QFileInfo info(m_pdfFileName);
    QFile f(info.filePath()+".html");
    if (f.open(QFile::WriteOnly | QFile::Truncate)) {
        f.write(htmls.toUtf8());
    }
    this->appendLog("loading html ...");
    m_doc->setHtml(htmls,QUrl::fromLocalFile(files.first()));
}

void MarkdownBatchBrowser::appendLog(const QString &log)
{
    ui->logPlainTextEdit->appendPlainText(log);
}

void MarkdownBatchBrowser::loadFinished(bool b)
{
    if (!b) {
        return;
    }
    if (m_mode == MODE_PDF) {
#ifndef QT_NO_PRINTER
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(m_pdfFileName);
        m_doc->print(&printer);
        this->appendLog("print pdf ...");
#endif
    }
}

void MarkdownBatchBrowser::on_addFilesPushButton_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(m_widget,tr("Select Markdown Files"),QString(),this->markdownFilter());
    foreach(QString file, files) {
        this->addFile(file);
    }
}

void MarkdownBatchBrowser::on_removePushButton_clicked()
{

}

void MarkdownBatchBrowser::on_mergePdfPushButton_clicked()
{
    QStringList files;
    for(int i = 0; i < m_model->rowCount(); i++) {
        QModelIndex index = m_model->index(i,0);
        if (index.isValid()) {
            files.append(index.data(Qt::UserRole+1).toString());
        }
    }
    if (files.isEmpty()) {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(m_widget, tr("Export PDF"),
                                                    "merge", "*.pdf");
    if (fileName.isEmpty()) {
        return;
    }

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(".pdf");
    }

    m_pdfFileName = fileName;

    mergeToPdf(files);
}
