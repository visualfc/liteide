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
// Module: documentbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "documentbrowser.h"
#include "extension/extension.h"
#include "sundown/mdtohtml.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QStatusBar>
#include <QComboBox>
#include <QToolBar>
#include <QToolButton>
#include <QCheckBox>
#include <QAction>
#include <QRegExp>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QDir>
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


DocumentBrowser::DocumentBrowser(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IDocumentBrowser(parent),
    m_liteApp(app),
    m_extension(new Extension)
{
    m_widget = new QWidget;

    m_htmlWidget = app->htmlWidgetManager()->createByName(this,"QTextBrowser");

    m_toolBar = new QToolBar;
    m_toolBar->setIconSize(LiteApi::getToolBarIconSize());

    m_backwardAct = new QAction(QIcon("icon:images/backward.png"),tr("Back"),this);
    m_forwardAct = new QAction(QIcon("icon:images/forward.png"),tr("Forward"),this);
    m_reloadUrlAct = new QAction(QIcon("icon:images/reload.png"),tr("Reload"),this);
    m_toolBar->addAction(m_backwardAct);
    m_toolBar->addAction(m_forwardAct);

    m_urlComboBox = new QComboBox;
    m_urlComboBox->setEditable(true);
    m_urlComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_urlComboBox);
    m_toolBar->addAction(m_reloadUrlAct);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_toolBar);
    mainLayout->addWidget(m_htmlWidget->widget());
    m_widget->setLayout(mainLayout);

    connect(m_htmlWidget,SIGNAL(linkHovered(QUrl)),this,SIGNAL(linkHovered(QUrl)));
    connect(m_htmlWidget,SIGNAL(linkClicked(QUrl)),this,SLOT(linkClicked(QUrl)));
    connect(m_htmlWidget,SIGNAL(loadFinished(bool)),this,SIGNAL(documentLoaded()));
    connect(m_backwardAct,SIGNAL(triggered()),this,SLOT(backward()));
    connect(m_forwardAct,SIGNAL(triggered()),this,SLOT(forward()));
    connect(m_reloadUrlAct,SIGNAL(triggered()),this,SLOT(reloadUrl()));
    connect(m_urlComboBox,SIGNAL(activated(QString)),this,SLOT(activatedUrl(QString)));
    connect(this,SIGNAL(backwardAvailable(bool)),m_backwardAct,SLOT(setEnabled(bool)));
    connect(this,SIGNAL(forwardAvailable(bool)),m_forwardAct,SLOT(setEnabled(bool)));

    m_extension->addObject("LiteApi.IDocumentBrowser",this);
    m_extension->addObject("LiteApi.IHtmlWidget",m_htmlWidget);
    m_extension->addObject("LiteApi.QTextBrowser",m_htmlWidget);
    m_htmlWidget->installEventFilter(m_liteApp->editorManager());

    emit backwardAvailable(false);
    emit forwardAvailable(false);
}

DocumentBrowser::~DocumentBrowser()
{
    if (m_widget) {
        delete m_widget;
    }
    if (m_extension) {
        delete m_extension;
    }
}

LiteApi::IExtension *DocumentBrowser::extension()
{
    return m_extension;
}

bool DocumentBrowser::open(const QString &fileName,const QString &mimeType)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }    
    m_mimeType = mimeType;
    QFileInfo info(fileName);
    QString htmlType = m_liteApp->mimeTypeManager()->findMimeTypeByFile(fileName);
    m_name = info.fileName();
    m_fileName = QDir::toNativeSeparators(fileName);

    m_htmlWidget->setSearchPaths(QStringList()<<info.absolutePath());

    QByteArray ba = file.readAll();

    if (htmlType == "text/html") {
        QTextCodec *codec = QTextCodec::codecForHtml(ba,QTextCodec::codecForName("utf-8"));
        setUrlHtml(QUrl::fromLocalFile(fileName),codec->toUnicode(ba));
    } else if (htmlType == "text/x-markdown") {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QByteArray out = mdtohtml(ba);
        setUrlHtml(QUrl::fromLocalFile(fileName),codec->toUnicode(out));
    }
    file.close();
    return true;
}

QWidget *DocumentBrowser::widget()
{
    return m_widget;
}

QString DocumentBrowser::name() const
{
    return m_name;
}

QString DocumentBrowser::filePath() const
{
    return m_fileName;
}

QString DocumentBrowser::mimeType() const
{
    return m_mimeType;
}

void DocumentBrowser::onActive()
{
}


void DocumentBrowser::setName(const QString &t)
{
    m_name = t;
}

void DocumentBrowser::setSearchPaths(const QStringList &paths)
{
    m_htmlWidget->setSearchPaths(paths);
}

QUrl DocumentBrowser::resolveUrl(const QUrl &url) const
{
    if (!url.isRelative())
        return url;

    // For the second case QUrl can merge "#someanchor" with "foo.html"
    // correctly to "foo.html#someanchor"
    if (!(m_url.isRelative()
          || (m_url.scheme() == QLatin1String("file")
              && !QFileInfo(m_url.toLocalFile()).isAbsolute()))
          || (url.hasFragment() && url.path().isEmpty())) {
        return m_url.resolved(url);
    }

    // this is our last resort when current url and new url are both relative
    // we try to resolve against the current working directory in the local
    // file system.
    QFileInfo fi(m_url.toLocalFile());
    if (fi.exists()) {
        return QUrl::fromLocalFile(fi.absolutePath() + QDir::separator()).resolved(url);
    }

    return url;
}

