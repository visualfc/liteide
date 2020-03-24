/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
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
// Module: dirsortfilterproxymodel.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "dirsortfilterproxymodel.h"
#include <QFileSystemModel>
#include <QFileInfo>
#include <QDirModel>
#include <QDateTime>
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
    bool sort(const QDirSortItem &, const QDirSortItem &);
};

bool QDirSortItemComparator::sort(const QDirSortItem &n1, const QDirSortItem &n2)
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


DirSortFilterProxyModel::DirSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent), m_sorts(QDir::NoSort)
{

}

void DirSortFilterProxyModel::setSorting(QDir::SortFlags sort)
{
    m_sorts = sort;
}

QDir::SortFlags DirSortFilterProxyModel::sorting() const
{
    return m_sorts;
}

bool DirSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QFileSystemModel *model = dynamic_cast<QFileSystemModel*>(this->sourceModel());
    if (!model) {
        return QSortFilterProxyModel::lessThan(left,right);
    }
    QDirSortItemComparator comp(m_sorts);
    QDirSortItem n1;
    QDirSortItem n2;
    n1.item = model->fileInfo(left);
    n2.item = model->fileInfo(right);
    return comp.sort(n1,n2);
}

