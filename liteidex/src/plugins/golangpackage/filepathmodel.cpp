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
// Module: filepathmodel.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filepathmodel.h"

#include <QFileInfo>
#include <QDir>
#include <QIcon>
#include <QFont>
#include <QFileIconProvider>
#include <QFileSystemWatcher>
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

PathNode::PathNode(FilePathModel *model) :
    m_model(model),
    m_parent(0),
    m_children(0)
{

}

PathNode::PathNode(FilePathModel *model, const QString &path, PathNode *parent) :
    m_model(model),
    m_parent(parent),
    m_children(0),
    m_path(path)
{
    QFileInfo info(path);
    if (parent && parent->parent() == 0) {
        m_text = info.filePath();
    } else {
        m_text = info.fileName();
    }
    if (info.isDir() && !m_path.isEmpty()) {
        m_model->fileWatcher()->addPath(m_path);
    }
}

PathNode::~PathNode()
{
    if (this->isDir() && !m_path.isEmpty())  {
        m_model->fileWatcher()->removePath(m_path);
    }
    if (m_children) {
        qDeleteAll(m_children->begin(),m_children->end());
        delete m_children;
    }
}

QList<PathNode*>* PathNode::children()
{
    if (m_children == 0) {
        m_children = new QList<PathNode*>();
        if (!m_path.isEmpty()) {
            QFileInfo info(m_path);
            if (info.isDir()) {
                QDir dir(m_path);
                foreach(QFileInfo childInfo, dir.entryInfoList(this->m_model->filter(),this->m_model->sort())) {
                    m_children->append(new PathNode(this->m_model,childInfo.filePath(),this));
                }
            }
        }
    }
    return m_children;
}

PathNode* PathNode::parent()
{
    return m_parent;
}

PathNode* PathNode::child(int row)
{
    return children()->at(row);
}

int PathNode::childCount()
{
    return children()->count();
}

int PathNode::row() const
{
    if (m_parent) {
        return m_parent->children()->indexOf(const_cast<PathNode*>(this));
    }
    return 0;
}

QString PathNode::path() const
{
    return m_path;
}

QString PathNode::text() const
{
    return m_text;
}

bool PathNode::isDir() const
{
    return QFileInfo(m_path).isDir();
}

bool PathNode::isFile() const
{
    return QFileInfo(m_path).isFile();
}

QFileInfo PathNode::fileInfo() const
{
    return QFileInfo(m_path);
}

void PathNode::clear()
{
    if (m_children) {
        qDeleteAll(m_children->begin(),m_children->end());
        m_children->clear();
    }
}

void PathNode::reload()
{
    clear();
    if (m_children == 0) {
        m_children = new QList<PathNode*>();
    }
    if (!m_path.isEmpty()) {
        QFileInfo info(m_path);
        if (info.isDir()) {
            QDir dir(m_path);
            foreach(QFileInfo childInfo, dir.entryInfoList(this->m_model->filter(),this->m_model->sort())) {
                m_children->append(new PathNode(this->m_model,childInfo.filePath(),this));
            }
        }
    }
}

PathNode *PathNode::findPath(const QString &path)
{
    if (!path.startsWith(m_path)) {
        return 0;
    }
    if (path == m_path) {
        return this;
    }
    QStringList nameList = path.right(path.length()-m_path.length()).split("/",QString::SkipEmptyParts);
    PathNode *parent = this;
    bool find = false;
    foreach (QString name,nameList) {
        find = false;
        QList<PathNode*>* chilren = parent->children();
        for (int i = 0; i < chilren->count(); i++) {
            PathNode *node = chilren->at(i);
            if (!node->isDir()) {
                continue;
            }
            if (node->m_text == name) {
                parent = node;
                find = true;
                break;
            }
        }
        if (!find) {
            return 0;
        }
    }
    return parent;
}


FilePathModel::FilePathModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_rootNode(new PathNode(this)),
    m_iconProvider(new QFileIconProvider),
    m_fileWatcher(new QFileSystemWatcher(this))
{
    m_filters = QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot;
    m_sorts = QDir::DirsFirst | QDir::Name | QDir::Type;
    connect(m_fileWatcher,SIGNAL(directoryChanged(QString)),this,SLOT(directoryChanged(QString)));
}

FilePathModel::~FilePathModel()
{
    delete m_rootNode;
    delete m_iconProvider;
}

void FilePathModel::directoryChanged(const QString &path)
{
    foreach(QModelIndex index,this->findPaths(path)) {
        PathNode *node = nodeFromIndex(index);
        this->beginRemoveRows(index,0,this->rowCount(index)-1);
        node->reload();
        this->endRemoveRows();
    }
}

