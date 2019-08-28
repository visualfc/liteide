/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE Team. All rights reserved.
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
// Module: multifoldermodel.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "multifoldermodel.h"
#include "filesystemmodelex.h"
#include <QFileSystemModel>
#include <QFileInfo>
#include <QDirModel>
#include <QDateTime>
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


/* For sorting */
struct QDirSortItem
{
    mutable QString filename_cache;
    mutable QString suffix_cache;
    QFileInfo item;
};


class QDirSortItemComparator
{
    int qt_cmp_si_sort_flags;
public:
    QDirSortItemComparator(int flags) : qt_cmp_si_sort_flags(flags) {}
    bool sort(const QDirSortItem &, const QDirSortItem &) const;
};

bool QDirSortItemComparator::sort(const QDirSortItem &n1, const QDirSortItem &n2) const
{
    const QDirSortItem* f1 = &n1;
    const QDirSortItem* f2 = &n2;

    if ((qt_cmp_si_sort_flags & QDir::DirsFirst) && (f1->item.isDir() != f2->item.isDir()))
        return f1->item.isDir();
    if ((qt_cmp_si_sort_flags & QDir::DirsLast) && (f1->item.isDir() != f2->item.isDir()))
        return !f1->item.isDir();

    int r = 0;
    int sortBy = (qt_cmp_si_sort_flags & QDir::SortByMask)
                 | (qt_cmp_si_sort_flags & QDir::Type);

    switch (sortBy) {
      case QDir::Time:
        r = f1->item.lastModified().secsTo(f2->item.lastModified());
        break;
      case QDir::Size:
          r = int(qBound<qint64>(-1, f2->item.size() - f1->item.size(), 1));
        break;
      case QDir::Type:
      {
        bool ic = qt_cmp_si_sort_flags & QDir::IgnoreCase;

        if (f1->suffix_cache.isNull())
            f1->suffix_cache = ic ? f1->item.suffix().toLower()
                               : f1->item.suffix();
        if (f2->suffix_cache.isNull())
            f2->suffix_cache = ic ? f2->item.suffix().toLower()
                               : f2->item.suffix();

        r = qt_cmp_si_sort_flags & QDir::LocaleAware
            ? f1->suffix_cache.localeAwareCompare(f2->suffix_cache)
            : f1->suffix_cache.compare(f2->suffix_cache);
      }
        break;
      default:
        ;
    }

    if (r == 0 && sortBy != QDir::Unsorted) {
        // Still not sorted - sort by name
        bool ic = qt_cmp_si_sort_flags & QDir::IgnoreCase;

        if (f1->filename_cache.isNull())
            f1->filename_cache = ic ? f1->item.fileName().toLower()
                                    : f1->item.fileName();
        if (f2->filename_cache.isNull())
            f2->filename_cache = ic ? f2->item.fileName().toLower()
                                    : f2->item.fileName();

        r = qt_cmp_si_sort_flags & QDir::LocaleAware
            ? f1->filename_cache.localeAwareCompare(f2->filename_cache)
            : f1->filename_cache.compare(f2->filename_cache);
    }
    if (r == 0) // Enforce an order - the order the items appear in the array
        r = (&n1) - (&n2);
    if (qt_cmp_si_sort_flags & QDir::Reversed)
        return r > 0;
    return r < 0;
}

MultiFolderModel::MultiFolderModel(QObject *parent)
    : MultiIndexModel(parent)
{
    m_filters = QDir::NoFilter;
    m_sorts = QDir::DirsFirst | QDir::Name;

    m_filters = QDir::AllDirs | QDir::Files | QDir::Drives
                            | QDir::Readable| QDir::Writable
                            | QDir::Executable /*| QDir::Hidden*/
                            | QDir::NoDotAndDotDot;
    m_resolveSymlinks = true;
    m_isReadOnly = true;
    m_nameFilterDisables = true;
    m_isShowDetails = true;
}

MultiFolderModel::~MultiFolderModel()
{

}

QModelIndex MultiFolderModel::addRootPath(const QString &path)
{
    if (!QDir::isAbsolutePath(path)) {
        return QModelIndex();
    }
    QFileSystemModel *model = new FileSystemModelEx(this);
    model->setFilter(m_filters);
    model->setResolveSymlinks(m_resolveSymlinks);
    model->setReadOnly(m_isReadOnly);
    model->setNameFilterDisables(m_nameFilterDisables);
    model->setNameFilters(m_nameFilters);
    connect(model,SIGNAL(directoryLoaded(QString)),this,SLOT(slotDirectoryLoaded(QString)));
    QModelIndex index = model->setRootPath(path);
    if (this->addSourceModel(model,index)) {
        return this->mapFromSourceEx(model,index);
    }
    return QModelIndex();
}

