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

#include "searchresulttreemodel.h"
#include "searchresulttreeitems.h"
#include "searchresulttreeitemroles.h"

#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>

using namespace Find;
using namespace Find::Internal;

SearchResultTreeModel::SearchResultTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_currentParent(0)
    , m_showReplaceUI(false)
    , m_editorFontIsUsed(false)
{
    m_rootItem = new SearchResultTreeItem;
    m_textEditorFont = QFont(QLatin1String("Courier"));
}

SearchResultTreeModel::~SearchResultTreeModel()
{
    delete m_rootItem;
}

void SearchResultTreeModel::setShowReplaceUI(bool show)
{
    m_showReplaceUI = show;
}

void SearchResultTreeModel::setTextEditorFont(const QFont &font, const SearchResultColor color)
{
    layoutAboutToBeChanged();
    m_textEditorFont = font;
    m_color = color;
    layoutChanged();
}

Qt::ItemFlags SearchResultTreeModel::flags(const QModelIndex &idx) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(idx);

    if (idx.isValid()) {
        if (const SearchResultTreeItem *item = treeItemAtIndex(idx)) {
            if (item->isUserCheckable())
                flags |= Qt::ItemIsUserCheckable;
        }
    }

    return flags;
}

QModelIndex SearchResultTreeModel::index(int row, int column,
                                         const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    const SearchResultTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = treeItemAtIndex(parent);

    const SearchResultTreeItem *childItem = parentItem->childAt(row);
    if (childItem)
        return createIndex(row, column, (void *)childItem);
    else
        return QModelIndex();
}

QModelIndex SearchResultTreeModel::index(SearchResultTreeItem *item) const
{
    return createIndex(item->rowOfItem(), 0, (void *)item);
}

QModelIndex SearchResultTreeModel::parent(const QModelIndex &idx) const
{
    if (!idx.isValid())
        return QModelIndex();

    const SearchResultTreeItem *childItem = treeItemAtIndex(idx);
    const SearchResultTreeItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->rowOfItem(), 0, (void *)parentItem);
}

int SearchResultTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    const SearchResultTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = treeItemAtIndex(parent);

    return parentItem->childrenCount();
}

int SearchResultTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

SearchResultTreeItem *SearchResultTreeModel::treeItemAtIndex(const QModelIndex &idx) const
{
    return static_cast<SearchResultTreeItem*>(idx.internalPointer());
}

QVariant SearchResultTreeModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid())
        return QVariant();

    QVariant result;

    if (role == Qt::SizeHintRole) {
        int height = QApplication::fontMetrics().height();
        if (m_editorFontIsUsed) {
            const int editorFontHeight = QFontMetrics(m_textEditorFont).height();
            height = qMax(height, editorFontHeight);
        }
        result = QSize(0, height);
    } else {
        result = data(treeItemAtIndex(idx), role);
    }

    return result;
}

bool SearchResultTreeModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole) {
        Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());
        return setCheckState(idx, checkState);
    }
    return QAbstractItemModel::setData(idx, value, role);
}

bool SearchResultTreeModel::setCheckState(const QModelIndex &idx, Qt::CheckState checkState, bool firstCall)
{
    SearchResultTreeItem *item = treeItemAtIndex(idx);
    if (item->checkState() == checkState)
        return false;
    item->setCheckState(checkState);
    if (firstCall) {
        emit dataChanged(idx, idx);
        // check parents
        SearchResultTreeItem *currentItem = item;
        QModelIndex currentIndex = idx;
        while (SearchResultTreeItem *parent = currentItem->parent()) {
            if (parent->isUserCheckable()) {
                bool hasChecked = false;
                bool hasUnchecked = false;
                for (int i = 0; i < parent->childrenCount(); ++i) {
                    SearchResultTreeItem *child = parent->childAt(i);
                    if (!child->isUserCheckable())
                        continue;
                    if (child->checkState() == Qt::Checked)
                        hasChecked = true;
                    else if (child->checkState() == Qt::Unchecked)
                        hasUnchecked = true;
                    else if (child->checkState() == Qt::PartiallyChecked)
                        hasChecked = hasUnchecked = true;
                }
                if (hasChecked && hasUnchecked)
                    parent->setCheckState(Qt::PartiallyChecked);
                else if (hasChecked)
                    parent->setCheckState(Qt::Checked);
                else
                    parent->setCheckState(Qt::Unchecked);
                emit dataChanged(idx.parent(), idx.parent());
            }
            currentItem = parent;
            currentIndex = idx.parent();
        }
    }
    // check children
    if (int children = item->childrenCount()) {
        for (int i = 0; i < children; ++i) {
            setCheckState(idx.child(i, 0), checkState, false);
        }
        emit dataChanged(idx.child(0, 0), idx.child(children-1, 0));
    }
    return true;
}

