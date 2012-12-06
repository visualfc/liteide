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
// Module: golangdoc.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: golangdoc.h,v 1.0 2011-7-7 visualfc Exp $

#ifndef GOLANGDOC_H
#define GOLANGDOC_H

#include "liteapi/liteapi.h"
#include "litebuildapi/litebuildapi.h"
#include "liteenvapi/liteenvapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "golangdocapi/golangdocapi.h"
#include "qtc_editutil/filterlineedit.h"

#include <QUrl>
#include <QModelIndex>
#include <QListView>

class QLabel;
class QListView;
class QLineEdit;
class QStringListModel;
class QComboBox;
class QPushButton;
class ProcessEx;
class DocumentBrowser;
class QSortFilterProxyModel;
class GolangApi;
class GolangApiThread;

class ListViewEx : public QListView
{
Q_OBJECT
public:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
signals:
    void currentIndexChanged(QModelIndex);
};

class GolangDoc : public LiteApi::IGolangDoc
{
    Q_OBJECT
public:
    explicit GolangDoc(LiteApi::IApplication *app, QObject *parent = 0);
    ~GolangDoc();    
public slots:
    virtual void openUrl(const QUrl &url);
    virtual void activeBrowser();
public slots:
    void appLoaded();
    void triggeredToolWindow(bool);
    void saveGolangApi();
    void editorFindDoc();
    void editorJumpToDecl();
    void editorCreated(LiteApi::IEditor *editor);
    void loadApi();
    void loadEnv();
    void loadApiFinished();
    void currentEnvChanged(LiteApi::IEnv*);
    void listCmd();
    void listPkg();
    void findPackage(QString name = QString());
    void findOutput(QByteArray,bool);
    void findFinish(bool,int,QString);
    void godocFindPackage(QString name);
    void godocOutput(QByteArray,bool);
    void godocFinish(bool,int,QString);
    void goapiOutput(QByteArray,bool);
    void goapiFinish(bool,int,QString);
    void lookupStarted();
    void lookupOutput(QByteArray,bool);
    void lookupFinish(bool,int,QString);
    void helpStarted();
    void helpOutput(QByteArray,bool);
    void helpFinish(bool,int,QString);
    void doubleClickListView(QModelIndex);
    void currentIndexChanged(QModelIndex);
    void findTag(const QString &tag);
    void highlighted(const QUrl &url);
    void documentLoaded();
    void filterTextChanged(QString);
protected:
    QUrl parserUrl(const QUrl &url);
    void openUrlList(const QUrl &url);
    void openUrlFind(const QUrl &url);
    void openUrlPdoc(const QUrl &url);
    void openUrlFile(const QUrl &url);
    void updateTextDoc(const QUrl &url, const QByteArray &ba, const QString &header);
    void updateHtmlDoc(const QUrl &url, const QByteArray &ba, const QString &header = QString(), bool toNav = true);
protected:
    LiteApi::IApplication   *m_liteApp;
    QUrl    m_openUrl;
    QUrl    m_lastUrl;
    QString m_lastPath;
    QWidget *m_widget;
    DocumentBrowser *m_docBrowser;
    QComboBox *m_godocFindComboBox;
    QStringListModel *m_findResultModel;
    QSortFilterProxyModel *m_findFilterModel;
    ListViewEx *m_findResultListView;
    Utils::FilterLineEdit *m_findEdit;
    QLabel     *m_tagInfo;
    ProcessEx  *m_findProcess;
    ProcessEx  *m_godocProcess;
    ProcessEx  *m_goapiProcess;
    ProcessEx  *m_lookupProcess;
    ProcessEx  *m_helpProcess;
    QAction *m_browserAct;
    QString m_goroot;
    QByteArray  m_godocData;
    QByteArray  m_goapiData;
    QByteArray  m_lookupData;
    QByteArray  m_srcData;
    QTextCursor m_lastCursor;
    LiteApi::IEditor *m_lastEditor;
    QByteArray  m_helpData;
    QByteArray  m_findData;
    QString  m_templateData;
    LiteApi::IEnvManager *m_envManager;
    GolangApiThread *m_golangApiThread;
    QString m_godocCmd;
    QString m_findCmd;
    QString m_goapiCmd;
    bool    m_bApiLoaded;
    QStringList m_targetList;
    QMap<QString,QString> m_pathFileMap;
    QAction *m_toolWindowAct;
    QAction *m_findDocAct;
    QAction *m_jumpDeclAct;
    QString m_docFind;
};

#endif // GOLANGDOC_H