void MultiFolderModel::removeRootPath(const QString &path)
{
    QFileInfo info(path);
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        QString rootPath = ((QFileSystemModel*)model)->rootPath();
        if (QFileInfo(rootPath) == info) {
            this->removeSourceModel(model);
            delete model;
            break;
        }
    }
}

void MultiFolderModel::removeRoot(const QModelIndex &index)
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (si.model == 0) {
        return;
    }
    foreach (SourceModelIndex i, this->sourceModelIndexlList()) {
        if (i.model == si.model && i.index == si.index) {
            this->removeSourceModel(i.model);
            delete si.model;
            break;
        }
    }
}

bool MultiFolderModel::isRootPath(const QString &path) const
{
    QFileInfo info(path);
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        QString rootPath = ((QFileSystemModel*)model)->rootPath();
        if (QFileInfo(rootPath) == info) {
            return true;
        }
    }
    return false;
}

bool MultiFolderModel::isRootIndex(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (si.model == 0) {
        return false;
    }
    foreach (SourceModelIndex i, this->sourceModelIndexlList()) {
        if (i.model == si.model && i.index == si.index) {
            return true;
        }
    }
    return false;
}

void MultiFolderModel::clearAll()
{
    QList<QAbstractItemModel*> all = this->sourceModelList();
    MultiIndexModel::removeAllSourceModel();
    foreach (QAbstractItemModel *model, all) {
        delete model;
    }
}

void MultiFolderModel::reloadAll()
{
    QStringList pathList = this->rootPathList();
    this->clearAll();
    foreach (QString path, pathList) {
        this->addRootPath(path);
    }
}

QList<QModelIndex> MultiFolderModel::rootIndexs() const
{
    QList<QModelIndex> indexList;
    for (int i = 0; i < this->rowCount(); i++) {
        indexList.push_back(this->index(i,0));
    }
    return indexList;
}

QStringList MultiFolderModel::rootPathList() const
{
    QStringList pathList;
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        pathList << ((QFileSystemModel*)model)->rootPath();
    }
    return pathList;
}

QString MultiFolderModel::filePath(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return QString();
    }
    return ((QFileSystemModel*)si.model)->filePath(si.index);
}

QString MultiFolderModel::fileName(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return QString();
    }
    return ((QFileSystemModel*)si.model)->fileName(si.index);
}

QFileInfo MultiFolderModel::fileInfo(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return QFileInfo();
    }
    return ((QFileSystemModel*)si.model)->fileInfo(si.index);
}

QString MultiFolderModel::fileRootPath(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return QString();
    }
    return ((QFileSystemModel*)si.model)->rootPath();
}

bool MultiFolderModel::isDir(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return false;
    }
    return ((QFileSystemModel*)si.model)->isDir(si.index);
}

qint64 MultiFolderModel::size(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return 0;
    }
    return ((QFileSystemModel*)si.model)->size(si.index);
}

QString MultiFolderModel::type(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return "";
    }
    return ((QFileSystemModel*)si.model)->type(si.index);
}

QDateTime MultiFolderModel::lastModified(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return QDateTime();
    }
    return ((QFileSystemModel*)si.model)->lastModified(si.index);
}

QFile::Permissions MultiFolderModel::permissions(const QModelIndex &index) const
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return 0;
    }
    return ((QFileSystemModel*)si.model)->permissions(si.index);
}

bool MultiFolderModel::remove(const QModelIndex &index)
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return false;
    }
    return ((QFileSystemModel*)si.model)->remove(si.index);
}

bool MultiFolderModel::rmdir(const QModelIndex &index)
{
    SourceModelIndex si = this->mapToSourceEx(index);
    if (!si.isValid()) {
        return false;
    }
    return ((QFileSystemModel*)si.model)->rmdir(si.index);
}

