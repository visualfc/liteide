/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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

inline QByteArray trimmedRight(const QByteArray &d)
{
    if (d.size() == 0) {
        return d;
    }
    const char *s = d.data();
    int end = d.size() - 1;
    while (end && isspace(uchar(s[end])))           // skip white space from end
        end--;
    return d.left(end+1);
}

static bool isLetterOrNumber(QChar ch)
{
    if (ch.isLetterOrNumber() || ch == '_') {
        return true;
    }
    return false;
}

inline QString selectionUnderCursor(QTextCursor tc, bool moveLeft = false)
{
    QString text = tc.block().text();
    if (tc.hasSelection()) {
        moveLeft = false;
    }
    int pos = tc.selectionStart() - tc.block().position();
    if (moveLeft) {
        pos--;
    }
    int left = pos;
    for (int i = pos; i >= 0; i--) {
        if (!isLetterOrNumber(text[i]) && text[i] != '.') {
            left = i;
            break;
        }
    }
    int right = text.length();
    for (int i = pos; i < text.length(); i++) {
        if (!isLetterOrNumber(text[i]))  {
            right = i;
            break;
        }
    }
    return text.mid(left+1,right-left-1);
}

class GolangFileSearch : public LiteApi::IFileSearch
{
    Q_OBJECT
public:
    explicit GolangFileSearch(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~GolangFileSearch();
    virtual QString mimeType() const;
    virtual QString displayName() const;
    virtual QWidget* widget() const;
    virtual void start();
    virtual void cancel();
    virtual void activate();
    virtual QString searchText() const;
    virtual bool replaceMode() const;
    virtual bool readOnly() const { return false; }
    virtual bool canCancel() const { return false; }
    virtual void setSearchInfo(const QString &text, const QString &fitler, const QString &path);
    void findUsages(LiteApi::ITextEditor *editor, QTextCursor cursor, bool global, bool skip_goroot, bool skip_tests, bool replace);
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
    bool    m_bParserHead;
    bool    m_replaceMode;
};

#endif // GOLANGFILESEARCH_H
