/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2024 LiteIDE. All rights reserved.
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
// Module: goplsfilesearch.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOPLSFILESEARCH_H
#define GOPLSFILESEARCH_H

#include "litefindapi/litefindapi.h"
#include "processex/processex.h"

#define GOPLS_references "references"
#define GOPLS_implementation "implementation"

struct gopls_resinfo {
    QString fileName;
    int     line;
    int     column;
    int     columnEnd;
};

class GoplsFileSearch : public LiteApi::IFileSearch
{
    Q_OBJECT
public:
    explicit GoplsFileSearch(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~GoplsFileSearch();
    virtual QString mimeType() const;
    virtual QString displayName() const { return m_displayName; }
    virtual QWidget* widget() const;
    virtual void start();
    virtual void cancel();
    virtual void activate();
    virtual QString searchText() const;
    virtual bool replaceMode() const;
    virtual bool readOnly() const { return m_readOnly; }
    virtual bool canCancel() const { return false; }
    virtual void setSearchInfo(const QString &text, const QString &fitler, const QString &path);
    void setReadOnly(bool b) { m_readOnly = b; };
    void setDisplyName(const QString &name) { m_displayName = name; }
    void findUsages(LiteApi::ITextEditor *editor, QTextCursor cursor, const QString &command, const QStringList &opts = QStringList(), bool replace = false);
public slots:
    void findUsagesStarted();
    void findUsagesOutput(QByteArray,bool);
    void findUsagesFinish(bool,int,QString);
protected:
    LiteApi::IApplication *m_liteApp;
    QList<gopls_resinfo> m_results;
    ProcessEx *m_process;
    QString m_searchText;
    QString m_displayName;
    bool    m_bParserHead;
    bool    m_replaceMode;
    bool    m_readOnly;
};

#endif // GOPLSFILESEARCH_H
