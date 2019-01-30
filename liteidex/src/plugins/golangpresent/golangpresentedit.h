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
// Module: golangpresenteedit.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGPRESENTEDIT_H
#define GOLANGPRESENTEDIT_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "processex/processex.h"

#define GOPRESENT_TAG "gopresent/navtag"

class GolangPresentEdit : public QObject
{
    Q_OBJECT
public:
    enum EXPORT_TYPE {
        EXPORT_TYPE_VERIFY,
        EXPORT_TYPE_HTML,
        EXPORT_TYPE_PDF
    };

public:
    explicit GolangPresentEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent = 0);
public slots:
    void editorSaved(LiteApi::IEditor*);
    void s1();
    void s2();
    void s3();
    void bold();
    void italic();
    void code();
    void bullets();
    void comment();
    void verify();
    void exportHtml();
    void exportPdf();
    void extOutput(const QByteArray &data,bool bError);
    void extFinish(bool error,int code, QString msg);
    void loadHtmlFinished(bool);
protected:
    bool startExportHtmlDoc(EXPORT_TYPE type);
    LiteApi::IApplication *m_liteApp;
    LiteApi::ILiteEditor  *m_editor;
    LiteApi::IHtmlDocument *m_htmldoc;
    QPlainTextEdit        *m_ed;
    ProcessEx             *m_process;
    QByteArray             m_exportData;
    QStringList            m_errorMsg;
    QString                m_pdfFileName;
    QString                m_exportName;
};

#endif // GOLANGPRESENTEDIT_H
