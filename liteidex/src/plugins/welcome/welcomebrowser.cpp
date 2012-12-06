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
// Module: welcomebrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: welcomebrowser.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "welcomebrowser.h"
#include "ui_welcomewidget.h"
#include "golangdocapi/golangdocapi.h"
#include "litedoc.h"
#include <QMenu>
#include <QDir>
#include <QFileInfo>
#include <QAction>
#include <QFile>
#include <QTextBrowser>
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


WelcomeBrowser::WelcomeBrowser(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IBrowserEditor(parent),
      m_liteApp(app),
      m_extension(new Extension),
      m_widget(new QWidget),
      ui (new Ui::WelcomeWidget)
{
    ui->setupUi(m_widget);

    connect(ui->newFileButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(newFile()));
    connect(ui->openFileButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openFiles()));
//    connect(ui->openProjectButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openProjects()));
    connect(ui->openFolderButton,SIGNAL(clicked()),this,SLOT(openFolder()));
//    connect(ui->openEditorButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openEditors()));
    connect(ui->optionsButton,SIGNAL(clicked()),m_liteApp->optionManager(),SLOT(exec()));
    connect(ui->textBrowser,SIGNAL(anchorClicked(QUrl)),this,SLOT(openUrl(QUrl)));
    connect(m_liteApp->fileManager(),SIGNAL(recentFilesChanged(QString)),this,SLOT(loadData()));
    connect(ui->textBrowser,SIGNAL(highlighted(QUrl)),this,SLOT(highlightedUrl(QUrl)));

    ui->textBrowser->setSearchPaths(QStringList() << m_liteApp->resourcePath()+"/welcome");
    ui->textBrowser->setOpenLinks(false);

    m_extension->addObject("LiteApi.QTextBrowser",ui->textBrowser);

    QString path = m_liteApp->resourcePath()+"/welcome/welcome.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }
    loadData();
}

WelcomeBrowser::~WelcomeBrowser()
{
    delete ui;
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
    ui->statusLabel->setText(url.toString());
}

void WelcomeBrowser::openUrl(const QUrl &url)
{
    if (url.scheme() == "http" || url.scheme() == "mailto") {
        QDesktopServices::openUrl(url);
    } else if (url.scheme() == "session") {
        m_liteApp->loadSession(url.path());
    } else if (url.scheme() == "proj") {
        m_liteApp->fileManager()->openProject(url.path());
    } else if (url.scheme() == "file") {
        m_liteApp->fileManager()->openEditor(url.path());
    } else if (url.scheme() == "folder") {
        m_liteApp->fileManager()->openFolderProject(url.path());
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
    foreach (QString scheme, m_liteApp->fileManager()->schemeList()) {
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

    data.replace("{recent_sessions}",sessionList.join("\n"));
    data.replace("{recent_files}",list.join("\n"));

    ui->textBrowser->setHtml(data);

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