QToolBar *DocumentBrowser::toolBar()
{
    return m_toolBar;
}

QComboBox *DocumentBrowser::urlComboBox()
{
    return m_urlComboBox;
}

LiteApi::IHtmlWidget *DocumentBrowser::htmlWidget()
{
    return m_htmlWidget;
}

void DocumentBrowser::scrollToAnchor(const QString &text)
{
    const HistoryEntry &historyEntry = createHistoryEntry();

    m_url.setFragment(text);

    if (text.isEmpty()) {
        m_htmlWidget->setScrollBarValue(Qt::Horizontal,0);
        m_htmlWidget->setScrollBarValue(Qt::Vertical,0);
    } else {
        m_htmlWidget->scrollToAnchor(text);
    }

    m_urlComboBox->blockSignals(true);
    int index = m_urlComboBox->findText(m_url.toString());
    if (index == -1) {
        m_urlComboBox->addItem(m_url.toString());
        index = m_urlComboBox->count()-1;
    }
    m_urlComboBox->setCurrentIndex(index);
    m_urlComboBox->blockSignals(false);

    emit documentLoaded();

    if (!m_backwardStack.isEmpty() && m_url == m_backwardStack.top().url)
    {
        //restoreHistoryEntry(m_backwardStack.top());
        return;
    }

    if (!m_backwardStack.isEmpty()) {
        m_backwardStack.top() = historyEntry;
    }

    HistoryEntry entry;
    entry.url = m_url;
    m_backwardStack.push(entry);

    emit backwardAvailable(m_backwardStack.count() > 1);

    if (!m_forwardStack.isEmpty() && m_url == m_forwardStack.top().url) {
        m_forwardStack.pop();
        emit forwardAvailable(m_forwardStack.count() > 0);
    } else {
        m_forwardStack.clear();
        emit forwardAvailable(false);
    }
}

void DocumentBrowser::setUrlHtml(const QUrl &url,const QString &data)
{
    const HistoryEntry &historyEntry = createHistoryEntry();
    if (!data.isEmpty()) {
        m_htmlWidget->setHtml(data,url);
    }
    m_url = url;
    if (!url.fragment().isEmpty()) {
        m_htmlWidget->scrollToAnchor(url.fragment());
    } else {
        m_htmlWidget->setScrollBarValue(Qt::Horizontal,0);
        m_htmlWidget->setScrollBarValue(Qt::Vertical,0);
    }

    m_urlComboBox->blockSignals(true);
    int index = m_urlComboBox->findText(m_url.toString());
    if (index == -1) {
        m_urlComboBox->addItem(m_url.toString());
        index = m_urlComboBox->count()-1;
    }
    m_urlComboBox->setCurrentIndex(index);
    m_urlComboBox->blockSignals(false);

    if (!m_backwardStack.isEmpty() && url == m_backwardStack.top().url)
    {
    //    restoreHistoryEntry(m_backwardStack.top());
        return;
    }

    if (!m_backwardStack.isEmpty()) {
        m_backwardStack.top() = historyEntry;
    }

    HistoryEntry entry;
    entry.url = url;
    m_backwardStack.push(entry);

    emit backwardAvailable(m_backwardStack.count() > 1);

    if (!m_forwardStack.isEmpty() && url == m_forwardStack.top().url) {
        m_forwardStack.pop();
        emit forwardAvailable(m_forwardStack.count() > 0);
    } else {
        m_forwardStack.clear();
        emit forwardAvailable(false);
    }
}

void DocumentBrowser::activatedUrl(QString text)
{
    if (text.isEmpty()) {
        return;
    }
    QUrl url(text);
    requestUrl(url);
}

DocumentBrowser::HistoryEntry DocumentBrowser::createHistoryEntry() const
{
    HistoryEntry entry;
    entry.url = m_url;
    entry.hpos = m_htmlWidget->scrollBarValue(Qt::Horizontal);
    entry.vpos = m_htmlWidget->scrollBarValue(Qt::Vertical);
    return entry;
}

void DocumentBrowser::restoreHistoryEntry(const HistoryEntry &entry)
{
    m_url = entry.url;
    m_htmlWidget->setScrollBarValue(Qt::Horizontal,entry.hpos);
    m_htmlWidget->setScrollBarValue(Qt::Vertical,entry.vpos);
}

void DocumentBrowser::backward()
{
    if (m_backwardStack.count() <= 1) {
        return;
    }
    m_forwardStack.push(createHistoryEntry());
    m_backwardStack.pop();
    emit requestUrl(m_backwardStack.top().url);
    emit backwardAvailable(m_backwardStack.count() > 1);
    emit forwardAvailable(true);
}

void DocumentBrowser::reloadUrl()
{
    emit requestUrl(m_url);
}

void DocumentBrowser::forward()
{
    if (m_forwardStack.isEmpty()) {
        return;
    }
    if (!m_backwardStack.isEmpty()) {
        m_backwardStack.top() = createHistoryEntry();
    }
    m_backwardStack.push(m_forwardStack.pop());
    emit requestUrl(m_backwardStack.top().url);
    emit backwardAvailable(true);
    emit forwardAvailable(m_forwardStack.count() > 0);
}

void DocumentBrowser::linkClicked(QUrl url)
{
    if (url.path().isEmpty() && !url.fragment().isEmpty()) {
        m_liteApp->mainWindow()->statusBar()->clearMessage();
        //m_textBrowser->scrollToAnchor(url.fragment());
        this->scrollToAnchor(url.fragment());
        return;
    }
    emit requestUrl(url);
}
