/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: webkitbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "webkitbrowser.h"
#include "sundown/mdtohtml.h"
#include <QWebView>
#include <QNetworkProxyFactory>
#include <QToolBar>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QFile>
#include <QFileInfo>
#include <QStatusBar>
#include <QProgressBar>
#include <QRegExp>
#include <QTextCodec>
#include <QFileDialog>
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

WebKitBrowser::WebKitBrowser(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IWebKitBrowser(parent), m_liteApp(app)
{        
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    m_widget = new QWidget;
    m_view = new QWebView;
    m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    m_locationEdit = new QLineEdit;
    m_locationEdit->setSizePolicy(QSizePolicy::Expanding, m_locationEdit->sizePolicy().verticalPolicy());
    connect(m_locationEdit, SIGNAL(returnPressed()), this,SLOT(changeLocation()));
    connect(m_view, SIGNAL(loadFinished(bool)), this,SIGNAL(loadFinished(bool)));
    connect(m_view, SIGNAL(loadFinished(bool)), this, SLOT(loadUrlFinished(bool)));
    connect(m_view, SIGNAL(linkClicked(QUrl)),this, SLOT(linkClicked(QUrl)));
    connect(m_view->page(), SIGNAL(linkHovered(QString,QString,QString)),this,SLOT(linkHovered(QString,QString,QString)));
    connect(m_view,SIGNAL(statusBarMessage(QString)),this,SLOT(statusBarMessage(QString)));
    connect(m_view,SIGNAL(loadStarted()),this,SLOT(loadStarted()));
    connect(m_view,SIGNAL(loadProgress(int)),this,SLOT(loadProgress(int)));

    QToolBar *toolBar = new QToolBar(tr("Navigation"));
    toolBar->setIconSize(QSize(16,16));

    QAction *openFile = new QAction(tr("Open Html File"),this);
    openFile->setIcon(QIcon("icon:images/openfile.png"));
    connect(openFile,SIGNAL(triggered()),this,SLOT(openHtmlFile()));
    toolBar->addAction(openFile);
    toolBar->addSeparator();
    toolBar->addAction(m_view->pageAction(QWebPage::Back));
    toolBar->addAction(m_view->pageAction(QWebPage::Forward));
    QAction *reload = m_view->pageAction(QWebPage::Reload);
    toolBar->addAction(reload);
    connect(reload,SIGNAL(triggered()),this,SLOT(reloadUrl()));
    toolBar->addAction(m_view->pageAction(QWebPage::Stop));
    toolBar->addWidget(m_locationEdit);

    m_progressBar = new QProgressBar;
    m_progressBar->hide();
    m_progressBar->setRange(0,100);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(toolBar);
    layout->addWidget(m_view);
    layout->addWidget(m_progressBar);

    m_widget->setLayout(layout);

    m_liteApp->extension()->addObject("LiteApp.IWebKitBrowser",this);
}

WebKitBrowser::~WebKitBrowser()
{
    if (m_progressBar) {
        delete m_progressBar;
    }
    if (m_widget) {
        delete m_widget;
    }
}

QWidget *WebKitBrowser::widget()
{
    return m_widget;
}

QString WebKitBrowser::name() const
{
    return tr("WebKitBrowser");
}

QString WebKitBrowser::mimeType() const
{
    return "browser/webkit";
}

void WebKitBrowser::openUrl(const QUrl &url)
{
    loadUrl(url);
}

static QByteArray html_data =
"<html>"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>"
"<title>__HTML_TITLE__</title>"
"</head>"
"<body>"
"__HTML_CONTENT__"
"</body>"
"</html>";

void WebKitBrowser::changeLocation()
{
    QString text = m_locationEdit->text();
    if (text.isEmpty()) {
        return;
    }
    if (!text.startsWith("http:") &&
            !text.startsWith("file:") &&
            !text.startsWith("https:")) {
        text = "http://"+text;
    }
    loadUrl(text);
}

void WebKitBrowser::adjustLocation()
{
    m_locationEdit->setText(m_view->url().toString());
}

void WebKitBrowser::loadUrlFinished(bool b)
{
    m_progressBar->hide();
    QString url = m_view->url().toString();
    if (b) {
        m_locationEdit->setText(url);
    } else {
        QString context = QString(tr("False load %1 !")).arg(url);
    #if QT_VERSION >= 0x050000
        QString html = context.toHtmlEscaped();
    #else
        QString html = Qt::escape(context);
    #endif
        QString data = html_data;
        data.replace("__HTML_TITLE__","LoadFalse");
        data.replace("__HTML_CONTENT__",QString("<pre>%1</pre>").arg(html));
        m_view->setHtml(data,url);
    }
}

void WebKitBrowser::linkClicked(QUrl url)
{
    this->loadUrl(url);
}

void WebKitBrowser::loadUrl(const QUrl &url)
{
    m_liteApp->mainWindow()->statusBar()->clearMessage();
    if (url.scheme() == "http" || url.scheme() == "https") {
        m_view->load(url);
    } else if (url.scheme() == "file") {
        QFileInfo info(url.toLocalFile());
#ifdef Q_OS_WIN
        QString file = info.filePath();
        if (file.length() > 4 && file[0] == '/' && file[1] == '/' &&
                file[2].isLetter() && file[3] == '/') {
            info.setFile(file[2]+":"+file.right(file.length()-3));
        }
#endif
        if (info.exists()) {
            QFile file(info.filePath());
            if (file.open(QFile::ReadOnly)) {
                QByteArray ba = file.readAll();
                QString ext = info.suffix().toLower();
                if (ext == "html" || ext == "htm") {
                    //QTextCodec *codec = QTextCodec::codecForHtml(ba,QTextCodec::codecForName("utf-8"));
                    //m_view->setHtml(codec->toUnicode(ba),url);
                    m_view->load(url);
                } else if (ext == "md") {
                    QString data = html_data;
                    data.replace("__HTML_TITLE__",info.fileName());
                    data.replace("__HTML_CONTENT__",QString::fromUtf8(md2html(ba)));
                    m_view->setHtml(data,url);
                } else {
                    QTextCodec *codec = QTextCodec::codecForUtfText(ba,QTextCodec::codecForName("utf-8"));
                #if QT_VERSION >= 0x050000
                    QString html = codec->toUnicode(ba).toHtmlEscaped();
                #else
                    QString html = Qt::escape(codec->toUnicode(ba));
                #endif
                    QString data = html_data;
                    data.replace("__HTML_TITLE__",info.fileName());
                    data.replace("__HTML_CONTENT__",QString("<pre>%1</pre>").arg(html));
                    m_view->setHtml(data,url);
                }
            }
        }
    }
    m_view->setFocus();
}

void WebKitBrowser::linkHovered(const QString & link, const QString & /*title*/, const QString & /*textContent*/)
{
   m_liteApp->mainWindow()->statusBar()->showMessage(link);
}

void WebKitBrowser::statusBarMessage(const QString &msg)
{
    m_liteApp->mainWindow()->statusBar()->showMessage(msg);
}

void WebKitBrowser::loadStarted()
{
    m_progressBar->show();
}

void WebKitBrowser::loadProgress(int value)
{
    m_progressBar->setValue(value);
}

void WebKitBrowser::openHtmlFile()
{    
    QString dir = m_liteApp->settings()->value("WebKitBrowser/home","").toString();
    QString filePath = QFileDialog::getOpenFileName(m_liteApp->mainWindow(),tr("Open Html or Markdown File"),dir,
                                                    "Html or Markdown File (*.html *.htm *.md *.markdown);;Html File (*.html *.htm);; Markdown File (*.md *.markdown)");
    if (!filePath.isEmpty()) {
        m_liteApp->settings()->setValue("WebKitBrowser/home",QFileInfo(filePath).absolutePath());
        loadUrl(QUrl::fromLocalFile(filePath));
    }
}

void WebKitBrowser::reloadUrl()
{
    this->loadUrl(m_view->url());
}
