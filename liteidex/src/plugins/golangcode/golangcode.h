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
// Module: golangcode.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGCODE_H
#define GOLANGCODE_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "liteenvapi/liteenvapi.h"
#include "golangastapi/golangastapi.h"
#include "processex/processex.h"

class QProcess;
class QLabel;
class ImportPkgTip : public QObject
{
    Q_OBJECT
public:
    explicit ImportPkgTip(LiteApi::IApplication *app, QObject *parent = 0);
    ~ImportPkgTip();
    void showPkgHint(int startpos, const QStringList &pkg, QPlainTextEdit *ed);
    void setWidget(QWidget *widget);
    void hide();
signals:
    void import(QString,int);
protected:
    bool eventFilter(QObject *obj, QEvent *e);
    LiteApi::IApplication *m_liteApp;
    QWidget *m_editWidget;
    QWidget *m_popup;
    QLabel *m_infoLabel;
    QLabel *m_pkgLabel;
    QStringList m_pkg;
    int     m_startPos;
    int     m_pkgIndex;
    bool m_escapePressed;
    bool m_enterPressed;
};


class GolangCode : public QObject
{
    Q_OBJECT
public:
    explicit GolangCode(LiteApi::IApplication *app, QObject *parent = 0);
    ~GolangCode();
    void setCompleter(LiteApi::ICompleter *completer);
    void gocodeUpdataLibpath(const QProcessEnvironment &env);
    void gocodeReset(const QProcessEnvironment &env);
    void cgoComplete();
    void loadPkgList();
    void loadImportsList(const QProcessEnvironment &env);
public slots:
    void currentEditorChanged(LiteApi::IEditor*);
    void currentEnvChanged(LiteApi::IEnv*);
    void prefixChanged(QTextCursor,QString,bool froce);
    void wordCompleted(QString,QString,QString);
    void started();
    void finished(int,QProcess::ExitStatus);
    void gocodeImportStarted();
    void gocodeImportFinished(int,QProcess::ExitStatus);
    void importFinished(int,QProcess::ExitStatus);
    void broadcast(QString,QString,QString);
    void applyOption(QString);
    void appLoaded();
    void import(const QString &import, int startPos);
    bool findImport(const QString &id);
    void customGOPATHChanged(const QString &buildPath);
    void globalGOPATHChanged();
protected:
    QStringList parserCgoInEditor(int nmax = 1024);
    void updateEditorGOPATH();
    static  int g_gocodeInstCount;
    LiteApi::IApplication *m_liteApp;
    LiteApi::ITextEditor  *m_editor;
    LiteApi::ICompleter   *m_completer;
    QWidget *m_pkgWidget;
    ImportPkgTip    *m_pkgImportTip;
    QMultiMap<QString,QString> m_pkgListMap;
    QStringList m_importList;
    QStringList m_allImportList;
    QMultiMap<QString,QString> m_extraPkgListMap;
    QString     m_gobinCmd;
    QString     m_preWord;
    QString     m_prefix;
    QString     m_lastPrefix;
    QFileInfo   m_fileInfo;
    Process   *m_gocodeProcess;
    Process   *m_gocodeSetProcess;
    Process   *m_gocodeImportProcess;
    Process   *m_importProcess;
    QByteArray  m_writeData;
    LiteApi::IEnvManager *m_envManager;
    LiteApi::IGolangAst *m_golangAst;
    QString     m_gocodeCmd;
    QString     m_lastGopathEnv;
    bool        m_closeOnExit;
    bool        m_autoUpdatePkg;
    bool        m_allImportHint;
};

#endif // GOLANGCODE_H
