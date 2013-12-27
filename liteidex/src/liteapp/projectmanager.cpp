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
// Module: projectmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "projectmanager.h"
#include "liteapp_global.h"
#include <QFileInfo>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStackedLayout>
#include <QMenu>
#include <QActionGroup>
#include <QFileDialog>
#include <QDebug>
#include <QScrollArea>
#include <QTabWidget>
#include "fileutil/fileutil.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


ProjectManager::ProjectManager()
    : m_widget(0), m_folderProject(0)
{
}

ProjectManager::~ProjectManager()
{
    if (m_folderProject) {
        delete m_folderProject;
    }
    if (m_widget) {
        m_liteApp->toolWindowManager()->removeToolWindow(m_widget);
        delete m_widget;
    }
}

bool ProjectManager::initWithApp(IApplication *app)
{
    if (!IProjectManager::initWithApp(app)) {
        return false;
    }

    m_widget = new QScrollArea;
    m_widget->setFrameShape(QFrame::NoFrame);
    m_widget->setWidgetResizable(true);

    m_folderProject = new FolderProject(m_liteApp);
    //m_toolWindowAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::LeftDockWidgetArea,m_widget,"projects",tr("Projects"),false);

    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));

    m_bAutoCloseProjectEditors = m_liteApp->settings()->value(LITEAPP_AUTOCLOSEPROEJCTFILES,true).toBool();

    return true;
}

void ProjectManager::appLoaded()
{
    foreach (IProjectFactory *factory, this->m_factoryList) {
        foreach(QString type, factory->mimeTypes()) {
            IMimeType *mimeType = m_liteApp->mimeTypeManager()->findMimeType(type);
            if (mimeType && !mimeType->scheme().isEmpty()) {
                QAction *act = new QAction(QString(tr("Project <%1>").arg(mimeType->scheme())),this);
                act->setData(mimeType->scheme());
                connect(act,SIGNAL(triggered()),this,SLOT(openSchemeAct()));
                //m_importMenu->addAction(act);
            }
        }
    }
}

QWidget *ProjectManager::widget()
{
    return m_widget;
}

void ProjectManager::triggeredProject(QAction* act)
{
    QString fileName = act->text();
    m_liteApp->fileManager()->openProject(fileName);
}

void ProjectManager::openSchemeDialog(const QString &scheme)
{
    static QString last = QDir::homePath();
    QString dir = QFileDialog::getExistingDirectory(m_liteApp->mainWindow(),
                                                    QString(tr("Import Directory <%1>").arg(scheme)),
                                                     last,
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        return;
    }
    last = dir;
    m_liteApp->fileManager()->openProjectScheme(dir,scheme);
}

void ProjectManager::openSchemeAct()
{
    QAction *act = (QAction*)sender();
    if (!act) {
        return;
    }
    QString scheme = act->data().toString();
    if (scheme.isEmpty()) {
        return;
    }
    openSchemeDialog(scheme);
}

void ProjectManager::currentEditorChanged(LiteApi::IEditor* editor)
{
    if (!editor) {
        return;
    }
//    QString fileName = editor->fileName();
//    IProject *project = 0;
//    foreach (IProjectFactory *factory , m_factoryList) {
//        project = factory->findByEditor(editor);
//        if (project) {
//            break;
//        }
//    }
//    if (project) {
//        QAction *act = m_mapNameToAction.value(fileName);
//        if (act == 0) {
//            act = m_projectActGroup->addAction(fileName);
//            act->setCheckable(true);
//            m_mapNameToAction.insert(fileName,act);
//            m_projectMenu->addAction(act);
//        }
//        setCurrentProject(project);
//    }
}

IFolderProject *ProjectManager::openFolder(const QString &folderPath)
{
    m_folderProject->openFolder(folderPath);
    setCurrentProject(m_folderProject);
    return m_folderProject;
}

IProject *ProjectManager::openProject(const QString &fileName, const QString &mimeType)
{
    if (m_currentProject && m_currentProject->filePath() == fileName) {
        return m_currentProject;
    }
    IProject *project = 0;
    foreach (IProjectFactory *factory , m_factoryList) {
        if (factory->mimeTypes().contains(mimeType)) {
            project = factory->open(fileName,mimeType);
            if (project) {
                break;
            }
        }
    }

    if (project) {
        setCurrentProject(project);
    }
    return project;
}

void ProjectManager::addFactory(IProjectFactory *factory)
{
    m_factoryList.append(factory);
}

void ProjectManager::removeFactory(IProjectFactory *factory)
{
    m_factoryList.removeOne(factory);
}

QList<IProjectFactory*> ProjectManager::factoryList() const
{
    return m_factoryList;
}

QStringList ProjectManager::mimeTypeList() const
{
    QStringList types;
    foreach(IProjectFactory *factory, m_factoryList) {
        types.append(factory->mimeTypes());
    }
    return types;
}

void ProjectManager::setCurrentProject(IProject *project)
{
    if (m_currentProject) {
        closeProjectHelper(m_currentProject);
    }
    m_currentProject = project;

    if (m_currentProject) {
        //m_toolWindowAct->setChecked(true);
        m_widget->setWidget(m_currentProject->widget());
        m_currentProject->load();
        m_liteApp->appendLog("ProjectManager","Loaded project "+m_currentProject->name());
    }
    emit currentProjectChanged(project);
}

IProject *ProjectManager::currentProject() const
{
    return m_currentProject;
}

QList<IEditor*> ProjectManager::editorList(IProject *project) const
{
    QList<IEditor*> editors;
    if (project) {
        foreach (QString fileName, project->filePathList()) {
             foreach(IEditor *editor, m_liteApp->editorManager()->editorList()) {
                if (FileUtil::compareFile(editor->filePath(),fileName)) {
                    editors << editor;
                    break;
                }
            }
        }
    }
    return editors;
}

void ProjectManager::addImportAction(QAction*)
{
    //m_importMenu->addAction(act);
}

void ProjectManager::saveProject(IProject *project)
{
    IProject *cur = 0;
    if (project == 0) {
        cur = m_currentProject;
    } else {
        cur = project;
    }
    if (cur == 0) {
        return;
    }
    foreach (IEditor *editor, editorList(cur)) {
        if (editor->isModified()) {
            m_liteApp->editorManager()->saveEditor(editor);
        }
    }
}

void ProjectManager::closeProjectHelper(IProject *project)
{
    IProject *cur = 0;
    if (project) {
        cur = project;
    } else {
       cur = m_currentProject;
    }
    if (cur == 0) {
        return;
    }
    //m_widget->clear();
    emit projectAboutToClose(cur);

    m_widget->takeWidget();

    if (m_bAutoCloseProjectEditors) {
        foreach (IEditor *editor, editorList(cur)) {
            m_liteApp->editorManager()->closeEditor(editor);
        }
    }

    m_liteApp->appendLog("ProjectManager","Closed project "+cur->name());
    if (cur != m_folderProject) {
        delete cur;
    } else if (cur && (cur == m_currentProject) ){
        m_currentProject = 0;
    }
}

void ProjectManager::closeProject(IProject *project)
{
    //m_toolWindowAct->setChecked(false);
    closeProjectHelper(project);
    emit currentProjectChanged(0);
}

void ProjectManager::applyOption(QString id)
{
    if (id != OPTION_LITEAPP) {
        return;
    }
    m_bAutoCloseProjectEditors = m_liteApp->settings()->value(LITEAPP_AUTOCLOSEPROEJCTFILES,true).toBool();
}
