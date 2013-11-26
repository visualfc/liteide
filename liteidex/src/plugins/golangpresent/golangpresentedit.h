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
// Module: golangpresenteedit.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGPRESENTEDIT_H
#define GOLANGPRESENTEDIT_H

#include "liteapi/liteapi.h"
#include "processex/processex.h"

class GolangPresentEdit : public QObject
{
    Q_OBJECT
public:
    explicit GolangPresentEdit(LiteApi::IApplication *app, LiteApi::IEditor *editor, QObject *parent = 0);
public slots:
    void s1();
    void s2();
    void s3();
    void bold();
    void italic();
    void code();
    void bullets();
    void comment();
    void exportHtml();
    void extOutput(const QByteArray &data,bool bError);
    void extFinish(bool error,int code, QString msg);
protected:
    LiteApi::IApplication *m_liteApp;
    LiteApi::ITextEditor  *m_editor;
    QPlainTextEdit        *m_ed;
    ProcessEx             *m_process;
    QByteArray             m_exportData;
};

#endif // GOLANGPRESENTEDIT_H
