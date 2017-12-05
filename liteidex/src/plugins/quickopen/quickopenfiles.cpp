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
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_matchCase = Qt::CaseInsensitive;
    m_maxCount = 100000;
}

QString QuickOpenFiles::id() const
{
    return "quickopen/files";
}

QString QuickOpenFiles::info() const
{
    return tr("Open Files by Name");
}

void QuickOpenFiles::activate()
{

}

QAbstractItemModel *QuickOpenFiles::model() const
{
    return m_proxyModel;
}

void updateFolder(QString folder, QStandardItemModel *model, int maxcount, QSet<QString> *extSet, QSet<QString> *folderSet, QSet<QString> *editorSet)
{
    if (model->rowCount() >= maxcount) {
        return;
    }
    if (folderSet->contains(folder)) {
        return;
    }
    folderSet->insert(folder);
    qApp->processEvents();
    QDir dir(folder);
    foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
        if (info.isDir()) {
            updateFolder(info.filePath(),model,maxcount,extSet,folderSet,editorSet);
        } else if (info.isFile()) {
            if (extSet->contains(info.suffix()) && !editorSet->contains(info.filePath()) ) {
                model->appendRow(QList<QStandardItem*>() << new QStandardItem("f") << new QStandardItem(info.fileName()) << new QStandardItem(info.filePath()));
            }
        }
    }
}

void QuickOpenFiles::updateModel()
{
    m_maxCount = m_liteApp->settings()->value(QUICKOPEN_FILES_MAXCOUNT,100000).toInt();
    m_matchCase = m_liteApp->settings()->value(QUICKOPNE_FILES_MATCHCASE,false).toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    m_model->clear();
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
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem("*") << new QStandardItem(ar[0]) << new QStandardItem(ar[1]) );
    }
    QTimer::singleShot(1,this,SLOT(updateFiles()));
}

void QuickOpenFiles::updateFiles()
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
    QSet<QString> folderSet;
    QSet<QString> editorSet = m_editors.toSet();

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor && !editor->filePath().isEmpty()) {
        QString folder = QFileInfo(editor->filePath()).path();
        updateFolder(folder,m_model,maxcount, &extSet, &folderSet, &editorSet);
    }

    foreach(QString folder, m_liteApp->fileManager()->folderList()) {
        updateFolder(folder,m_model,maxcount, &extSet, &folderSet, &editorSet);
    }
}

QModelIndex QuickOpenFiles::filterChanged(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);

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
}
