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
// Module: welcomebrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "welcomebrowser.h"
#include "golangdocapi/golangdocapi.h"
#include "litedoc.h"
#include <QMenu>
#include <QStatusBar>
#include <QToolBar>
#include <QDir>
#include <QFileInfo>
#include <QAction>
#include <QFile>
#include <QTextBrowser>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
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


WelcomeBrowser::WelcomeBrowser(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IBrowserEditor(parent),
      m_liteApp(app),
      m_extension(new Extension),
      m_widget(new QWidget)
{
    m_browser = new DocumentBrowser(m_liteApp,this);
    m_browser->toolBar()->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);

    mainLayout->addWidget(m_browser->widget());

    QPushButton *newFile = new QPushButton(tr("New"));
    QPushButton *openFile = new QPushButton(tr("Open"));
    QPushButton *openFolder = new QPushButton(tr("Open Folder"));
    QPushButton *options = new QPushButton(tr("Options"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(newFile);
    layout->addWidget(openFile);
    layout->addWidget(openFolder);
    layout->addStretch(1);
    layout->addWidget(options);

    mainLayout->addLayout(layout);

    m_widget->setLayout(mainLayout);

    connect(newFile,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(newFile()));
    connect(openFile,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openFiles()));
    connect(openFolder,SIGNAL(clicked()),this,SLOT(openFolder()));

    connect(options,SIGNAL(clicked()),m_liteApp->optionManager(),SLOT(exec()));
    connect(m_browser,SIGNAL(requestUrl(QUrl)),this,SLOT(openUrl(QUrl)));
    connect(m_liteApp->fileManager(),SIGNAL(recentFilesChanged(QString)),this,SLOT(loadData()));
    connect(m_browser,SIGNAL(linkHovered(QUrl)),this,SLOT(highlightedUrl(QUrl)));

    QStringList paths;
    paths << m_liteApp->resourcePath()+"/welcome" << LiteDoc::localePath(m_liteApp->resourcePath()+"/welcome");
    m_browser->setSearchPaths(paths);

    m_extension->addObject("LiteApi.QTextBrowser",m_browser->htmlWidget()->widget());

    QString path = LiteDoc::localeFile(m_liteApp->resourcePath()+"/welcome/welcome.html");
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = QString::fromUtf8(file.readAll());
        file.close();
    }
    loadData();
}

WelcomeBrowser::~WelcomeBrowser()
{
    delete m_browser;
    delete m_widget;
    if (m_extension) {
        delete m_extension;
    }
}

void WelcomeBrowser::openFolder()
{
    m_liteApp->fileManager()->openFolder();
}

LiteApi::IExtension *WelcomeBrowser::extension()
{
    return m_extension;
}

void WelcomeBrowser::highlightedUrl(const QUrl &url)
{
    m_liteApp->mainWindow()->statusBar()->showMessage(url.toString(),2000);
}

void WelcomeBrowser::openUrl(const QUrl &url)
{
    m_liteApp->mainWindow()->statusBar()->clearMessage();

    if (url.scheme() == "http" ||
            url.scheme() == "https" ||
            url.scheme() == "mailto") {
        QDesktopServices::openUrl(url);
    } else if (url.scheme() == "session") {
        m_liteApp->loadSession(url.path());
    } else if (url.scheme() == "proj") {
        m_liteApp->fileManager()->openProject(url.path());
    } else if (url.scheme() == "file") {
        m_liteApp->fileManager()->openEditor(url.path());
    } else if (url.scheme() == "folder") {
        m_liteApp->fileManager()->addFolderList(url.path());
    }else if (url.scheme() == "doc") {
        LiteApi::ILiteDoc *doc = LiteApi::findExtensionObject<LiteApi::ILiteDoc*>(m_liteApp,"LiteApi.ILiteDoc");
        if (doc) {
            doc->openUrl(url.path());
            doc->activeBrowser();
        }
    } else if (url.scheme() == "godoc") {
        LiteApi::IGolangDoc *doc = LiteApi::findExtensionObject<LiteApi::IGolangDoc*>(m_liteApp,"LiteApi.IGolangDoc");
        if (doc) {
            doc->openUrl(url.path());
            doc->activeBrowser();
        }
    } else if (url.scheme() == "goplay") {
        LiteApi::IEditor *browser = LiteApi::findExtensionObject<LiteApi::IEditor*>(m_liteApp,"LiteApi.Goplay");
        if (browser) {
            m_liteApp->editorManager()->activeBrowser(browser);
        }
    }
}

void WelcomeBrowser::loadData()
{
    QString data = m_templateData;

    QStringList sessionList;
    sessionList.append("<ul>");
    sessionList.append(QString("<li><a href=\"session:default\">default</a></li>"));
    sessionList.append("</ul>");


    QStringList list;
    QStringList schemeList;
    //schemeList = m_liteApp->fileManager()->schemeList();
    schemeList << "folder" << "file";
    schemeList.append(m_liteApp->fileManager()->schemeList());
    schemeList.removeDuplicates();
    foreach (QString scheme, schemeList) {
        QString s = scheme.left(1).toUpper()+scheme.right(scheme.length()-1);
        list.append(QString("<h3><i>Recent %1</i></h3>").arg(s));
        list.append("<table border=\"0\"><tr><td>");
        list.append("<ul>");
        QStringList recentProjects = m_liteApp->fileManager()->recentFiles(scheme);
        int count = 0;
        foreach (QString file, recentProjects) {
            QFileInfo info(file);
            list.append(QString("<li><a href=\"%1:%2\">%3</a> <span class=\"recent\">%4</span></li>")
                               .arg(scheme)
                               .arg(info.filePath())
                               .arg(info.fileName())
                               .arg(QDir::toNativeSeparators(info.filePath())));
            if (count++ > 8) {
                break;
            }
        }
        list.append("</ul>");
        list.append("</td></tr></table>");
    }
    data.replace("{liteide_version}",m_liteApp->ideVersion());
    data.replace("{recent_sessions}",sessionList.join("\n"));
    data.replace("{recent_files}",list.join("\n"));
    QUrl url(m_liteApp->resourcePath()+"/welcome/welcome.html");
    m_browser->setUrlHtml(url,data);
}

QWidget *WelcomeBrowser::widget()
{
    return m_widget;
}

QString WelcomeBrowser::name() const
{
    return tr("Welcome");
}

QString WelcomeBrowser::mimeType() const
{
    return "browser/welcome";
}
