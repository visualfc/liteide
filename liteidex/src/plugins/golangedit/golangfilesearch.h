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
// Module: golangfilesearch.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGFILESEARCH_H
#define GOLANGFILESEARCH_H

#include "liteenvapi/liteenvapi.h"
#include "litefindapi/litefindapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "processex/processex.h"

class GolangFileSearch : public LiteApi::IFileSearch
{
    Q_OBJECT
public:
    explicit GolangFileSearch(LiteApi::IApplication *app, QObject *parent = 0);
    virtual QString mimeType() const;
    virtual QString displayName() const;
    virtual QWidget* widget() const;
    virtual void start();
    virtual void cancel();
    virtual void activate();
    virtual QString searchText() const;
    virtual bool replaceMode() const;
    void findUsages(LiteApi::ITextEditor *editor, QTextCursor cursor, bool replace = false);
public slots:
    void findUsagesStarted();
    void findUsagesOutput(QByteArray,bool);
    void findUsagesFinish(bool,int,QString);
protected:
    LiteApi::IApplication *m_liteApp;
    ProcessEx *m_process;
    QFile   m_file;
    QString m_searchText;
    QString m_lastLineText;
    int     m_lastLine;
    bool    m_replaceMode;
};

#endif // GOLANGFILESEARCH_H
