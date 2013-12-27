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
// Module: filesystemmodel.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "filesystemmodel.h"

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

FileNode::FileNode(FileSystemModel *model) :
    m_model(model),
    m_parent(0),
    m_children(0)
{

}

FileNode::FileNode(FileSystemModel *model, const QString &path, FileNode *parent) :
    m_model(model),
    m_parent(parent),
    m_children(0),
    m_path(path)
{
    QFileInfo info(path);
    if (parent && parent->parent() == 0) {
        m_text = QDir::toNativeSeparators(info.filePath());
    } else {
        m_text = info.fileName();
    }
    if (info.isDir() && !m_path.isEmpty()) {
        m_model->fileWatcher()->addPath(m_path);
    }
}

FileNode::~FileNode()
{
    if (this->isDir() && !m_path.isEmpty())  {
        m_model->fileWatcher()->removePath(m_path);
    }
    if (m_children) {
        qDeleteAll(m_children->begin(),m_children->end());
        delete m_children;
    }
}

QList<FileNode*>* FileNode::children()
{
    if (m_children == 0) {
        m_children = new QList<FileNode*>();
        if (!m_path.isEmpty()) {
            QFileInfo info(m_path);
            if (info.isDir()) {
                QDir dir(m_path);
                foreach(QFileInfo childInfo, dir.entryInfoList(this->m_model->filter(),this->m_model->sort())) {
                    if (!this->m_model->isShowHideFiles() && childInfo.isDir() && childInfo.fileName().startsWith(".")) {
                       continue;
                    }
                    m_children->append(new FileNode(this->m_model,childInfo.filePath(),this));
                }
            }
        }
    }
    return m_children;
}

FileNode* FileNode::parent()
{
    return m_parent;
}

FileNode* FileNode::child(int row)
{
    return children()->at(row);
}

int FileNode::childCount()
{
    return children()->count();
}

int FileNode::row() const
{
    if (m_parent) {
        return m_parent->children()->indexOf(const_cast<FileNode*>(this));
    }
    return 0;
}

QString FileNode::path() const
{
    return m_path;
}

QString FileNode::text() const
{
    return m_text;
}

bool FileNode::isDir() const
{
    return QFileInfo(m_path).isDir();
}

bool FileNode::isFile() const
{
    return QFileInfo(m_path).isFile();
}

QFileInfo FileNode::fileInfo() const
{
    return QFileInfo(m_path);
}

void FileNode::clear()
{
    if (m_children) {
        qDeleteAll(m_children->begin(),m_children->end());
        m_children->clear();
    }
}

void FileNode::reload()
{
    clear();
    if (m_children == 0) {
        m_children = new QList<FileNode*>();
    }
    if (!m_path.isEmpty()) {
        QFileInfo info(m_path);
        if (info.isDir()) {
            QDir dir(m_path);
            foreach(QFileInfo childInfo, dir.entryInfoList(this->m_model->filter(),this->m_model->sort())) {
                if (!this->m_model->isShowHideFiles() && childInfo.isDir() && childInfo.fileName().startsWith(".")) {
                   continue;
                }
                m_children->append(new FileNode(this->m_model,childInfo.filePath(),this));
            }
        }
    }
}