void setDataInternal(const QModelIndex &index, const QVariant &value, int role);

QVariant SearchResultTreeModel::data(const SearchResultTreeItem *row, int role) const
{
    QVariant result;

    switch (role)
    {
    case Qt::CheckStateRole:
        if (row->isUserCheckable())
            result = row->checkState();
        break;
    case Qt::ToolTipRole:
        result = row->item.text.trimmed();
        break;
    case Qt::FontRole:
        if (row->item.useTextEditorFont)
            result = m_textEditorFont;
        else
            result = QVariant();
        break;
    case Qt::TextColorRole:
        result = m_color.textForeground;
        break;
    case Qt::BackgroundRole:
        result = m_color.textBackground;
        break;
    case ItemDataRoles::ResultLineRole:
    case Qt::DisplayRole:
        result = row->item.text;
        break;
    case ItemDataRoles::ResultItemRole:
        result = qVariantFromValue(row->item);
        break;
    case ItemDataRoles::ResultLineNumberRole:
        result = row->item.lineNumber;
        break;
    case ItemDataRoles::ResultIconRole:
        result = row->item.icon;
        break;
    case ItemDataRoles::ResultHighlightBackgroundColor:
        result = m_color.highlightBackground;
        break;
    case ItemDataRoles::ResultHighlightForegroundColor:
        result = m_color.highlightForeground;
        break;
    case ItemDataRoles::SearchTermStartRole:
        result = row->item.textMarkPos;
        break;
    case ItemDataRoles::SearchTermLengthRole:
        result = row->item.textMarkLength;
        break;
    case ItemDataRoles::IsGeneratedRole:
        result = row->isGenerated();
        break;
    default:
        result = QVariant();
        break;
    }

    return result;
}

