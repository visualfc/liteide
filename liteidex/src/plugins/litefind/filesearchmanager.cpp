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
// Module: filesearchmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filesearchmanager.h"
#include "qtc_searchresult/searchresultcolor.h"
#include "replacedocument.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

FileSearchManager::FileSearchManager(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IFileSearchManager(parent), m_liteApp(app)
{
    m_currentSearch = 0;
    m_widget = new QStackedWidget;

    QHBoxLayout *hbox = new QHBoxLayout;
    QLabel *label = new QLabel(tr("Search Item:"));
    m_searchItemCombox = new QComboBox;
    hbox->addWidget(label);
    hbox->addWidget(m_searchItemCombox);
    hbox->addStretch();

    QVBoxLayout *vbox = new QVBoxLayout;
    m_searchItemStackedWidget = new QStackedWidget;
    vbox->addLayout(hbox);
    vbox->addWidget(m_searchItemStackedWidget);
    vbox->addStretch();

    m_searchWidget = new QWidget;
    m_searchWidget->setLayout(vbox);;

    m_searchResultWidget = new Find::Internal::SearchResultWidget(m_widget);
    m_searchResultWidget->setAutoExpandResults(true);

    QPalette pal = m_searchResultWidget->palette();
    Find::Internal::SearchResultColor color;
    color.textForeground = pal.color(QPalette::Text);
    color.textBackground = pal.color(QPalette::Base);
    color.highlightForeground = pal.color(QPalette::Text);
    color.highlightBackground = QColor(255,239,11);//pal.color(QPalette::ToolTipBase);
    QFont font = m_searchWidget->font();
    m_searchResultWidget->setTextEditorFont(font,color);

    m_widget->addWidget(m_searchWidget);
    m_widget->addWidget(m_searchResultWidget);

    QAction *newSearch = new QAction(tr("New Search"),this);

    m_toolAct = m_liteApp->toolWindowManager()->addToolWindow(Qt::BottomDockWidgetArea,
                                                  m_widget,"searchresult",tr("Search Result"),true,
                                                  QList<QAction*>() << newSearch);
    m_liteApp->extension()->addObject("LiteApi.IFileSearchManager",this);

    connect(newSearch,SIGNAL(triggered()),this,SLOT(newSearch()));
    connect(m_searchItemCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(currentSearchItemChanged(int)));
    connect(m_searchResultWidget,SIGNAL(activated(Find::SearchResultItem)),this,SLOT(activated(Find::SearchResultItem)));
    connect(m_searchResultWidget,SIGNAL(replaceButtonClicked(QString,QList<Find::SearchResultItem>,bool)),this,SLOT(doReplace(QString,QList<Find::SearchResultItem>,bool)));
}

FileSearchManager::~FileSearchManager()
{
    foreach(LiteApi::IFileSearch *search, m_fileSearchList) {
        delete search;
    }
    m_fileSearchList.clear();
    delete m_widget;
}

void FileSearchManager::addFileSearch(LiteApi::IFileSearch *search)
{
    m_fileSearchList.push_back(search);
    if (search->widget()) {
        m_searchItemStackedWidget->addWidget(search->widget());
        m_searchItemCombox->addItem(search->displayName(),search->mimeType());
    }
    connect(search,SIGNAL(findStarted()),this,SLOT(findStarted()));
    connect(search,SIGNAL(findFinished(bool)),this,SLOT(findFinished(bool)));
    connect(search,SIGNAL(findResult(LiteApi::FileSearchResult)),this,SLOT(findResult(LiteApi::FileSearchResult)));
}

LiteApi::IFileSearch *FileSearchManager::findFileSearch(const QString &mime)
{
    foreach (LiteApi::IFileSearch *i, m_fileSearchList) {
        if (i->mimeType() == mime) {
            return i;
        }
    }
    return 0;
}

QList<LiteApi::IFileSearch *> FileSearchManager::fileSearchList() const
{
    return m_fileSearchList;
}

void FileSearchManager::setCurrentSearch(LiteApi::IFileSearch *search)
{
    m_currentSearch = search;
    if (m_currentSearch->widget()) {
        m_searchItemStackedWidget->setCurrentWidget(search->widget());
    }
    m_searchResultWidget->setShowReplaceUI(m_currentSearch->replaceMode());
    m_currentSearch->activate();
}

void FileSearchManager::activated(const Find::SearchResultItem &item)
{
    if (item.path.isEmpty()) {
        return;
    }
    QString fileName = item.path.at(0);
    LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName,true);
    if (!editor) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::getTextEditor(editor);
    if (!textEditor) {
        return;
    }
    textEditor->gotoLine(item.lineNumber-1,item.textMarkPos,false);
}

void FileSearchManager::newSearch()
{
    m_toolAct->setChecked(true);
    m_widget->setCurrentWidget(m_searchWidget);
    currentSearchItemChanged(0);
}

void FileSearchManager::currentSearchItemChanged(int item)
{
    QString mimeType = m_searchItemCombox->itemData(item,Qt::UserRole).toString();
    LiteApi::IFileSearch *search = this->findFileSearch(mimeType);
    if (search) {
        setCurrentSearch(search);
    }
}

void FileSearchManager::findStarted()
{
    m_toolAct->setChecked(true);
    m_widget->setCurrentWidget(m_searchResultWidget);
    m_searchResultWidget->restart();
    m_searchResultWidget->setInfo(m_currentSearch->displayName()+":",QString(),m_currentSearch->searchText());
}

void FileSearchManager::findFinished(bool)
{
    m_searchResultWidget->finishSearch(false);
}

void FileSearchManager::findResult(const LiteApi::FileSearchResult &result)
{
    m_searchResultWidget->addResult(result.path,
                                    result.line,
                                    result.lineText,
                                    result.col,
                                    result.len);
}

void FileSearchManager::doReplace(const QString &text, const QList<Find::SearchResultItem> &items, bool preserveCase)
{
    if (items.isEmpty()) {
        return;
    }
    QHash<QString, QList<Find::SearchResultItem> > changes;
    foreach (const Find::SearchResultItem &item, items)
        changes[QDir::fromNativeSeparators(item.path.first())].append(item);

    // Checking for files without write permissions
    QHashIterator<QString, QList<Find::SearchResultItem> > it(changes);
    QStringList roFiles;
    while (it.hasNext()) {
        it.next();
        const QFileInfo fileInfo(it.key());
        if (!fileInfo.isWritable()) {
            roFiles.append(it.key());
        }
    }
    if (!roFiles.isEmpty()) {
        QString text = tr("The following files have no write permissions. Do you want to change the permissions?");
        text += "\n";
        text += roFiles.join("\n");
        int ret = QMessageBox::warning(m_liteApp->mainWindow(),tr("File is readonly"),text,QMessageBox::Yes|QMessageBox::Cancel,QMessageBox::Cancel);
        if (ret != QMessageBox::Yes) {
            return;
        }
        foreach(QString fileName, roFiles) {
            QFile::setPermissions(fileName, QFile::permissions(fileName) | QFile::WriteUser);
        }
    }
    it.toFront();
    while(it.hasNext()) {
        it.next();
        ReplaceDocument doc;
        doc.replace(it.key(),text,it.value());
    }
}