QList<QModelIndex> MultiFolderModel::indexForPath(const QString &path) const
{
    QList<QModelIndex> pathList;
    QString findPath = QDir::cleanPath(QDir::fromNativeSeparators(path));
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        QFileSystemModel *fs = (QFileSystemModel*)model;
        if (!findPath.startsWith(fs->rootPath()+"/")) {
            continue;
        }
        QModelIndex index = fs->index(findPath);
        if (index.isValid()) {
            pathList << this->mapFromSourceEx(model,index);
        }
    }
    return pathList;
}

QModelIndex MultiFolderModel::indexForPath(QFileSystemModel *sourceModel, const QString &path) const
{
    QString findPath = QDir::cleanPath(QDir::fromNativeSeparators(path));
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        QFileSystemModel *fs = (QFileSystemModel*)model;
        if (fs != sourceModel) {
            continue;
        }
        if (!findPath.startsWith(fs->rootPath())) {
            continue;
        }
        QModelIndex index = fs->index(path);
        if (index.isValid()) {
            return this->mapFromSourceEx(model,index);
        }
    }
    return QModelIndex();
}

void MultiFolderModel::setFilter(QDir::Filters filters)
{
    if (m_filters == filters) {
        return;
    }
    m_filters = filters;
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        ((QFileSystemModel*)model)->setFilter(filters);
    }
}

QDir::Filters MultiFolderModel::filter() const
{
    return m_filters;
}

void MultiFolderModel::setSorting(QDir::SortFlags sort)
{
    m_sorts = sort;
}

QDir::SortFlags MultiFolderModel::sorting() const
{
    return m_sorts;
}

void MultiFolderModel::setResolveSymlinks(bool enable)
{
    if (m_resolveSymlinks == enable) {
        return;
    }
    m_resolveSymlinks = enable;
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        ((QFileSystemModel*)model)->setResolveSymlinks(enable);
    }
}

bool MultiFolderModel::resolveSymlinks() const
{
    return m_resolveSymlinks;
}

void MultiFolderModel::setReadOnly(bool enable)
{
    if (m_isReadOnly == enable) {
        return;
    }
    m_isReadOnly = enable;
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        ((QFileSystemModel*)model)->setReadOnly(enable);
    }
}

bool MultiFolderModel::isReadOnly() const
{
    return m_isReadOnly;
}

void MultiFolderModel::setNameFilterDisables(bool enable)
{
    if (m_nameFilterDisables == enable) {
        return;
    }
    m_nameFilterDisables = enable;
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        ((QFileSystemModel*)model)->setNameFilterDisables(enable);
    }
}

bool MultiFolderModel::nameFilterDisables() const
{
    return m_nameFilterDisables;
}

void MultiFolderModel::setNameFilters(const QStringList &filters)
{
    if (m_nameFilters == filters) {
        return;
    }
    m_nameFilters = filters;
    foreach (QAbstractItemModel *model, this->sourceModelList()) {
        ((QFileSystemModel*)model)->setNameFilters(filters);
    }
}

QStringList MultiFolderModel::nameFilters() const
{
    return m_nameFilters;
}

void MultiFolderModel::setShowDetails(bool b)
{
    emit layoutAboutToBeChanged();
    m_isShowDetails = b;
    emit layoutChanged();
}

bool MultiFolderModel::isShowDetails() const
{
    return m_isShowDetails;
}

bool MultiFolderModel::lessThan(const QAbstractItemModel *sourceModel, const QModelIndex &left, const QModelIndex &right) const
{
    QDirSortItemComparator comp(m_sorts);
    QDirSortItem n1;
    QDirSortItem n2;
    n1.item = ((QFileSystemModel*)sourceModel)->fileInfo(left);
    n2.item = ((QFileSystemModel*)sourceModel)->fileInfo(right);
    return comp.sort(n1,n2);
}

bool MultiFolderModel::filterAcceptsRow(const QAbstractItemModel *sourceModel,  int source_row, const QModelIndex &source_parent) const
{
//    QFileSystemModel *model = (QFileSystemModel*)sourceModel;
//    QModelIndex index = model->index(source_row,0,source_parent);
//    QFileInfo info = model->fileInfo(index);
//    if (info.fileName().startsWith(".")) {
//        return false;
//    }
    return true;
}

int MultiFolderModel::columnCount(const QModelIndex &parent) const
{
    if (m_isShowDetails) {
        return MultiIndexModel::columnCount(parent);
    }
    return 1;
}

void MultiFolderModel::slotDirectoryLoaded(const QString &path)
{
    emit directoryLoaded((QFileSystemModel*)sender(),path);
}