FileNode *FileNode::findPath(const QString &path)
{
    if (!path.startsWith(m_path)) {
        return 0;
    }
    if (path == m_path) {
        return this;
    }
    QStringList nameList = path.right(path.length()-m_path.length()).split("/",QString::SkipEmptyParts);
    FileNode *parent = this;
    bool find = false;
    foreach (QString name,nameList) {
        find = false;
        QList<FileNode*>* chilren = parent->children();
        for (int i = 0; i < chilren->count(); i++) {
            FileNode *node = chilren->at(i);
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


FileSystemModel::FileSystemModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_rootNode(new FileNode(this)),
    m_iconProvider(new QFileIconProvider),
    m_fileWatcher(new QFileSystemWatcher(this))
{
    m_filters = QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot;
    m_sorts = QDir::DirsFirst | QDir::Name | QDir::Type;
   // connect(m_fileWatcher,SIGNAL(directoryChanged(QString)),this,SLOT(directoryChanged(QString)));
}

FileSystemModel::~FileSystemModel()
{
    delete m_rootNode;
    delete m_iconProvider;
}

void FileSystemModel::directoryChanged(const QString &path)
{
    this->beginResetModel();
    QDir dir(path);
    bool b = dir.exists();
    if (!b) {
        m_fileWatcher->removePath(path);
    }

    foreach(QModelIndex index,this->findPaths(path)) {
        FileNode *node = nodeFromIndex(index);
        if (b) {
            node->reload();
        }
        else {
            FileNode *parent = node->parent();
            if (parent) {
                parent->reload();
            }
        }
    }
    this->endResetModel();
}

FileNode *FileSystemModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<FileNode*>(index.internalPointer());
    }
    return m_rootNode;
}

void FileSystemModel::setStartIndex(const QModelIndex &index)
{
    m_startPath = filePath(index);
}

void FileSystemModel::setStartPath(const QString &path)
{
    m_startPath = path;
}

QModelIndex FileSystemModel::startIndex() const
{
    return findPath(m_startPath);
}

QString FileSystemModel::startPath() const
{
    return m_startPath;
}

QString FileSystemModel::filePath(const QModelIndex &index) const
{
    return nodeFromIndex(index)->path();
}

void FileSystemModel::setFilter(QDir::Filters filters)
{
    if (m_filters != filters) {
        m_filters = filters;
        this->reload();
    }
}

void FileSystemModel::setSort(QDir::SortFlags flags)
{
    if (m_sorts != flags) {
        m_sorts = flags;
        this->reload();
    }
}

QDir::Filters FileSystemModel::filter() const
{
    return m_filters;
}

bool FileSystemModel::isShowHideFiles() const
{
    return m_filters & QDir::Hidden;
}

QDir::SortFlags FileSystemModel::sort() const
{
    return m_sorts;
}

void FileSystemModel::clear()
{
    this->beginResetModel();
    m_rootNode->clear();
    m_pathList.clear();
    m_startPath.clear();
    this->endResetModel();
}

void FileSystemModel::reload()
{
    this->setRootPathList(this->rootPathList());
}

void FileSystemModel::setRootPath(const QString &path)
{
    this->setRootPathList(QStringList() << path);
}

bool FileSystemModel::removeRootPath(const QString &path)
{
    QString pathName = QDir::fromNativeSeparators(path);
    FileNode *node = 0;
    int index = -1;
    for (int i = 0; i < m_rootNode->childCount(); i++) {
        node = m_rootNode->children()->at(i);
        if (node && node->path() == pathName) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return false;
    }
    if (!m_pathList.removeAll(pathName)) {
        return false;
    }
    this->beginRemoveRows(QModelIndex(),index,index);
    m_rootNode->children()->removeAt(index);
    delete node;
    this->endRemoveRows();
    return true;
}


bool FileSystemModel::addRootPath(const QString &path)
{
    QString pathName = QDir::fromNativeSeparators(path);
    if (m_pathList.contains(pathName)) {
        return false;
    }
    this->beginInsertRows(QModelIndex(),m_rootNode->childCount(),m_rootNode->childCount());
    m_pathList.append(pathName);
    m_rootNode->children()->append(new FileNode(this,pathName,m_rootNode));
    this->endInsertRows();
    return true;
}

void FileSystemModel::setRootPathList(const QStringList &pathList)
{
    this->beginResetModel();
    m_rootNode->clear();
    m_pathList.clear();
    foreach (QString path, pathList) {
        m_pathList.append(QDir::fromNativeSeparators(QDir::cleanPath(path)));
    }
    m_pathList.removeDuplicates();

    foreach(QString path, m_pathList) {
        m_rootNode->children()->append(new FileNode(this,path,m_rootNode));
    }

    if (m_startPath.isEmpty() && !pathList.isEmpty()) {
        m_startPath = pathList.first();
    }

    this->endResetModel();
}

QStringList FileSystemModel::rootPathList() const
{
    return m_pathList;
}

QModelIndex FileSystemModel::findPathHelper(const QString &path, const QModelIndex &parentIndex) const
{
    FileNode *node = nodeFromIndex(parentIndex);
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
            FileNode *node = nodeFromIndex(index);
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

QList<QModelIndex> FileSystemModel::findPaths(const QString &path) const
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

QModelIndex FileSystemModel::findPath(const QString &path) const
{
    QList<QModelIndex> list = this->findPaths(path);
    if (!list.isEmpty()) {
        return list.last();
    }
    return QModelIndex();
}

int FileSystemModel::rowCount(const QModelIndex &parent) const
{
    FileNode *node = nodeFromIndex(parent);
    return node->childCount();
}

int FileSystemModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QModelIndex FileSystemModel::parent(const QModelIndex &child) const
{
    FileNode *node = nodeFromIndex(child);
    FileNode *parent = node->parent();
    if (parent == m_rootNode) {
        return QModelIndex();
    }
    return createIndex(parent->row(),0,parent);
}

QModelIndex FileSystemModel::index(int row, int column,const QModelIndex &parent) const
{
    if (!hasIndex(row,column,parent))
        return QModelIndex();
    FileNode *node = nodeFromIndex(parent);
    return createIndex(row,column,node->child(row));
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    FileNode *node = nodeFromIndex(index);
    if (!node) {
        return QVariant();
    }
    switch(role) {
    case Qt::DisplayRole:
        return node->text();
    case Qt::DecorationRole:
        return m_iconProvider->icon(node->fileInfo());
/*
    case Qt::FontRole: {
        QFont font;
        if (node->path() == m_startPath) {
            font.setBold(true);
        }
        return font;
    }
*/
    }
    return QVariant();
}

QFileSystemWatcher* FileSystemModel::fileWatcher() const
{
    return m_fileWatcher;
}

bool FileSystemModel::isRootPathNode(FileNode *node) const
{
    return node->parent() == m_rootNode;
}
