/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef SEARCHRESULTTREEITEMS_H
#define SEARCHRESULTTREEITEMS_H

#include "searchresultitem.h"

#include <QString>
#include <QList>

namespace Find {
namespace Internal {

class SearchResultTreeItem
{
public:
    explicit SearchResultTreeItem(const SearchResultItem &item = SearchResultItem(),
                                  SearchResultTreeItem *parent = NULL);
    virtual ~SearchResultTreeItem();

    bool isLeaf() const;
    SearchResultTreeItem *parent() const;
    SearchResultTreeItem *childAt(int index) const;
    int insertionIndex(const QString &text, SearchResultTreeItem **existingItem) const;
    int insertionIndex(const SearchResultItem &item, SearchResultTreeItem **existingItem) const;
    void insertChild(int index, SearchResultTreeItem *child);
    void insertChild(int index, const SearchResultItem &item);
    void appendChild(const SearchResultItem &item);
    int childrenCount() const;
    int rowOfItem() const;
    void clearChildren();

    bool isUserCheckable() const;
    void setIsUserCheckable(bool isUserCheckable);

    Qt::CheckState checkState() const;
    void setCheckState(Qt::CheckState checkState);

    bool isGenerated() const { return m_isGenerated; }
    void setGenerated(bool value) { m_isGenerated = value; }

    SearchResultItem item;

private:
    SearchResultTreeItem *m_parent;
    QList<SearchResultTreeItem *> m_children;
    bool m_isUserCheckable;
    bool m_isGenerated;
    Qt::CheckState m_checkState;
};

} // namespace Internal
} // namespace Find

#endif // SEARCHRESULTTREEITEMS_H
