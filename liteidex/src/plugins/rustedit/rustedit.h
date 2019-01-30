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
// Module: rustedit.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef RUSTEDIT_H
#define RUSTEDIT_H

#include <liteapi/liteapi.h>
#include <liteeditorapi/liteeditorapi.h>
#include <liteenvapi/liteenvapi.h>

class RustEdit : public QObject
{
    Q_OBJECT
public:
    explicit RustEdit(LiteApi::IApplication *app, QObject *parent = 0);
public slots:
    void currentEnvChanged(LiteApi::IEnv *env);
    void editorCreated(LiteApi::IEditor *editor);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void prefixChanged(const QTextCursor &cur,const QString &pre,bool force);
    void wordCompleted(QString,QString,QString);
    void finished(int code,QProcess::ExitStatus);
protected:
    void setCompleter(LiteApi::ICompleter *completer);
    LiteApi::IApplication *m_liteApp;
    LiteApi::ICompleter   *m_completer;
    LiteApi::ILiteEditor  *m_editor;
    QProcess    *m_process;
    QString     m_racerCmd;
    QString     m_preWord;
    QString     m_prefix;
    QString     m_lastPrefix;
};

#endif // RUSTEDIT_H
