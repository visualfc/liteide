/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2013 LiteIDE Team. All rights reserved.
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
// Module: litedoc.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "litedoc.h"
#include "documentbrowser/documentbrowser.h"
#include "htmlutil/htmlutil.h"
#include "sundown/mdtohtml.h"

#include <QAction>
#include <QMenu>
#include <QFile>
#include <QTextCodec>
#include <QFileInfo>
#include <QDir>
#include <QPlainTextEdit>
#include <QDesktopServices>
#include <QStatusBar>
#include <QComboBox>
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

LiteDoc::LiteDoc(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::ILiteDoc(parent),
    m_liteApp(app)
{
    m_docBrowser = new DocumentBrowser(app,this);
    m_docBrowser->urlComboBox()->setEditable(false);
    m_docBrowser->setName(tr("LiteIDE Document Browser"));
    QStringList paths;
    paths << m_liteApp->resourcePath()+"/welcome";
    m_docBrowser->setSearchPaths(paths);

    m_browserAct = m_liteApp->editorManager()->registerBrowser(m_docBrowser);
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuBrowserPos,m_browserAct);

    connect(m_docBrowser,SIGNAL(requestUrl(QUrl)),this,SLOT(openUrl(QUrl)));
    connect(m_docBrowser,SIGNAL(highlighted(QUrl)),this,SLOT(highlighted(QUrl)));

    QString path = m_liteApp->resourcePath()+"/welcome/doc.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }

    m_liteApp->extension()->addObject("LiteApi.ILiteDoc",this);

    QUrl url("/readme.html");
    openUrl(url);
}

void LiteDoc::activeBrowser()
{
    m_liteApp->editorManager()->activeBrowser(m_docBrowser);
}

QUrl LiteDoc::parserUrl(const QUrl &_url)
{
    QDir root(m_liteApp->resourcePath()+"/welcome");
    QUrl url = _url;
    if (url.isRelative() && !url.path().isEmpty()) {
        QFileInfo info;
        if (url.path().at(0) == '/') {
            info.setFile(root,url.path().right(url.path().length()-1));
        } else if (m_lastUrl.scheme() == "file") {
            info.setFile(QFileInfo(m_lastUrl.toLocalFile()).absoluteDir(),url.path());
        }
        if (info.exists() && info.isFile()) {
            url.setScheme("file");
            url.setPath(info.filePath());
        }
    }
    return url;
}

void LiteDoc::openUrl(const QUrl &_url)
{
    m_liteApp->mainWindow()->statusBar()->clearMessage();

    QUrl url = parserUrl(_url);
    if (url.scheme() == "file") {
        openUrlFile(url);
    } else if (url.scheme() == "http" ||
               url.scheme() == "https" ||
               url.scheme() == "mailto") {
        QDesktopServices::openUrl(url);
    }
}

void LiteDoc::highlighted(const QUrl &url)
{
    m_liteApp->mainWindow()->statusBar()->showMessage(url.toString());
}

void LiteDoc::openUrlFile(const QUrl &url)
{
    QFileInfo info(url.toLocalFile());
    if (!info.exists()) {
        info.setFile(url.path());
    }
    QString ext = info.suffix().toLower();
    if (ext == "html") {
        QFile file(info.filePath());
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray ba = file.readAll();
            file.close();
            if (info.fileName().compare("docs.html",Qt::CaseInsensitive) == 0) {
                updateHtmlDoc(url,ba,QString(),false);
            } else {
                updateHtmlDoc(url,ba);
            }
        }
    } else if (ext == "md") {
        QFile file(info.filePath());
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray ba = mdtohtml(file.readAll());
            updateHtmlDoc(url,ba);
        }
    } else if (ext == "go") {
        LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(info.filePath());
        if (editor) {
            editor->setReadOnly(true);
            QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
            if (ed && url.hasQueryItem("s")) {
                QStringList pos = url.queryItemValue("s").split(":");
                if (pos.length() == 2) {
                    bool ok = false;
                    int begin = pos.at(0).toInt(&ok);
                    if (ok) {
                        QTextCursor cur = ed->textCursor();
                        cur.setPosition(begin);
                        ed->setTextCursor(cur);
                        ed->centerCursor();
                    }
                }
            }
        }
    } else if (ext == "pdf") {
        QDesktopServices::openUrl(info.filePath());
    } else {
        QFile file(info.filePath());
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray ba = file.readAll();
            updateTextDoc(url,ba,info.fileName());
        }
    }
}

void LiteDoc::updateTextDoc(const QUrl &url, const QByteArray &ba, const QString &header)
{
    m_lastUrl = url;
    QTextCodec *codec = QTextCodec::codecForUtfText(ba,QTextCodec::codecForName("utf-8"));
    QString html = Qt::escape(codec->toUnicode(ba));
    QString data = m_templateData;
    data.replace("{header}",header);
    data.replace("{nav}","");
    data.replace("{content}",QString("<pre>%1</pre>").arg(html));
    m_docBrowser->setUrlHtml(url,data);

}

void LiteDoc::updateHtmlDoc(const QUrl &url, const QByteArray &ba, const QString &header, bool toNav)
{
    m_lastUrl = url;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString genHeader;
    QString nav;
    QString content = HtmlUtil::docToNavdoc(codec->toUnicode(ba),genHeader,nav);
    QString data = m_templateData;

    if (!header.isEmpty()) {
        data.replace("{header}",header);
    } else {
        data.replace("{header}",genHeader);
    }
    if (toNav) {
        data.replace("{nav}",nav);
    } else {
        data.replace("{nav}","");
    }
    data.replace("{content}",content);
    m_docBrowser->setUrlHtml(url,data);
}
