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
// Module: golangdoc.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGDOC_H
#define GOLANGDOC_H

#include "liteapi/liteapi.h"
#include "litebuildapi/litebuildapi.h"
#include "liteenvapi/liteenvapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "golangdocapi/golangdocapi.h"
#include "qtc_editutil/fancylineedit.h"

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
class FindDocWidget;
class FindApiWidget;

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
    void rebuildApiData();
    void appLoaded();
    void triggeredToolWindow(bool);    
    void editorFindDoc();
    void editorJumpToDecl();
    void editorCreated(LiteApi::IEditor *editor);
    void loadEnv();
    void currentEnvChanged(LiteApi::IEnv*);
    void listCmd();
    void listPkg();
    void findOutput(QByteArray,bool);
    void findFinish(bool,int,QString);
    void godocFindPackage(QString name);
    void godocOutput(QByteArray,bool);
    void godocFinish(bool,int,QString);
    void lookupStarted();
    void lookupOutput(QByteArray,bool);
    void lookupFinish(bool,int,QString);
    void helpStarted();
    void helpOutput(QByteArray,bool);
    void helpFinish(bool,int,QString);
    void highlighted(const QUrl &url);
    void documentLoaded();
    void anchorChanged(const QString &anchor);
    void openApiUrl(QStringList);
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
    FindDocWidget *m_findDocWidget;
    FindApiWidget *m_findApiWidget;
    DocumentBrowser *m_docBrowser;
    QComboBox *m_godocFindComboBox;
    ProcessEx  *m_findProcess;
    ProcessEx  *m_godocProcess;
    ProcessEx  *m_lookupProcess;
    ProcessEx  *m_helpProcess;
    QAction *m_browserAct;
    QString m_goroot;
    QByteArray  m_godocData;
    QByteArray  m_lookupData;
    QByteArray  m_srcData;
    QTextCursor m_lastCursor;
    LiteApi::IEditor *m_lastEditor;
    QByteArray  m_helpData;
    QByteArray  m_findData;
    QString  m_templateData;
    LiteApi::IEnvManager *m_envManager;
    QString m_godocCmd;
    QStringList m_targetList;
    QMap<QString,QString> m_pathFileMap;
    QAction *m_toolWindowAct;
    QAction *m_findDocAct;
    QAction *m_jumpDeclAct;
    QMenu   *m_configMenu;
    QString m_docFind;
};

#endif // GOLANGDOC_H
