/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: gopathprojectfactor.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-2-27
// $Id: gopathprojectfactor.cpp,v 1.0 2012-2-28 visualfc Exp $

#include "gopathprojectfactor.h"
#include "gopathproject.h"
#include "gopathbrowser.h"
#include "importgopathdialog.h"
#include <QAction>
#include <QFileDialog>
#include <QToolBar>
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

GopathProjectFactor::GopathProjectFactor(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IProjectFactory(parent),
    m_liteApp(app)
{
    m_mimeTypes << "text/x-gopath";
    m_browser = new GopathBrowser(app,this);

    QAction *act = new QAction(QIcon(":/images/gopath.png"),tr("<GOPATH> Project"),this);
    connect(act,SIGNAL(triggered()),this,SLOT(importGopath()));
    m_liteApp->projectManager()->addImportAction(act);

    QToolBar *toolBar = m_liteApp->actionManager()->loadToolBar("toolbar/nav");
    if (toolBar) {
        toolBar->addAction(act);
    }

    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(loadApp()));
}

GopathProjectFactor::~GopathProjectFactor()
{
    if (m_browser) {
        delete m_browser;
    }
}

void GopathProjectFactor::loadApp()
{
    m_browser->reloadEnv();
    GopathProject *project = new GopathProject(m_browser);
    m_liteApp->projectManager()->setCurrentProject(project);
}

void GopathProjectFactor::importGopath()
{
    ImportGopathDialog *dlg = new ImportGopathDialog(m_liteApp->mainWindow());
    dlg->setSysPathList(m_browser->systemGopathList());
    dlg->setPathList(m_browser->pathList());
    if (dlg->exec() == QDialog::Accepted) {
         m_browser->setPathList(dlg->pathList());
         GopathProject *project =  new GopathProject(m_browser);
         m_liteApp->projectManager()->setCurrentProject(project);
    }
    delete dlg;
}

QStringList GopathProjectFactor::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IProject *GopathProjectFactor::open(const QString &fileName, const QString &mimeType)
{
    if (m_mimeTypes.contains(mimeType)) {        
        GopathProject *project =  new GopathProject(m_browser);
        project->browser()->addPathList(fileName);
        return project;
    }
    return 0;
}

bool GopathProjectFactor::findProjectInfo(const QString &fileName, const QString &mimetype, QMap<QString,QString>& projectInfo, QMap<QString,QString>& findProjectInfo) const
{
    return false;
}