PathNode *FilePathModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<PathNode*>(index.internalPointer());
    }
    return m_rootNode;
}

void FilePathModel::setStartIndex(const QModelIndex &index)
{
    m_startPath = filePath(index);
}

void FilePathModel::setStartPath(const QString &path)
{
    m_startPath = path;
}

QModelIndex FilePathModel::startIndex() const
{
    return findPath(m_startPath);
}

QString FilePathModel::startPath() const
{
    return m_startPath;
}

QString FilePathModel::filePath(const QModelIndex &index) const
{
    return nodeFromIndex(index)->path();
}

void FilePathModel::setFilter(QDir::Filters filters)
{
    if (m_filters != filters) {
        m_filters = filters;
        m_rootNode->reload();
    }
}

void FilePathModel::setSort(QDir::SortFlags flags)
{
    if (m_sorts != flags) {
        m_sorts = flags;
        m_rootNode->reload();
    }
}

QDir::Filters FilePathModel::filter() const
{
    return m_filters;
}

QDir::SortFlags FilePathModel::sort() const
{
    return m_sorts;
}

void FilePathModel::setRootPath(const QString &path)
{
    this->setRootPathList(QStringList() << path);
    this->setStartPath(path);
}

void FilePathModel::setRootPathList(const QStringList &pathList)
{
    this->beginResetModel();
    m_rootNode->clear();
    m_pathList.clear();
    foreach(QString path, pathList) {
        QString spath = QDir::fromNativeSeparators(QDir::cleanPath(path));
        m_pathList.append(spath);
        m_rootNode->children()->append(new PathNode(this,spath,m_rootNode));
    }

    if (m_startPath.isEmpty() && !pathList.isEmpty()) {
        m_startPath = pathList.first();
    }

    this->endResetModel();
}

QModelIndex FilePathModel::findPathHelper(const QString &path, const QModelIndex &parentIndex) const
{
    PathNode *node = nodeFromIndex(parentIndex);
    if (!path.startsWith(node->path())) {
        return QModelIndex();
    }
    if (path == node->path()) {
        return parentIndex;
    }
    QStringList nameList = path.right(path.length()-node->path().length()).split("/",QString::SkipEmptyParts);
    QModelIndex parent = parentIndex;
    bool find = false;
    int count = nameList.count();
    for (int i = 0; i < count; i++) {
        find = false;
        for (int j = 0; j < this->rowCount(parent); j++) {
            QModelIndex index = this->index(j,0,parent);
            PathNode *node = nodeFromIndex(index);
            if ( ( (i == count-1) || node->isDir()) && node->text() == nameList.at(i)) {
                parent = index;
                find = true;
                break;
            }
        }
        if (!find) {
            return QModelIndex();
        }
    }
    return parent;
}

QList<QModelIndex> FilePathModel::findPaths(const QString &path) const
{
    QList<QModelIndex> list;
    QString cpath = QDir::fromNativeSeparators(QDir::cleanPath(path));
    for (int i = 0; i < this->rowCount(); i++) {
        QModelIndex find = findPathHelper(cpath,this->index(i,0));
        if (find.isValid()) {
            list.append(find);
        }
     }
    return list;
}

QModelIndex FilePathModel::findPath(const QString &path) const
{
    QList<QModelIndex> list = this->findPaths(path);
    if (!list.isEmpty()) {
        return list.last();
    }
    return QModelIndex();
}

int FilePathModel::rowCount(const QModelIndex &parent) const
{
    PathNode *node = nodeFromIndex(parent);
    return node->childCount();
}

int FilePathModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QModelIndex FilePathModel::parent(const QModelIndex &child) const
{
    PathNode *node = nodeFromIndex(child);
    PathNode *parent = node->parent();
    if (parent == m_rootNode) {
        return QModelIndex();
    }
    return createIndex(parent->row(),0,parent);
}

QModelIndex FilePathModel::index(int row, int column,const QModelIndex &parent) const
{
    if (!hasIndex(row,column,parent))
        return QModelIndex();
    PathNode *node = nodeFromIndex(parent);
    return createIndex(row,column,node->child(row));
}

QVariant FilePathModel::data(const QModelIndex &index, int role) const
{
    PathNode *node = nodeFromIndex(index);
    if (!node) {
        return QVariant();
    }
    switch(role) {
    case Qt::DisplayRole:
        return node->text();
    case Qt::DecorationRole:
        return m_iconProvider->icon(node->fileInfo());
    case Qt::FontRole: {
        QFont font;
        if (node->path() == m_startPath) {
            font.setBold(true);
        }
        return font;
    }
    }
    return QVariant();
}

QFileSystemWatcher* FilePathModel::fileWatcher() const
{
    return m_fileWatcher;
}
