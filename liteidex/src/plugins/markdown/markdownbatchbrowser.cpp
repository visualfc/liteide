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
#include <QStringListModel>
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

MarkdownBatchBrowser::MarkdownBatchBrowser(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IBrowserEditor(parent),
    m_liteApp(app),
    ui(new Ui::MarkdownBatchWidget)
{
    m_widget = new QWidget;
    m_doc = 0;
    m_mode = 0;
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels(QStringList()<< "Name" << "Path");
    ui->setupUi(m_widget);
    ui->filesTreeView->setModel(m_model);
    ui->filesTreeView->setEditTriggers(0);
    ui->filesTreeView->setDragDropMode(QAbstractItemView::InternalMove);
    connect(ui->importFolderPushButton,SIGNAL(clicked()),this,SLOT(browserImportFolder()));
    connect(ui->addFilesPushButton,SIGNAL(clicked()),this,SLOT(addFiles()));
    connect(ui->removePushButton,SIGNAL(clicked()),this,SLOT(remove()));
    connect(ui->removeAllPushButton,SIGNAL(clicked()),this,SLOT(removeAll()));
    connect(ui->moveDownPushButton,SIGNAL(clicked()),this,SLOT(moveDown()));
    connect(ui->moveUpPushButton,SIGNAL(clicked()),this,SLOT(moveUp()));
    connect(ui->useCssCheckBox,SIGNAL(toggled(bool)),ui->cssComboBox,SLOT(setEnabled(bool)));
    connect(ui->browserExportFolderPushButton,SIGNAL(clicked()),this,SLOT(browserExportFolder()));
    connect(ui->splitHtmlPushButton,SIGNAL(clicked()),this,SLOT(splitHtml()));
    QDir dir(m_liteApp->resourcePath()+"/markdown/css");
    QStringList csss;
    foreach (QFileInfo info, dir.entryInfoList(QStringList()<<"*.css",QDir::Files)) {
        csss.append(info.fileName());
    }
    if (!csss.isEmpty()) {
        ui->cssComboBox->addItems(csss);
        int index = m_liteApp->settings()->value("markdown/batch_css",0).toInt();
        if (index >= 0 && index <= csss.size()-1) {
            ui->cssComboBox->setCurrentIndex(index);
        } else {
            ui->cssComboBox->setCurrentIndex(0);
        }
        bool useCss = m_liteApp->settings()->value("markdown/batch_usecss",true).toBool();
        ui->useCssCheckBox->setChecked(useCss);
    } else {
        ui->useCssCheckBox->setChecked(false);
        ui->useCssCheckBox->setEnabled(false);
    }
    ui->cssComboBox->setEnabled(ui->useCssCheckBox->isChecked());
    ui->exportFolderLineEdit->setText(m_liteApp->settings()->value("markdown/batch_oupath").toString());
}

MarkdownBatchBrowser::~MarkdownBatchBrowser()
{
    m_liteApp->settings()->setValue("markdown/batch_usecss",ui->useCssCheckBox->isChecked());
    m_liteApp->settings()->setValue("markdown/batch_css",ui->cssComboBox->currentIndex());
    m_liteApp->settings()->setValue("markdown/batch_oupath",ui->exportFolderLineEdit->text());
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

QString MarkdownBatchBrowser::markdownOpenFilter() const
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

QStringList MarkdownBatchBrowser::markdonwFilter() const
{
    LiteApi::IMimeType *mimeType = m_liteApp->mimeTypeManager()->findMimeType("text/x-markdown");
    if (mimeType) {
        return mimeType->globPatterns();
    }
    return QStringList() << "*.md";
}

void MarkdownBatchBrowser::addFile(const QString &file)
{
    QFileInfo info(file);
    QStandardItem *item = new QStandardItem(info.fileName());
    item->setData(info.filePath());

    m_model->appendRow(QList<QStandardItem*>()
                       << item
                       << new QStandardItem(info.filePath())
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
            QByteArray data = mdtohtml(f.readAll());
            datas.append(data);
            htmls.append(codec->toUnicode(data));
            htmls.append("\n<div STYLE=\"page-break-after: always;\"></div>\n");
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

void MarkdownBatchBrowser::browserImportFolder()
{
    QString folder = QFileDialog::getExistingDirectory(m_widget,tr("Select Markdown Folder"));
    if (!folder.isEmpty()) {
        QDir dir(folder);
        foreach(QFileInfo info, dir.entryInfoList(markdonwFilter(),QDir::Files,QDir::Name)) {
            addFile(info.filePath());
        }
    }
}

void MarkdownBatchBrowser::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(m_widget,tr("Select Markdown Files"),QString(),this->markdownOpenFilter());
    foreach(QString file, files) {
        this->addFile(file);
    }
}

void MarkdownBatchBrowser::remove()
{
    QModelIndex index = ui->filesTreeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    m_model->removeRow(index.row());
}

void MarkdownBatchBrowser::removeAll()
{
    int size = m_model->rowCount();
    if (size == 0) {
        return;
    }
    m_model->removeRows(0,size);
}

void MarkdownBatchBrowser::moveUp()
{
    QModelIndex index = ui->filesTreeView->currentIndex();
    if (!index.isValid() || index.row() == 0) {
        return;
    }
    int row = index.row();
    m_model->insertRow(row-1,m_model->takeRow(row));
    ui->filesTreeView->setCurrentIndex(m_model->index(row-1,0));
}

void MarkdownBatchBrowser::moveDown()
{
    QModelIndex index = ui->filesTreeView->currentIndex();
    if (!index.isValid() || index.row() >= m_model->rowCount()-1) {
        return;
    }
    int row = index.row();
    m_model->insertRow(row+1,m_model->takeRow(row));
    ui->filesTreeView->setCurrentIndex(m_model->index(row+1,0));
}

void MarkdownBatchBrowser::browserExportFolder()
{
    QString folder = QFileDialog::getExistingDirectory(m_widget,tr("Select Export Folder"));
    if (!folder.isEmpty()) {
        ui->exportFolderLineEdit->setText(folder);
    }
}

void MarkdownBatchBrowser::splitHtml()
{
    this->init();
    if (m_fileList.isEmpty()) {
        return;
    }
    this->appendLog("\nSplit html ...\n");
    if (m_exportPath.isEmpty()) {
        this->appendLog("output path empty!");
        return;
    }
    foreach (QString file, m_fileList) {
        QFileInfo info(file);
        QString out = m_exportPath+"/"+info.completeBaseName()+".html";
        QFile f(out);
        if (f.open(QFile::WriteOnly|QFile::Truncate)) {
            this->appendLog(file+" => "+out+" ...");
            QByteArray exportData = m_exportTemple;
            exportData.replace("__MARKDOWN_TITLE__",info.fileName().toUtf8());
            exportData.replace("__MARKDOWN_CONTENT__",m_fileHtmlMap.value(file));
            f.write(exportData);
        }
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_exportPath));
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

void MarkdownBatchBrowser::init()
{
    if (m_doc == 0) {
        m_doc = m_liteApp->htmlWidgetManager()->createDocument(this);
        connect(m_doc,SIGNAL(loadFinished(bool)),this,SLOT(loadFinished(bool)));
        QFile file(m_liteApp->resourcePath()+"/markdown/export.html");
        if (file.open(QFile::ReadOnly)) {
            m_exportOrgTemple = file.readAll();
        } else {
            m_exportOrgTemple = export_data;
        }
    }
    m_exportTemple = m_exportOrgTemple;
    QByteArray css;
    if (ui->useCssCheckBox->isChecked()) {
        QString fileName = ui->cssComboBox->currentText();
        QFile f(m_liteApp->resourcePath()+"/markdown/css/"+fileName);
        if (f.open(QFile::ReadOnly)) {
            css = f.readAll();
        }
    }
    if (css.isEmpty()) {
        css = defcss_data;
    }
    m_exportTemple.replace("__MARKDOWN_CSS__",css);

    m_fileList = getFiles();
    m_fileHtmlMap = getFileHtmlDataMap(m_fileList);

    m_exportPath.clear();

    QString path = ui->exportFolderLineEdit->text();
    if (!path.isEmpty()) {
        QDir dir(path);
        if (!dir.exists()) {
            dir.mkpath(path);
        }
        if (dir.exists()) {
            m_exportPath = dir.path();
        }
   }
}

QStringList MarkdownBatchBrowser::getFiles() const
{
    QStringList files;
    for(int i = 0; i < m_model->rowCount(); i++) {
        QModelIndex index = m_model->index(i,0);
        if (index.isValid()) {
            files.append(index.data(Qt::UserRole+1).toString());
        }
    }
    return files;
}

QMap<QString, QByteArray> MarkdownBatchBrowser::getFileHtmlDataMap(const QStringList &files) const
{
    QMap<QString, QByteArray> map;
    foreach(QString file, files) {
        QFile f(file);
        if (f.open(QFile::ReadOnly)) {
            map.insert(file,mdtohtml(f.readAll()));
        }
    }
    return map;
}
