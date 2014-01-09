/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: golanglint.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGLINT_H
#define GOLANGLINT_H

#include <QObject>
#include <QTextCursor>

#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"

#define GOLINT_TAG "golint"

class ProcessEx;
class GolangLint : public QObject
{
    Q_OBJECT
public:
    explicit GolangLint(LiteApi::IApplication *app,QObject *parent = 0);
public slots:
    void started();
    void golint();
    void currentEnvChanged(LiteApi::IEnv*);
    void lintOutput(QByteArray,bool);
    void lintFinish(bool,int,QString);
    void loadDiff(QTextCursor &cursor, const QString &diff);
    void editorAboutToSave(LiteApi::IEditor*);
    void applyOption(QString);
    void lintEditor(LiteApi::IEditor*,bool);
    void syncLintEditor(LiteApi::IEditor *, bool save, bool check = true, int timeout = -1);
protected:
    LiteApi::IApplication *m_liteApp;
    ProcessEx *m_process;
    LiteApi::IEnvManager *m_envManager;
    QString m_golintCmd;
    QByteArray m_data;
    QByteArray m_errData;
    bool m_goimports;
    bool m_diff;
    bool m_autolint;
    bool m_synclint;
    int  m_timeout;
};

#endif // GOLANGLINT_H
