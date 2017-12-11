/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: packagebrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "packagebrowser.h"
#include "setupgopathdialog.h"
#include "golangpackage_global.h"
#include "liteenvapi/liteenvapi.h"
#include "golangdocapi/golangdocapi.h"
#include "qjson/include/QJson/Parser"
#include "fileutil/fileutil.h"
#include "packageproject.h"
#include "liteapi/liteid.h"
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QUrl>
#include <QClipboard>
#include <QApplication>
#include <QTimer>
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

PackageBrowser::PackageBrowser(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_bLoaded(false)
{
    m_goTool = new GoTool(m_liteApp,this);
    m_widget = new QWidget;
    m_groupByPath = true;
    m_hideStandard = false;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);

    m_model = new QStandardItemModel(this);

    m_treeView = new SymbolTreeView;    
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setModel(m_model);
    m_treeView->setEditTriggers(QTreeView::NoEditTriggers);
    m_treeView->setExpandsOnDoubleClick(false);

    layout->addWidget(m_treeView);
    m_widget->setLayout(layout);

    m_rootMenu = new QMenu;
    m_pkgMenu = new QMenu;
    m_fileMenu = new QMenu;

    m_reloadAct = new QAction(tr("Reload All"),this);
    m_setupGopathAct = new QAction(QIcon("icon:images/gopath.png"),tr("Manage GOPATH..."),this);

    LiteApi::IActionContext *actionContext = m_liteApp->actionManager()->getActionContext(this,"GoPkg");

    actionContext->regAction(m_setupGopathAct,"SetupGOPATH","");

    m_godocAct = new QAction(tr("Use godoc View"),this);
    m_loadPackageInNewWindowAct = new QAction(tr("Load Package in New Window"),this);
    m_addToFoldersAct = new QAction(tr("Add Package to Folders"),this);
    m_openSrcAct = new QAction(tr("Open Source File"),this);
    m_copyNameAct = new QAction(tr("Copy Name to Clipboard"),this);

    m_liteApp->actionManager()->insertMenuActions(ID_MENU_TOOLS,"sep/gopath",true,QList<QAction*>() << m_setupGopathAct);

    m_rootMenu->addAction(m_reloadAct);
    m_rootMenu->addAction(m_setupGopathAct);

    m_pkgMenu->addAction(m_loadPackageInNewWindowAct);
    m_pkgMenu->addAction(m_addToFoldersAct);
    m_pkgMenu->addSeparator();
    m_pkgMenu->addAction(m_godocAct);
    m_pkgMenu->addAction(m_copyNameAct);
    m_pkgMenu->addSeparator();
    m_pkgMenu->addAction(m_reloadAct);
    m_pkgMenu->addAction(m_setupGopathAct);

    m_fileMenu->addAction(m_openSrcAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_reloadAct);
    m_fileMenu->addAction(m_setupGopathAct);


    m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_widget,"GoPackageBrowser",tr("Go Package Browser"),true);
    connect(m_toolWindowAct,SIGNAL(triggered(bool)),this,SLOT(toggledToolWindow(bool)));
    connect(m_goTool,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    connect(m_goTool,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
    connect(m_treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
    connect(m_treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClicked()));
    connect(m_treeView,SIGNAL(enterKeyPressed(QModelIndex)),this,SLOT(enterKeyPressed(QModelIndex)));
    connect(m_reloadAct,SIGNAL(triggered()),this,SLOT(reloadAll()));
    connect(m_setupGopathAct,SIGNAL(triggered()),this,SLOT(setupGopath()));
    connect(m_godocAct,SIGNAL(triggered()),this,SLOT(loadPackageDoc()));
    connect(m_loadPackageInNewWindowAct,SIGNAL(triggered()),this,SLOT(loadPackageInNewWindow()));
    connect(m_addToFoldersAct,SIGNAL(triggered()),this,SLOT(addPackageToFolders()));
    connect(m_openSrcAct,SIGNAL(triggered()),this,SLOT(doubleClicked()));
    connect(m_copyNameAct,SIGNAL(triggered()),this,SLOT(copyPackageName()));

    //QAction *act = new QAction(QIcon("icon:images/gopath.png"),tr("GOPATH Setup"),this);
    //connect(act,SIGNAL(triggered()),this,SLOT(setupGopath()));

    QToolBar *toolBar = m_liteApp->actionManager()->loadToolBar("toolbar/std");
    if (toolBar) {
        toolBar->addSeparator();
        toolBar->addAction(m_setupGopathAct);
    }

    LiteApi::IEnvManager *env = LiteApi::getEnvManager(m_liteApp);
    if (env) {
        connect(env,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
    }
    connect(m_liteApp->fileManager(),SIGNAL(fileWizardFinished(QString,QString,QString)),this,SLOT(fileWizardFinished(QString,QString,QString)));
}

PackageBrowser::~PackageBrowser()
{
    delete m_goTool;
    delete m_rootMenu;
    delete m_pkgMenu;
    delete m_fileMenu;
    if (m_widget) {
        delete m_widget;
    }
}

void PackageBrowser::toggledToolWindow(bool b)
{
    if (b && !m_bLoaded) {
        this->reloadAll();
    }
}

void PackageBrowser::fileWizardFinished(const QString&, const QString&, const QString&)
{
    /*if (scheme == "gopkg") {
        reloadAll();
    }*/
}

void PackageBrowser::currentEnvChanged(LiteApi::IEnv */*env*/)
{
    reloadAll();
}

void PackageBrowser::reloadAll()
{
    QProcessEnvironment env = LiteApi::getGoEnvironment(m_liteApp);
    QString gocmd = FileUtil::lookupGoBin("go",m_liteApp,false);
    if (!gocmd.isEmpty()) {
        m_liteApp->appendLog("GolangPackage","Found go bin at "+QDir::toNativeSeparators(gocmd));
    } else {
        m_liteApp->appendLog("GolangPackage","Could not find go bin, (hint: is Go installed?)",true);
    }
    QString goroot = env.value("GOROOT");
    m_liteApp->appendLog("GolangPackage","GOROOT="+goroot);
    QStringList gopathList = LiteApi::getGOPATH(m_liteApp,false);
#ifdef Q_OS_WIN
    m_liteApp->appendLog("GolangPackage","GOPATH="+gopathList.join(";"));
#else
    m_liteApp->appendLog("GolangPackage","GOPATH="+gopathList.join(":"));
#endif

    m_bLoaded = false;
    if (!m_toolWindowAct->isChecked()) {
        return;
    }
//    if (!LiteApi::hasGoEnv(env)) {
//        return;
//    }
    m_liteApp->appendLog("GolangPackages","reload all packages");
    m_bLoaded = true;
    if (m_model->rowCount() == 0) {
        m_model->appendRow(new QStandardItem(tr("Loading Go package list...")));
    }
    QString root = LiteApi::getGOROOT(m_liteApp);
    m_goTool->setProcessEnvironment(env);
    m_goTool->setWorkDir(root);
    //m_goTool->start(QStringList() << "list" << "-e" << "-json" << "...");
    m_goTool->start_list_json();
}

static bool hasSameList(const QStringList &list1, const QStringList &list2)
{
    if (list1.size() != list2.size()) {
        return false;
    }
    for (int i = 0; i < list1.size(); i++) {
        if (list1[i] != list2[i]) {
            return false;
        }
    }
    return true;
}

void PackageBrowser::setupGopath()
{
    SetupGopathDialog *dlg = new SetupGopathDialog(m_liteApp->mainWindow());
    dlg->setSysPathList(m_goTool->sysGopath());
    dlg->setLitePathList(m_goTool->liteGopath());
    dlg->setUseSysGopath(m_liteApp->settings()->value(LITEIDE_USESYSGOPATH,true).toBool());
    dlg->setUseLiteGopath(m_liteApp->settings()->value(LITEIDE_USELITEIDEGOPATH,true).toBool());
    if (dlg->exec() == QDialog::Accepted) {
        //QStringList orgLitePath = m_goTool->liteGopath();
        QStringList newLitePath = dlg->litePathList();
        //m_liteApp->sendBroadcast("golangpackage","reloadgopath");
        m_liteApp->settings()->setValue(LITEIDE_USESYSGOPATH,dlg->isUseSysGopath());
        m_liteApp->settings()->setValue(LITEIDE_USELITEIDEGOPATH,dlg->isUseLiteGopath());
        //if (!hasSameList(orgLitePath,newLitePath)) {
        m_goTool->setLiteGopath(newLitePath);
        this->reloadAll();
        LiteApi::IGoEnvManger *env = LiteApi::getGoEnvManager(m_liteApp);
        if (env) {
            env->updateGoEnv();
        }
    }
}

void PackageBrowser::loadPackageDoc()
{
    QModelIndex index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    int type = index.data(PackageType::RoleItem).toInt();
    if (type != PackageType::ITEM_PACKAGE &&
            type != PackageType::ITEM_DEP &&
            type != PackageType::ITEM_IMPORT) {
        return;
    }
    QString pkgName = index.data(Qt::DisplayRole).toString();
    if (!pkgName.isEmpty()) {
        LiteApi::IGolangDoc *doc = LiteApi::getGolangDoc(m_liteApp);
        if (doc) {
            doc->openUrl(QUrl(QString("pdoc:%1").arg(pkgName)));
            doc->activeBrowser();
        }
    }
}

void PackageBrowser::copyPackageName()
{
    QModelIndex index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QString name = index.data(Qt::DisplayRole).toString();
    QApplication::clipboard()->setText(name);
}

void PackageBrowser::loadPackageInNewWindow()
{
    QModelIndex index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    loadPackageFolderHelper(index,false);
}

void PackageBrowser::addPackageToFolders()
{
    QModelIndex index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    loadPackageFolderHelper(index,true);
}

bool PackageBrowser::loadPackageFolderHelper(QModelIndex index, bool add)
{
    if (!index.isValid()) {
        return false;
    }
    int type = index.data(PackageType::RoleItem).toInt();
    if (type != PackageType::ITEM_PACKAGE &&
            type != PackageType::ITEM_DEP &&
            type != PackageType::ITEM_IMPORT) {
        return false;
    }

    QString pkgName = index.data(Qt::DisplayRole).toString();
    QVariant json = m_pkgJson.value(pkgName);
    if (json.isNull()) {
        return false;
    }
    QDir dir(json.toMap().value("Dir").toString());
    if (dir.exists()) {
        if (add) {
            m_liteApp->fileManager()->addFolderList(dir.path());
        } else {
            m_liteApp->fileManager()->openFolderInNewWindow(dir.path());
        }
        return true;
    }
    return false;
}

void PackageBrowser::doubleClicked()
{
    QModelIndex index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }

    int type = index.data(PackageType::RoleItem).toInt();
    if (type == PackageType::ITEM_SOURCE) {
        QString path = index.data(PackageType::RolePath).toString();
        if (!path.isEmpty()) {
            m_liteApp->fileManager()->openEditor(path);
        }
    } else if (type == PackageType::ITEM_PACKAGE) {
        if (loadPackageFolderHelper(index,true)) {
            return;
        }
    }
    if (m_treeView->isExpanded(index)) {
        m_treeView->collapse(index);
    } else {
        m_treeView->expand(index);
    }
}

void PackageBrowser::enterKeyPressed(const QModelIndex &index)
{
    int type = index.data(PackageType::RoleItem).toInt();
    if (type == PackageType::ITEM_SOURCE) {
        QString path = index.data(PackageType::RolePath).toString();
        if (!path.isEmpty()) {
            m_liteApp->fileManager()->openEditor(path);
        }
    }
    if (m_treeView->isExpanded(index)) {
        m_treeView->collapse(index);
    } else {
        m_treeView->expand(index);
    }
}

void PackageBrowser::customContextMenuRequested(QPoint pos)
{
    QMenu *contextMenu = m_rootMenu;

    QModelIndex index = m_treeView->currentIndex();
    if (index.isValid()) {
        int type = index.data(PackageType::RoleItem).toInt();
        if (type == PackageType::ITEM_PACKAGE ||
                type == PackageType::ITEM_IMPORT ||
                type == PackageType::ITEM_DEP) {
            contextMenu = m_pkgMenu;
        } else if (type == PackageType::ITEM_SOURCE) {
            contextMenu = m_fileMenu;
        }
    }
    if (contextMenu && contextMenu->actions().count() > 0) {
        contextMenu->popup(m_treeView->mapToGlobal(pos));
    }
}

void PackageBrowser::error(QProcess::ProcessError code)
{
    m_model->clear();
    QString goroot = LiteApi::getGOROOT(m_liteApp);
    m_model->appendRow(new QStandardItem(QString("Load Package Error %1\nGOROOT=%2").arg(code).arg(goroot)));
}

void PackageBrowser::finished(int code,QProcess::ExitStatus)
{
    if (code == 0) {
        QByteArray data = m_goTool->stdOutputData();
        resetTree(data);
    } else {
        m_model->clear();
        QString goroot = LiteApi::getGOROOT(m_liteApp);
        m_model->appendRow(new QStandardItem(QString("Load Package Error %1\nGOROOT=%2").arg(code).arg(goroot)));
    }
}

void PackageBrowser::resetTree(const QByteArray &data)
{
    //save state
    SymbolTreeState state;
    m_treeView->saveState(&state);
    m_model->clear();
    //load tree
    QStringList rootList = LiteApi::getGOPATH(m_liteApp,true);

    foreach (QString root, rootList) {
        QStandardItem *item = new QStandardItem(root);
        QStandardItem *cmd = new QStandardItem("cmd");
        item->appendRow(cmd);
        QStandardItem *pkg = new QStandardItem("pkg");
        item->appendRow(pkg);
        m_model->appendRow(item);
        m_treeView->expand(m_model->indexFromItem(item));
    }
    QByteArray jsonData;
    foreach(QByteArray line, data.split('\n')) {
        jsonData.append(line);
        if (line == "}") {
            QJson::Parser parser;
            bool ok = false;
            QVariant json = parser.parse(jsonData, &ok).toMap();
            jsonData.clear();
            if (!ok) {
                continue;
            }
            QVariantMap jsonMap = json.toMap();
            QString root = QDir::toNativeSeparators(jsonMap.value("Root").toString());
            if (root.isEmpty()) {
                continue;
            }
            QStandardItem *parent = 0;
            for (int i = 0; i < m_model->rowCount(); i++) {
                QModelIndex index = m_model->index(i,0);
                if (index.data() == root) {
                    parent = m_model->itemFromIndex(index);
                    break;
                }
            }
            if (parent == 0) {
                continue;
            }
            if (jsonMap.value("Name").toString() == "main") {
                parent = parent->child(0,0);
            } else {
                parent = parent->child(1,0);
            }
            //if (bRoot && pkgName.indexOf("_") == 0) {
            //    parent = 0;
            //}
            if (parent) {
                QString pkgName = jsonMap.value("ImportPath").toString();
                QStandardItem *item = new QStandardItem(pkgName);
                item->setToolTip(pkgName);
                item->setData(PackageType::ITEM_PACKAGE,PackageType::RoleItem);
                m_pkgJson.insert(pkgName,json);
                QStandardItem *base = new QStandardItem("BaseInfo");
                item->appendRow(base);
                QDir dir(jsonMap.value("Dir").toString());
                foreach (QString key, jsonMap.keys()) {
                    QVariant var = jsonMap.value(key);
                    if (key.indexOf("Error") >= 0) {
                        QString text = QString("%1 : true").arg(key);
                        QStandardItem *ic = new QStandardItem(text);
                        ic->setToolTip(text);
                        base->appendRow(ic);
                        continue;
                    }
                    if (var.type() == QVariant::String ||
                            var.type() == QVariant::Bool) {
                        QString text = QString("%1 : %2").arg(key).arg(var.toString());
                        QStandardItem *ic = new QStandardItem(text);
                        ic->setToolTip(text);
                        base->appendRow(ic);
                    } else if (var.type() == QVariant::List) {
                        PackageType::ITEM_TYPE type = PackageType::ITEM_NONE;
                        if (key.indexOf("Deps") >= 0) {
                            type = PackageType::ITEM_DEP;
                        } else if (key.indexOf("Imports") >= 0) {
                            type = PackageType::ITEM_IMPORT;
                        } else if (key.indexOf("Files") >= 0) {
                            type = PackageType::ITEM_SOURCE;
                        }
                        QStandardItem *ic = new QStandardItem(key);

                        foreach(QVariant v, var.toList()) {
                            if (v.type() == QVariant::String) {
                                QStandardItem *iv = new QStandardItem(v.toString());
                                iv->setData(type,PackageType::RoleItem);
                                if (type == PackageType::ITEM_SOURCE) {
                                    iv->setData(QFileInfo(dir,v.toString()).filePath(),PackageType::RolePath);
                                }
                                ic->appendRow(iv);
                            }
                        }
                        item->appendRow(ic);
                    }
                }
                parent->appendRow(item);
            }
        }
    }
    //load state
    m_treeView->loadState(m_model,&state);
}
