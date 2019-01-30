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
// Module: golangast.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGAST_H
#define GOLANGAST_H

#include "liteapi/liteapi.h"
#include "golangastapi/golangastapi.h"
#include "symboltreeview/symboltreeview.h"
#include <QProcess>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class QStackedWidget;
class AstWidget;
class QLabel;

class GolangAst : public LiteApi::IGolangAst
{
    Q_OBJECT
public:
    explicit GolangAst(LiteApi::IApplication *app, QObject *parent = 0);
    ~GolangAst();
    virtual QIcon iconFromTag(const QString &tag, bool pub) const;
    virtual QIcon iconFromTagEnum(LiteApi::ASTTAG_ENUM tag, bool pub) const;
public:
    void setEnable(bool b);
    void loadProject(LiteApi::IProject *project);
    void loadProjectPath(const QString &path);
public slots:
    void astProjectEnable(bool);
    void astFileEnable(bool);
    void projectReloaded();
    void projectChanged(LiteApi::IProject*);
    void editorCreated(LiteApi::IEditor*);
    void editorAboutToClose(LiteApi::IEditor *editor);
    void editorChanged(LiteApi::IEditor*);
    void editorSaved(LiteApi::IEditor*);
    void finishedProcess(int,QProcess::ExitStatus);
    void finishedProcessFile(int,QProcess::ExitStatus);
    void updateAst();
    void updateAstNow();
    void updateAstFile();
    void updateAstNowFile();
    void syncClassView(bool b);
    void syncOutline(bool b);
    void editorPositionChanged();
protected:
    LiteApi::IApplication *m_liteApp;
    QTimer  *m_timer;
    QTimer  *m_timerFile;
    QProcess *m_process;
    QProcess *m_processFile;
    QStringList m_updateFileNames;
    QStringList m_updateFilePaths;
    QString m_editorFileName;
    QString m_editorFilePath;
    QString m_workPath;
    QStackedWidget *m_stackedWidget;
    QLabel    *m_blankWidget;
    AstWidget *m_projectAstWidget;
    LiteApi::IEditor *m_currentEditor;
    QPlainTextEdit   *m_currentPlainTextEditor;
    QMap<LiteApi::IEditor*,AstWidget*> m_editorAstWidgetMap;
    QAction *m_syncClassViewAct;
    QAction *m_syncOutlineAct;
    bool    m_isSyncClassView;
    bool    m_isSyncOutline;
    QAction *m_classViewToolAct;
    QAction *m_outlineToolAct;
};

#endif // GOLANGAST_H
