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

void updateFolder(QString folder, QStringList extFilter, QStandardItemModel *model, int maxcount)
{
    if (model->rowCount() > maxcount) {
        return;
    }
    QDir dir(folder);
    foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)) {
        if (info.isDir()) {
            updateFolder(info.filePath(),extFilter,model,maxcount);
        } else if (info.isFile()) {
            if (extFilter.contains(info.suffix())) {
                model->appendRow(QList<QStandardItem*>() << new QStandardItem("f") << new QStandardItem(info.fileName()) << new QStandardItem(info.filePath()));
            }
        }
    }
}

void QuickOpenFiles::updateModel()
{
    m_model->clear();
    m_proxyModel->setFilterFixedString("");
    m_proxyModel->setFilterKeyColumn(2);

    QStringList editors;
    foreach(LiteApi::IEditor *editor, m_liteApp->editorManager()->editorList()) {
        if (editor->filePath().isEmpty()) {
            continue;
        }
        editors.push_back(editor->name()+";"+editor->filePath());
    }
    qSort(editors);
    foreach (QString text, editors) {
        QStringList ar = text.split(";");
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem("*") << new QStandardItem(ar[0]) << new QStandardItem(ar[1]) );
    }

    QStringList extFilter;
    foreach(LiteApi::IMimeType* type, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        foreach (QString ext, type->globPatterns()) {
            if (ext.startsWith(".")) {
                extFilter << ext.mid(1);
            } else if (ext.startsWith("*.")) {
                extFilter << ext.mid(2);
            }
        }
    }

    int count = m_model->rowCount();
    int maxcount = count+m_liteApp->settings()->value(QUICKOPEN_FILES_MAXCOUNT,100000).toInt();
    foreach(QString folder, m_liteApp->fileManager()->folderList()) {
        updateFolder(folder,extFilter,m_model,maxcount);
    }
    //m_proxyModel->sort(0);
}

QModelIndex QuickOpenFiles::filterChanged(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);

    for(int i = 0; i < m_proxyModel->rowCount(); i++) {
        QModelIndex index = m_proxyModel->index(i,1);
        QString name = index.data().toString();
        if (name.startsWith(text)) {
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

bool QuickOpenFiles::selected(const QString &text, const QModelIndex &index)
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
