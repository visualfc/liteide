/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: filesearchmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILESEARCHMANAGER_H
#define FILESEARCHMANAGER_H

#include "liteapi/liteapi.h"
#include "litefindapi/litefindapi.h"
#include "filesearch.h"
#include "qtc_searchresult/searchresultwidget.h"

class QStackedWidget;

class FileSearchManager : public LiteApi::IFileSearchManager
{
    Q_OBJECT
public:
    explicit FileSearchManager(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~FileSearchManager();
    virtual void addFileSearch(LiteApi::IFileSearch* search);
    virtual LiteApi::IFileSearch* findFileSearch(const QString &mime);
    virtual QList<LiteApi::IFileSearch*> fileSearchList() const;
    virtual void setCurrentSearch(LiteApi::IFileSearch *search);
public slots:
    virtual void newSearch();
    void activated(const Find::SearchResultItem &item);
    void currentSearchItemChanged(int);
    void findStarted();
    void findFinished(bool);
    void findError(const QString &error);
    void findResult(const LiteApi::FileSearchResult &result);
    void doReplace(const QString &text, const QList<Find::SearchResultItem> &items, bool preserveCase);
    void searchTextChanged(const QString &text);
protected:
    LiteApi::IApplication   *m_liteApp;
    QStackedWidget *m_widget;
    QAction        *m_toolAct;
    QWidget        *m_searchWidget;
    QComboBox      *m_searchItemCombox;
    QStackedWidget *m_searchItemStackedWidget;
    Find::Internal::SearchResultWidget *m_searchResultWidget;
    QList<LiteApi::IFileSearch*> m_fileSearchList;
    LiteApi::IFileSearch    *m_currentSearch;
};

#endif // FILESEARCHMANAGER_H
