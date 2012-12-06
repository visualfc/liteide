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
// Module: filepathmodel.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef FILEPATHMODEL_H
#define FILEPATHMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QIcon>
#include <QFileInfo>
#include <QDir>

class FilePathModel;
class PathNode
{
public:
    PathNode(FilePathModel *model);
    PathNode(FilePathModel *model,const QString &path, PathNode *parent);
    ~PathNode();
    PathNode* parent();
    PathNode* child(int row);
    int childCount();
    int row() const;
    QList<PathNode*>* children();
    QString path() const;
    QString text() const;
    QFileInfo fileInfo() const;
    bool isDir() const;
    bool isFile() const;
    void clear();
    void reload();
    PathNode *findPath(const QString &path);
protected:
    FilePathModel *m_model;
    PathNode *m_parent;
    QList<PathNode*> *m_children;
    QString m_path;
    QString m_text;
};

class QFileIconProvider;
class QFileSystemWatcher;
class QTreeView;
class FilePathModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit FilePathModel(QObject *parent = 0);
    ~FilePathModel();
    void setFilter(QDir::Filters filters);
    void setSort(QDir::SortFlags flags);
    QDir::Filters filter() const;
    QDir::SortFlags sort() const;
    void setRootPathList(const QStringList &pathList);
    void setRootPath(const QString &path);
    QList<QModelIndex> findPaths(const QString &path) const;
    QModelIndex findPath(const QString &path) const;
    QString filePath(const QModelIndex &index) const;
    PathNode *nodeFromIndex(const QModelIndex &index) const;
    void setStartIndex(const QModelIndex &index);
    void setStartPath(const QString &path);
    QModelIndex startIndex() const;
    QString startPath() const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    QFileSystemWatcher* fileWatcher() const;
public slots:
    void directoryChanged(const QString&);
protected:
    QModelIndex findPathHelper(const QString &path, const QModelIndex &parentIndex) const;
    QStringList m_pathList;
    PathNode *m_rootNode;
    QString   m_startPath;
    QFileIconProvider *m_iconProvider;
    QFileSystemWatcher *m_fileWatcher;
    QTreeView *m_treeView;
    QDir::Filters m_filters;
    QDir::SortFlags m_sorts;
};

#endif // FILEPATHMODEL_H
