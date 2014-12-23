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
// Module: filesystemmodel.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QIcon>
#include <QFileInfo>
#include <QDir>
#include <QMutex>

class FileSystemModel;
class QFileSystemWatcher;
class FileNode
{
public:
    FileNode(FileSystemModel *model);
    FileNode(FileSystemModel *model,const QString &path, FileNode *parent);
    virtual ~FileNode();
    FileNode* parent();
    FileNode* child(int row);
    int childCount();
    int row() const;
    QList<FileNode*>* children();
    QString path() const;
    QString text() const;
    QFileInfo fileInfo() const;
    bool isExist() const;
    bool isDir() const;
    bool isFile() const;
    void clear();
    void reload();
    FileNode *findPath(const QString &path);
protected:
    FileSystemModel *m_model;
    FileNode *m_parent;
    QList<FileNode*> *m_children;
    QString m_path;
    QString m_text;
    bool    m_bWatcher;
};

class QFileIconProvider;
class QFileSystemWatcher;
class QTreeView;
class FileSystemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit FileSystemModel(QObject *parent = 0);
    ~FileSystemModel();
    void clear();
    void reload();
    void setFilter(QDir::Filters filters);
    void setDirSort(QDir::SortFlags flags);
    QDir::Filters filter() const;
    bool isShowHideFiles() const;
    QDir::SortFlags dirSort() const;
    bool removeRootPath(const QString &path);
    bool addRootPath(const QString &path);
    void setRootPathList(const QStringList &pathList);
    void setRootPath(const QString &path);
    QStringList rootPathList() const;
    QList<QModelIndex> findPaths(const QString &path) const;
    QModelIndex findPath(const QString &path) const;
    QString filePath(const QModelIndex &index) const;
    FileNode *nodeFromIndex(const QModelIndex &index) const;
    void setStartIndex(const QModelIndex &index);
    void setStartPath(const QString &path);
    QModelIndex startIndex() const;
    QString startPath() const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    bool isRootPathNode(FileNode *node) const;
    bool isRootPathNodeFillPath() const;
    void addWatcher(const QString &path);
    void removeWatcher(const QString &path);
signals:
    void direcotryChanged(QString);
public slots:
    void reloadDirectory(const QString &path);
protected:
    QModelIndex findPathHelper(const QString &path, const QModelIndex &parentIndex) const;
    QStringList m_pathList;
    FileNode *m_rootNode;    
    QString   m_startPath;
    QFileIconProvider *m_iconProvider;
    QFileSystemWatcher *m_fileWatcher;
    QMap<QString,int> m_fileWatcherMap;
    QTreeView *m_treeView;
    QDir::Filters m_filters;
    QDir::SortFlags m_sorts;
    QMutex m_mutex;
};

#endif // FILESYSTEMMODEL_H
