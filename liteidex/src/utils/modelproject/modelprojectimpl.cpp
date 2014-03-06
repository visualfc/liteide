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
// Module: modelprojectimpl.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "modelprojectimpl.h"
#include "modelfileimpl.h"
#include "fileutil/fileutil.h"

#include <QVBoxLayout>
#include <QFileInfo>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
#include <QToolBar>
#include <QFileInfo>
#include <QDir>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


ModelProjectImpl::ModelProjectImpl(LiteApi::IApplication *app)
    : m_liteApp(app)
{
    m_widget = new QWidget(m_liteApp->mainWindow());
    m_tree = new QTreeView;
    m_tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tree->setHeaderHidden(true);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_tree);
    m_widget->setLayout(layout);

    connect(m_tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedTree(QModelIndex)));
    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
}

ModelProjectImpl::~ModelProjectImpl()
{
    if (m_widget) {
        delete m_widget;
    }
   // m_liteApp->dockManager()->removeDock(m_widget);
}

void ModelProjectImpl::load()
{
    //m_liteApp->dockManager()->addDock(m_widget,tr("Project - ")+displayName());
}

QWidget *ModelProjectImpl::widget()
{
    return m_widget;
}

QString ModelProjectImpl::name() const
{
    return QFileInfo(m_file->filePath()).fileName();
}

QIcon ModelProjectImpl::icon() const
{
    return QIcon();
}

LiteApi::IFile *ModelProjectImpl::file()
{
    return m_file;
}

QString ModelProjectImpl::filePath() const
{
    if (m_file) {
        return m_file->filePath();
    }
    return QString();
}

QString ModelProjectImpl::mimeType() const
{
    if (m_file) {
        return m_file->mimeType();
    }
    return QString();
}


void ModelProjectImpl::setModelFile(ModelFileImpl *file)
{
    if (m_file == file) {
        return;
    }
    m_file = file;
    m_tree->setModel(m_file->model());
}

bool ModelProjectImpl::open(const QString &fileName, const QString &mimeType)
{
    bool success =  m_file->open(fileName,mimeType);
    if (success) {
        m_file->updateModel();
        m_tree->expandAll();
    }
    return success;
}

QStringList ModelProjectImpl::folderList() const
{
    QFileInfo info(m_file->filePath());
    return QStringList() << info.path();
}

QStringList ModelProjectImpl::fileNameList() const
{
    return m_file->fileNameList();
}

QStringList ModelProjectImpl::filePathList() const
{
    return m_file->filePathList();
}

QString ModelProjectImpl::fileNameToFullPath(const QString &fileName)
{
    return m_file->fileNameToFullPath(fileName);
}

QMap<QString,QString> ModelProjectImpl::targetInfo() const
{
    return m_file->targetInfo();
}

void ModelProjectImpl::editorSaved(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }

    if (FileUtil::compareFile(editor->filePath(),m_file->filePath())) {
        bool success = m_file->reload();
        if (success) {
            m_file->updateModel();
            m_tree->expandAll();
            emit reloaded();
        }
    }
}

void ModelProjectImpl::doubleClickedTree(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QStandardItem *item = m_file->model()->itemFromIndex(index);
    if (!item) {
        return;
    }
    bool ok;
    int itemType = item->data().toInt(&ok);
    if (ok && ( itemType == ModelFileImpl::ItemFile ||
               itemType == ModelFileImpl::ItemProFile ) ) {
        QString fileName = m_file->fileNameToFullPath(item->text());
        if (!fileName.isEmpty()) {
            m_liteApp->fileManager()->openEditor(fileName);
        }
    }
}
