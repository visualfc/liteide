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
// Module: quickopenfiles.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenfiles.h"
#include "quickopen_global.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QApplication>
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

QuickOpenFiles::QuickOpenFiles(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_filesModel = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_matchCase = Qt::CaseInsensitive;
    m_maxCount = 100000;
    m_thread = new FindFilesThread(this);
    connect(m_thread,SIGNAL(findResult(QStringList)),this,SLOT(findResult(QStringList)));
}

QuickOpenFiles::~QuickOpenFiles()
{
    if (m_thread) {
        m_thread->stop();
        delete m_thread;
    }
}

QString QuickOpenFiles::id() const
{
    return "quickopen/files";
}

QString QuickOpenFiles::info() const
{
    return tr("Go to File");
}

QString QuickOpenFiles::placeholderText() const
{
    return tr("Type '?' to get help on the actions you can take from here");
}

void QuickOpenFiles::activate()
{
}

QAbstractItemModel *QuickOpenFiles::model() const
{
    return m_proxyModel;
}

void QuickOpenFiles::updateModel()
{
    m_maxCount = m_liteApp->settings()->value(QUICKOPEN_FILES_MAXCOUNT,100000).toInt();
    m_matchCase = m_liteApp->settings()->value(QUICKOPNE_FILES_MATCHCASE,false).toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    m_model->clear();
    m_filesModel->clear();
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterFixedString("");
    m_proxyModel->setFilterKeyColumn(2);
    m_proxyModel->setFilterCaseSensitivity(m_matchCase);

    m_editors.clear();
    QStringList names;
    foreach(LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
        if (editor->filePath().isEmpty()) {
            continue;
        }
        names.push_back(editor->name()+";"+editor->filePath());
        m_editors.push_back(editor->filePath());
    }
    qSort(names);
    foreach (QString text, names) {
        QStringList ar = text.split(";");
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem("*") << new QStandardItem(ar[0]) << new QStandardItem(ar[1]));
        m_filesModel->appendRow(QList<QStandardItem*>() << new QStandardItem("*") << new QStandardItem(ar[0]) << new QStandardItem(ar[1]));
    }

    startFindThread();
}

void QuickOpenFiles::startFindThread()
{
    QSet<QString> extSet;
    foreach(LiteApi::IMimeType* type, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        foreach (QString ext, type->allPatterns()) {
            if (ext.startsWith(".")) {
                extSet << ext.mid(1);
            } else if (ext.startsWith("*.")) {
                extSet << ext.mid(2);
            }
        }
    }

    int count = m_model->rowCount();
    int maxcount = count+m_liteApp->settings()->value(QUICKOPEN_FILES_MAXCOUNT,100000).toInt();
    QSet<QString> editorSet = m_editors.toSet();

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    QStringList folderList;
    if (editor && !editor->filePath().isEmpty()) {
        folderList << QFileInfo(editor->filePath()).path();
    }
    folderList << m_liteApp->fileManager()->folderList();

    m_thread->setFolderList(folderList,extSet,editorSet,maxcount);

    m_thread->stop();
    m_thread->start();
}

void QuickOpenFiles::findResult(const QStringList &fileList)
{
    foreach (QString filePath, fileList) {
        m_filesModel->appendRow(QList<QStandardItem*>() << new QStandardItem("f") << new QStandardItem(QFileInfo(filePath).fileName()) << new QStandardItem(filePath));
    }
}

QModelIndex QuickOpenFiles::filterChanged(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);
    if (!text.isEmpty()) {
        m_proxyModel->setSourceModel(m_filesModel);
    } else {
        m_proxyModel->setSourceModel(m_model);
    }

    for(int i = 0; i < m_proxyModel->rowCount(); i++) {
        QModelIndex index = m_proxyModel->index(i,1);
        QString name = index.data().toString();
        if (name.startsWith(text,m_matchCase)) {
            return index;
        }
    }
    if (m_proxyModel->rowCount() > 0)
        return m_proxyModel->index(0,0);
    return QModelIndex();
}

void QuickOpenFiles::indexChanged(const QModelIndex &/*index*/)
{
}

bool QuickOpenFiles::selected(const QString &/*text*/, const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QString filePath = m_proxyModel->index(index.row(),2).data().toString();
    if (!m_liteApp->fileManager()->openFile(filePath)) {
        return false;
    }
    return true;
}

void QuickOpenFiles::cancel()
{
    m_thread->stop();
}