QVariant SearchResultTreeModel::headerData(int section, Qt::Orientation orientation,
                                           int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

/**
 * Makes sure that the nodes for a specific path exist and sets
 * m_currentParent to the last final
 */
QSet<SearchResultTreeItem *> SearchResultTreeModel::addPath(const QStringList &path)
{
    QSet<SearchResultTreeItem *> pathNodes;
    SearchResultTreeItem *currentItem = m_rootItem;
    QModelIndex currentItemIndex = QModelIndex();
    SearchResultTreeItem *partItem = 0;
    QStringList currentPath;
    foreach (const QString &part, path) {
        const int insertionIndex = currentItem->insertionIndex(part, &partItem);
        if (!partItem) {
            SearchResultItem item;
            item.path = currentPath;
            item.text = part;
            partItem = new SearchResultTreeItem(item, currentItem);
            if (m_showReplaceUI) {
                partItem->setIsUserCheckable(true);
                partItem->setCheckState(Qt::Checked);
            }
            partItem->setGenerated(true);
            beginInsertRows(currentItemIndex, insertionIndex, insertionIndex);
            currentItem->insertChild(insertionIndex, partItem);
            endInsertRows();
        }
        pathNodes << partItem;
        currentItemIndex = index(insertionIndex, 0, currentItemIndex);
        currentItem = partItem;
        currentPath << part;
    }

    m_currentParent = currentItem;
    m_currentPath = currentPath;
    m_currentIndex = currentItemIndex;
    return pathNodes;
}

void SearchResultTreeModel::addResultsToCurrentParent(const QList<SearchResultItem> &items, AddMode mode)
{
    if (!m_currentParent)
        return;

    if (mode == AddOrdered) {
        // this is the mode for e.g. text search
        beginInsertRows(m_currentIndex, m_currentParent->childrenCount(), m_currentParent->childrenCount() + items.count());
        foreach (const SearchResultItem &item, items) {
            m_currentParent->appendChild(item);
        }
        endInsertRows();
    } else if (mode == AddSorted) {
        foreach (const SearchResultItem &item, items) {
            SearchResultTreeItem *existingItem;
            const int insertionIndex = m_currentParent->insertionIndex(item, &existingItem);
            if (existingItem) {
                existingItem->setGenerated(false);
                existingItem->item = item;
                QModelIndex itemIndex = m_currentIndex.child(insertionIndex, 0);
                dataChanged(itemIndex, itemIndex);
            } else {
                beginInsertRows(m_currentIndex, insertionIndex, insertionIndex);
                m_currentParent->insertChild(insertionIndex, item);
                endInsertRows();
            }
        }
    }
    dataChanged(m_currentIndex, m_currentIndex); // Make sure that the number after the file name gets updated
}

static bool lessThanByPath(const SearchResultItem &a, const SearchResultItem &b)
{
    if (a.path.size() < b.path.size())
        return true;
    if (a.path.size() > b.path.size())
        return false;
    for (int i = 0; i < a.path.size(); ++i) {
        if (a.path.at(i) < b.path.at(i))
            return true;
        if (a.path.at(i) > b.path.at(i))
            return false;
    }
    return false;
}

/**
 * Adds the search result to the list of results, creating nodes for the path when
 * necessary.
 */
QList<QModelIndex> SearchResultTreeModel::addResults(const QList<SearchResultItem> &items, AddMode mode)
{
    QSet<SearchResultTreeItem *> pathNodes;
    QList<SearchResultItem> sortedItems = items;
    qStableSort(sortedItems.begin(), sortedItems.end(), lessThanByPath);
    QList<SearchResultItem> itemSet;
    foreach (const SearchResultItem &item, sortedItems) {
        m_editorFontIsUsed |= item.useTextEditorFont;
        if (!m_currentParent || (m_currentPath != item.path)) {
            // first add all the items from before
            if (!itemSet.isEmpty()) {
                addResultsToCurrentParent(itemSet, mode);
                itemSet.clear();
            }
            // switch parent
            pathNodes += addPath(item.path);
        }
        itemSet << item;
    }
    if (!itemSet.isEmpty()) {
        addResultsToCurrentParent(itemSet, mode);
        itemSet.clear();
    }
    QList<QModelIndex> pathIndices;
    foreach (SearchResultTreeItem *item, pathNodes)
        pathIndices << index(item);
    return pathIndices;
}

void SearchResultTreeModel::clear()
{
    beginResetModel();
    m_currentParent = NULL;
    m_rootItem->clearChildren();
    m_editorFontIsUsed = false;
    endResetModel();
}

QModelIndex SearchResultTreeModel::nextIndex(const QModelIndex &idx, bool *wrapped) const
{
    if (wrapped)
        *wrapped = false;
    // pathological
    if (!idx.isValid())
        return index(0, 0);

    if (rowCount(idx) > 0) {
        // node with children
        return idx.child(0, 0);
    }
    // leaf node
    QModelIndex nextIndex;
    QModelIndex current = idx;
    while (!nextIndex.isValid()) {
        int row = current.row();
        current = current.parent();
        if (row + 1 < rowCount(current)) {
            // Same parent has another child
            nextIndex = index(row + 1, 0, current);
        } else {
            // go up one parent
            if (!current.isValid()) {
                // we start from the beginning
                if (wrapped)
                    *wrapped = true;
                nextIndex = index(0, 0);
            }
        }
    }
    return nextIndex;
}

QModelIndex SearchResultTreeModel::next(const QModelIndex &idx, bool includeGenerated, bool *wrapped) const
{
    QModelIndex value = idx;
    do {
        value = nextIndex(value, wrapped);
    } while (value != idx && !includeGenerated && treeItemAtIndex(value)->isGenerated());
    return value;
}

QModelIndex SearchResultTreeModel::prevIndex(const QModelIndex &idx, bool *wrapped) const
{
    if (wrapped)
        *wrapped = false;
    QModelIndex current = idx;
    bool checkForChildren = true;
    if (current.isValid()) {
        int row = current.row();
        if (row > 0) {
            current = index(row - 1, 0, current.parent());
        } else {
            current = current.parent();
            checkForChildren = !current.isValid();
            if (checkForChildren && wrapped) {
                // we start from the end
                *wrapped = true;
            }
        }
    }
    if (checkForChildren) {
        // traverse down the hierarchy
        while (int rc = rowCount(current)) {
            current = index(rc - 1, 0, current);
        }
    }
    return current;
}

QModelIndex SearchResultTreeModel::prev(const QModelIndex &idx, bool includeGenerated, bool *wrapped) const
{
    QModelIndex value = idx;
    do {
        value = prevIndex(value, wrapped);
    } while (value != idx && !includeGenerated && treeItemAtIndex(value)->isGenerated());
    return value;
}
