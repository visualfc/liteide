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
// Module: packageproject.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-19
// $Id: packageproject.cpp,v 1.0 2012-4-19 visualfc Exp $

#include "packageproject.h"
#include "filepathmodel.h"
#include "gotool.h"
#include "qjson/include/QJson/Parser"
#include "fileutil/fileutil.h"
#include "golangdocapi/golangdocapi.h"
#include <QDir>
#include <QFileInfo>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QTimer>
#include <QUrl>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QDesktopServices>

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

PackageProject::PackageProject(LiteApi::IApplication *app) :
    m_liteApp(app)
{
    m_goTool = new GoTool(m_liteApp,this);

    m_widget = new QWidget;   
    m_reloadTimer = new QTimer(this);
    m_reloadTimer->setSingleShot(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    m_treeView = new PackageTree(m_widget);
    m_treeView->setEditTriggers(QTreeView::NoEditTriggers);
    m_treeView->setHeaderHidden(true);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(m_treeView);
    m_widget->setLayout(layout);

    m_contextMenu = new QMenu;

    QAction *reloadAct = new QAction(tr("Reload Package"),this);
    QAction *explorerAct = new QAction(tr("Open Explorer Here"),this);
    QAction *addSource = new QAction(tr("Add Source File"),this);
    m_contextMenu->addAction(reloadAct);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(explorerAct);
    m_contextMenu->addAction(addSource);

    connect(m_treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClicked(QModelIndex)));
    connect(m_goTool,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    //connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(m_reloadTimer,SIGNAL(timeout()),this,SLOT(reload()));
    connect(addSource,SIGNAL(triggered()),this,SLOT(addSource()));
    connect(reloadAct,SIGNAL(triggered()),this,SLOT(reload()));
    connect(explorerAct,SIGNAL(triggered()),this,SLOT(openExplorer()));
    connect(m_treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
}

PackageProject::~PackageProject()
{
    if (m_reloadTimer) {
        m_reloadTimer->stop();
        delete m_reloadTimer;
    }
    if (m_goTool) {
         delete m_goTool;
    }
    if (m_contextMenu) {
        delete m_contextMenu;
    }
    if (m_widget) {
        delete m_widget;
    }
}

void PackageProject::customContextMenuRequested(QPoint pos)
{
    QMenu *contextMenu = m_contextMenu;
/*
    if (node->isDir()) {
        contextMenu = m_folderMenu;
    } else {
        contextMenu = m_fileMenu;
    }
*/
    if (contextMenu && contextMenu->actions().count() > 0) {
        contextMenu->popup(m_treeView->mapToGlobal(pos));
    }
}

void PackageProject::loadProject(LiteApi::IApplication *app, const QString &path)
{
    PackageProject *project = new PackageProject(app);
    project->setPath(path);
    project->reload();
    app->projectManager()->setCurrentProject(project);
    QAction *act = app->toolWindowManager()->findToolWindow(app->projectManager()->widget());
    if (act) {
        act->setChecked(true);
    }
}

void PackageProject::setPath(const QString &path)
{
    m_filePath = path;
    m_goTool->setWorkDir(path);
}

void PackageProject::reload()
{
    if (m_reloadTimer->isActive()) {
        m_reloadTimer->stop();
    }
    m_goTool->reloadEnv();
    m_goTool->start(QStringList() << "list" << "-e" << "-json" << ".");
}

void PackageProject::setJson(const QMap<QString,QVariant> &json)
{
    m_json = json;
}

QWidget *PackageProject::widget()
{
    return m_widget;
}

QString PackageProject::name() const
{
    return m_json.value("ImportPath").toString();
}
QString PackageProject::filePath() const
{
    return m_filePath;
}
QString PackageProject::mimeType() const
{
    return "text/x-gopackage";
}

QStringList PackageProject::folderList() const
{
    return QStringList() << m_filePath;
}

QStringList PackageProject::fileNameList() const
{
    return m_treeView->nameList;
}
QStringList PackageProject::filePathList() const
{
    return m_treeView->fileList;
}
QString PackageProject::fileNameToFullPath(const QString &filePath)
{
    QDir dir(m_filePath);
    return QFileInfo(dir,filePath).filePath();
}

/*
valueForKey

EDITORPATH
EDITORNAME
EDITORDIR

PROJECTPATH
PROJECTNAME
PROJECTDIR

WORKDIR

TARGETPATH
TARGETNAME
TARGETDIR
*/

QMap<QString,QString> PackageProject::targetInfo() const
{
    QMap<QString,QString> m;
    QDir dir(m_filePath);
    m.insert("WORKDIR",dir.path());
    QString name = m_json.value("ImportPath").toString();
    name = QFileInfo(name).fileName();
    m.insert("TARGETPATH",QFileInfo(dir,name).filePath());
    m.insert("TARGETNAME",name);
    m.insert("TARGETDIR",dir.path());
    return m;
}

void PackageProject::load()
{
    m_treeView->loadJson(m_json);
}

void PackageProject::doubleClicked(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QStandardItem *item = m_treeView->model->itemFromIndex(index);
    if (!item) {
        return;
    }
    switch (item->data(PackageType::RoleItem).toInt()) {
        case PackageType::ITEM_SOURCE: {
            QString path = item->data(RolePath).toString();
            m_liteApp->fileManager()->openEditor(path,true);
        }
        break;
    case PackageType::ITEM_DEP:
    case PackageType::ITEM_IMPORT: {
        QString pkg = item->data(Qt::DisplayRole).toString();
        LiteApi::IGolangDoc *doc = LiteApi::getGolangDoc(m_liteApp);
        if (doc) {
            doc->openUrl(QUrl(QString("pdoc:%1").arg(pkg)));
            doc->activeBrowser();
        }
        }
        break;
    }

}

void PackageProject::finished(int code, QProcess::ExitStatus)
{
    if (code != 0) {
        return;
    }
    QJson::Parser parser;
    bool ok = false;
    QVariant json = parser.parse(m_goTool->stdOutputData(), &ok).toMap();
    if (ok) {
        m_json = json.toMap();
        load();
        emit reloaded();
    }
}

void PackageProject::editorSaved(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    QString filePath = editor->filePath();
    bool find = false;
    foreach(QString path, m_treeView->fileList) {
        if (FileUtil::compareFile(filePath,path)) {
            find = true;
            break;
        }
    }
    if (find) {
        //m_reloadTimer->start(10000);
    }
}

void PackageProject::openExplorer()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(this->filePath()));
}

void PackageProject::addSource()
{
    QString source = QInputDialog::getText(m_widget,tr("Add Source File"),tr("FileName:"));
    if (source.isEmpty()) {
        return;
    }
    QDir dir(m_filePath);
    QFileInfo info(dir,source);
    QString fileName = info.filePath();
    if (info.suffix().isEmpty()) {
        fileName += ".go";
    }
    if (QFile::exists(fileName)) {
        QMessageBox::information(m_widget,tr("LiteApp"),QString(tr("File %1 exists")).arg(fileName));
        return;
    }
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::information(m_widget,tr("LiteApp"),QString(tr("Open File %1 false")).arg(fileName));
        return;
    }
    file.write(QString("package %1\n").arg(m_json.value("Name").toString()).toLatin1());
    file.close();
    this->reload();
}
