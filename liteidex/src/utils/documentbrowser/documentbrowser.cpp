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
// Module: documentbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: documentbrowser.cpp,v 1.0 2011-7-26 visualfc Exp $

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

    m_textBrowser = new QTextBrowser;
    m_textBrowser->setOpenExternalLinks(false);
    m_textBrowser->setOpenLinks(false);

    m_toolBar = new QToolBar;
    m_toolBar->setIconSize(QSize(16,16));

    m_backwardAct = new QAction(QIcon("icon:images/backward.png"),tr("Backward"),this);
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

    m_statusBar = new QStatusBar;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_toolBar);
    mainLayout->addWidget(m_textBrowser);
    mainLayout->addWidget(m_statusBar);
    m_widget->setLayout(mainLayout);

    connect(m_textBrowser,SIGNAL(highlighted(QUrl)),this,SIGNAL(highlighted(QUrl)));
    connect(m_textBrowser,SIGNAL(anchorClicked(QUrl)),this,SLOT(anchorClicked(QUrl)));
    connect(m_backwardAct,SIGNAL(triggered()),this,SLOT(backward()));
    connect(m_forwardAct,SIGNAL(triggered()),this,SLOT(forward()));
    connect(m_reloadUrlAct,SIGNAL(triggered()),this,SLOT(reloadUrl()));
    connect(m_urlComboBox,SIGNAL(activated(QString)),this,SLOT(activatedUrl(QString)));
    connect(this,SIGNAL(backwardAvailable(bool)),m_backwardAct,SLOT(setEnabled(bool)));
    connect(this,SIGNAL(forwardAvailable(bool)),m_forwardAct,SLOT(setEnabled(bool)));

    m_extension->addObject("LiteApi.IDocumentBrowser",this);
    m_extension->addObject("LiteApi.QTextBrowser",m_textBrowser);
    m_textBrowser->installEventFilter(m_liteApp->editorManager());

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

    QStringList paths = m_textBrowser->searchPaths();
    paths << info.absolutePath();
    m_textBrowser->setSearchPaths(paths);

    QByteArray ba = file.readAll();

    if (htmlType == "text/html") {
        QTextCodec *codec = QTextCodec::codecForHtml(ba);
        setUrlHtml(QUrl::fromLocalFile(fileName),codec->toUnicode(ba));
    } else if (htmlType == "text/markdown") {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QByteArray out = mdtohtml(ba);
        setUrlHtml(QUrl::fromLocalFile(fileName),codec->toUnicode(out),true);
    } else {
        QTextCodec *codec = QTextCodec::codecForLocale();
        setUrlHtml(QUrl::fromLocalFile(fileName),codec->toUnicode(ba),false);
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

void DocumentBrowser::executeAction(const QString &id, QAction *action)
{

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
    m_textBrowser->setSearchPaths(paths);
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

void DocumentBrowser::setUrlHtml(const QUrl &url,const QString &data)
{
    setUrlHtml(url,data,false);
    emit documentLoaded();
}

QToolBar *DocumentBrowser::toolBar()
{
    return m_toolBar;
}

QStatusBar *DocumentBrowser::statusBar()
{
    return m_statusBar;
}

QComboBox *DocumentBrowser::urlComboBox()
{
    return m_urlComboBox;
}

QTextBrowser *DocumentBrowser::textBrowser()
{
    return m_textBrowser;
}

void DocumentBrowser::scrollToAnchor(const QString &text)
{
    const HistoryEntry &historyEntry = createHistoryEntry();

    m_url.setFragment(text);

    if (text.isEmpty()) {
        m_textBrowser->horizontalScrollBar()->setValue(0);
        m_textBrowser->verticalScrollBar()->setValue(0);
    } else {
        m_textBrowser->scrollToAnchor(text);
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

void DocumentBrowser::setUrlHtml(const QUrl &url,const QString &data,bool html)
{
    const HistoryEntry &historyEntry = createHistoryEntry();
    if (!data.isEmpty()) {
        if (html) {
            m_textBrowser->setHtml(data);
        } else {
            m_textBrowser->setText(data);
        }
    }
    m_url = url;
    if (!url.fragment().isEmpty()) {
        m_textBrowser->scrollToAnchor(url.fragment());
    } else {
        m_textBrowser->horizontalScrollBar()->setValue(0);
        m_textBrowser->verticalScrollBar()->setValue(0);
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
    entry.hpos = m_textBrowser->horizontalScrollBar()->value();
    entry.vpos = m_textBrowser->verticalScrollBar()->value();
    return entry;
}

void DocumentBrowser::restoreHistoryEntry(const HistoryEntry &entry)
{
    m_url = entry.url;
    m_textBrowser->horizontalScrollBar()->setValue(entry.hpos);
    m_textBrowser->verticalScrollBar()->setValue(entry.vpos);
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

void DocumentBrowser::anchorClicked(QUrl url)
{
    QString text = url.toString(QUrl::RemoveFragment);
    if (text.isEmpty() && !url.fragment().isEmpty()) {
        m_textBrowser->scrollToAnchor(url.fragment());
        return;
    }
    emit requestUrl(url);
}
