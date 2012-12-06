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
// Module: packagebrowser.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-19
// $Id: packagebrowser.h,v 1.0 2012-4-19 visualfc Exp $

#ifndef PACKAGEBROWSER_H
#define PACKAGEBROWSER_H

#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include "symboltreeview/symboltreeview.h"
#include "gotool.h"

class QTreeView;

namespace PackageType {
    enum ITEM_TYPE{
        ITEM_NONE = 0,
        ITEM_PACKAGE,
        ITEM_SOURCE,
        ITEM_IMPORT,
        ITEM_DEP
    };
    enum Role_Type{
        RoleItem = Qt::UserRole+1,
        RolePath
    };
}

struct PathData
{
    QString     path;
    QByteArray  data;
};

class QStandardItemModel;
class PackageBrowser : public QObject
{
    Q_OBJECT
public:
    explicit PackageBrowser(LiteApi::IApplication *app, QObject *parent = 0);
    ~PackageBrowser();
signals:
    
public slots:    
    void appLoaded();
    void triggeredToolWindow(bool);
    void currentEnvChanged(LiteApi::IEnv*);
    void reloadAll();
    void setupGopath();
    void loadPackageDoc();
    void loadPackageProject();
    void finished(int,QProcess::ExitStatus);
    void error(QProcess::ProcessError);
    void customContextMenuRequested(QPoint);
    void doubleClicked();
    void resetTree();
    void copyPackageName();
    void fileWizardFinished(const QString &type, const QString &scheme, const QString &location);
protected:
    bool loadPackageProjectHelper(QModelIndex index);
protected:
    QStringList m_taskList;
    QList<PathData> m_taskData;
    QStringList m_gopathList;
    LiteApi::IApplication *m_liteApp;
    QWidget      *m_widget;
    SymbolTreeView    *m_treeView;
    QStandardItemModel *m_model;
    GoTool       *m_goTool;
    bool         m_groupByPath;
    bool         m_hideStandard;
    QMenu       *m_rootMenu;
    QMenu       *m_pkgMenu;
    QMenu       *m_fileMenu;
    QAction     *m_reloadAct;
    QAction     *m_setupGopathAct;
    QAction     *m_godocAct;
    QAction     *m_editPackageAct;
    QAction     *m_openSrcAct;
    QAction     *m_copyNameAct;
    QAction     *m_toolWindowAct;
    QMap<QString,QVariant> m_pkgJson;
    bool         m_bLoaded;
};

#endif // PACKAGEBROWSER_H
