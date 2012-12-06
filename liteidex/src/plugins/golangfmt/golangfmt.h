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
// Module: golangfmt.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: golangfmt.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef GOLANGFMT_H
#define GOLANGFMT_H

#include <QObject>
#include <QTextCursor>

#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"

class ProcessEx;
class GolangFmt : public QObject
{
    Q_OBJECT
public:
    explicit GolangFmt(LiteApi::IApplication *app,QObject *parent = 0);
public slots:
    void fmtStarted();
    void gofmt();
    void currentEnvChanged(LiteApi::IEnv*);
    void fmtOutput(QByteArray,bool);
    void fmtFinish(bool,int,QString);
    void loadDiff(QTextCursor &cursor, const QString &diff);
    void editorAboutToSave(LiteApi::IEditor*);
    void applyOption(QString);
    void fmtEditor(LiteApi::IEditor*,bool);
    void syncfmtEditor(LiteApi::IEditor *, bool save, bool check = true);
protected:
    LiteApi::IApplication *m_liteApp;
    ProcessEx *m_process;
    LiteApi::IEnvManager *m_envManager;
    QString m_gofmtCmd;
    QByteArray m_data;
    QByteArray m_errData;
    bool m_diff;
    bool m_autofmt;
    bool m_autopop;
    int  m_timeout;
};

#endif // GOLANGFMT_H
