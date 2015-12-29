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
// Module: markdownbatchbrowser.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef MARKDOWNBATCHBROWSER_H
#define MARKDOWNBATCHBROWSER_H

#include "liteapi/liteapi.h"
#include <QFileDialog>

namespace Ui {
    class MarkdownBatchWidget;
}

class QStandardItemModel;

class MarkdownBatchBrowser : public LiteApi::IBrowserEditor
{
    Q_OBJECT
public:
    MarkdownBatchBrowser(LiteApi::IApplication *app, QObject *parent);
    virtual ~MarkdownBatchBrowser();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString mimeType() const;
    QString markdownOpenFilter() const;
    QStringList markdonwFilter() const;
    void addFile(const QString &file);
    void appendLog(const QString &log);
public:
    enum {
        MODE_NONE = 0,
        MODE_SPLIT_PDF,
        MODE_MERGE_PDF,
        MODE_MERGE_PRINT,
        MODE_MERGE_PRINTPREVIEW
    };
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    Ui::MarkdownBatchWidget *ui;
    LiteApi::IHtmlDocument *m_doc;
    QStandardItemModel     *m_model;
    int     m_mode;
    QString m_exportPath;
    QString m_pdfFileName;
    QStringList m_fileList;
    QByteArray  m_exportOrgTemple;
    QByteArray  m_exportTemple;
    QMap<QString,QByteArray> m_fileHtmlMap;
private slots:
    void loadFinished(bool);
    void browserImportFolder();
    void addFiles();
    void remove();
    void removeAll();
    void moveUp();
    void moveDown();
    void browserExportFolder();
    void splitHtml();
    void mergeHtml();
    void splitPdf();
    void mergePdf();
    void mergePrint();
    void mergePrintPreview();
protected:
    void init();
    void processPdfList();
    QStringList getFiles() const;
    QMap<QString,QByteArray> getFileHtmlDataMap(const QStringList &files) const;
};

#endif // MARKDOWNBATCHBROWSER_H
