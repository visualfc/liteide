#include "folderlistmodel.h"
#include <QDateTime>
#include <QDebug>

MyFileSystemModel::MyFileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{

}


FolderListModel::FolderListModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    ignoreNextLayoutAboutToBeChanged = false;
    ignoreNextLayoutChanged = false;
    m_filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs;
    m_resolveSymlinks = false;
    m_nameFilterDisables = true;
    m_bWatcherRoot = false;
}

FolderListModel::~FolderListModel()
{
    clear();
}

void FolderListModel::clear()
{
    if (m_modelList.isEmpty()) {
        return;
    }
    this->beginRemoveRows(QModelIndex(),0,m_modelList.size());
    this->removeRows(0,m_modelList.size());
    if (m_bWatcherRoot) {
        foreach (SourceModel s, m_modelList) {
            m_watcher->removePath(s.watchPath);
            delete s.model;
        }
    }
    m_modelList.clear();
    m_indexMap.clear();
    this->endRemoveRows();
}

QModelIndex FolderListModel::addRootPath(const QString &path)
{
    if (!QDir(path).exists()) {
        return QModelIndex();
    }
    MyFileSystemModel *model = new MyFileSystemModel(this);
    model->setFilter(m_filters);
    if (!m_nameFilters.isEmpty()) {
        model->setNameFilters(m_nameFilters);
    }
    model->setNameFilterDisables(m_nameFilterDisables);
    model->setResolveSymlinks(m_resolveSymlinks);
    QModelIndex sourceIndex = model->setRootPath(path);
    SourceModel m;
    m.model = model;
    m.rootPath = QDir::cleanPath(QDir::fromNativeSeparators(path));
    m.watchPath = model->filePath(sourceIndex.parent());
    if (m_bWatcherRoot) {
        if (!m_watcher->directories().contains(m.watchPath)) {
            m_watcher->addPath(m.watchPath);
        }
    }
    m.rootSourceIndex = sourceIndex;
    m.rootIndex = createIndex(m_modelList.size(),0,sourceIndex.internalPointer());
    m_indexMap.insert(sourceIndex.internalId(),model);
    m_modelList.append(m);

    connect(model, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)),
            SLOT(sourceRowsAboutToBeInserted(const QModelIndex &, int, int)));
    connect(model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            SLOT(sourceRowsInserted(const QModelIndex &, int, int)));
    connect(model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
            SLOT(sourceRowsAboutToBeRemoved(const QModelIndex &, int, int)));
    connect(model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            SLOT(sourceRowsRemoved(const QModelIndex &, int, int)));
    connect(model, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            SLOT(sourceRowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(model, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            SLOT(sourceRowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(model, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)),
            SLOT(sourceColumnsAboutToBeInserted(const QModelIndex &, int, int)));
    connect(model, SIGNAL(columnsInserted(const QModelIndex &, int, int)),
            SLOT(sourceColumnsInserted(const QModelIndex &, int, int)));
    connect(model, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)),
            SLOT(sourceColumnsAboutToBeRemoved(const QModelIndex &, int, int)));
    connect(model, SIGNAL(columnsRemoved(const QModelIndex &, int, int)),
            SLOT(sourceColumnsRemoved(const QModelIndex &, int, int)));
    connect(model, SIGNAL(columnsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            SLOT(sourceColumnsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(model, SIGNAL(columnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            SLOT(sourceColumnsMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(model, SIGNAL(modelAboutToBeReset()),
            SLOT(sourceModelAboutToBeReset()));
    connect(model, SIGNAL(modelReset()),
            SLOT(sourceModelReset()));
    connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            SLOT(sourceDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            SLOT(sourceHeaderDataChanged(Qt::Orientation,int,int)));
    connect(model, SIGNAL(layoutAboutToBeChanged()),
            SLOT(sourceLayoutAboutToBeChanged()));
    connect(model, SIGNAL(layoutChanged()),
            SLOT(sourceLayoutChanged()));
    return m.rootIndex;
}

void FolderListModel::removeRootPath(const QString &path)
{
    QString rootPath = QDir::cleanPath(QDir::fromNativeSeparators(path));
    QModelIndex index;
    foreach (SourceModel s, m_modelList) {
        if (s.rootPath == rootPath) {
            index = s.rootIndex;
            break;
        }
    }
    if (index.isValid()) {
        removeRoot(index);
    }
}

void FolderListModel::removeRoot(const QModelIndex &index)
{
    QMutableListIterator<SourceModel> i(m_modelList);
    while (i.hasNext()) {
        SourceModel s = i.next();
        if (s.rootIndex == index) {
            this->beginRemoveRows(QModelIndex(),index.row(),index.row());
            if (m_bWatcherRoot) {
                int n = 0;
                foreach (SourceModel m, m_modelList) {
                    if (m.watchPath == s.watchPath) {
                        n++;
                    }
                }
                if (n <= 1) {
                    m_watcher->removePath(s.watchPath);
                }
            }
            this->removeRow(index.row());
            i.remove();
            this->endRemoveRows();
            QMutableHashIterator<qint64,QAbstractItemModel*> i(m_indexMap);
            while (i.hasNext()) {
                i.next();
                if (i.value() == s.model) {
                    i.remove();
                }
            }
            delete s.model;
            break;
        }
    }
}

bool FolderListModel::isRootPath(const QString &path)
{
    QString rootPath = QDir::cleanPath(QDir::fromNativeSeparators(path));
    foreach (SourceModel s, m_modelList) {
        if (s.rootPath == rootPath) {
            return true;
        }
    }
    return false;
}

QList<QModelIndex> FolderListModel::rootIndexs() const
{
    QList<QModelIndex> indexs;
    foreach (SourceModel s, m_modelList) {
        indexs.append(s.rootIndex);
    }
    return indexs;
}

QStringList FolderListModel::rootPathList() const
{
    QStringList paths;
    foreach (SourceModel s, m_modelList) {
        paths.append(s.rootPath);
    }
    return paths;
}

MyFileSystemModel *FolderListModel::findSource(const QModelIndex &proxyIndex) const
{
    return (MyFileSystemModel*)m_indexMap[proxyIndex.internalId()];
}

QItemSelection FolderListModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    QModelIndexList proxyIndexes = proxySelection.indexes();
    QItemSelection sourceSelection;
    for (int i = 0; i < proxyIndexes.size(); ++i) {
        const QModelIndex proxyIdx = mapToSource(proxyIndexes.at(i));
        if (!proxyIdx.isValid())
            continue;
        sourceSelection << QItemSelectionRange(proxyIdx);
    }
    return sourceSelection;
}

QItemSelection FolderListModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    QModelIndexList sourceIndexes = sourceSelection.indexes();
    QItemSelection proxySelection;
    for (int i = 0; i < sourceIndexes.size(); ++i) {
        const QModelIndex srcIdx = mapFromSource(sourceIndexes.at(i));
        if (!srcIdx.isValid())
            continue;
        proxySelection << QItemSelectionRange(srcIdx);
    }
    return proxySelection;
}


QModelIndex FolderListModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();
    int row = sourceIndex.row();
    for (int i = 0; i < m_modelList.size(); i++) {
        if (m_modelList[i].rootSourceIndex.internalId() == sourceIndex.internalId()) {
            row = i;
            break;
        }
    }
    QModelIndex index = createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
    m_indexMap.insert(sourceIndex.internalId(),(QAbstractItemModel*)sourceIndex.model());
    return index;
}

QModelIndex FolderListModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }
    int row = proxyIndex.row();
    for (int i = 0; i < m_modelList.size(); i++) {
        if (m_modelList[i].rootIndex.internalId() == proxyIndex.internalId()) {
            row = i;
            break;
        }
    }
    MyFileSystemModel *model = findSource(proxyIndex);
    return model->createIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalPointer());
}

QString FolderListModel::filePath(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->filePath(sourceIndex);
}

QString FolderListModel::fileName(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->fileName(sourceIndex);
}

QFileInfo FolderListModel::fileInfo(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QFileInfo();
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->fileInfo(sourceIndex);
}

bool FolderListModel::isDir(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return true;
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->isDir(sourceIndex);
}

QModelIndex FolderListModel::mkdir(const QModelIndex &parent, const QString &name)
{
    if (!parent.isValid()) {
        return QModelIndex();
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return ((MyFileSystemModel*)sourceIndex.model())->mkdir(sourceIndex,name);
}

bool FolderListModel::rmdir(const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->rmdir(sourceIndex);
}

bool FolderListModel::remove(const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->remove(sourceIndex);
}

void FolderListModel::setFilter(QDir::Filters filters)
{
    m_filters = filters;
    foreach (SourceModel s, m_modelList) {
        s.model->setFilter(filters);
    }
}

QDir::Filters FolderListModel::filter() const
{
    return m_filters;
}

void FolderListModel::setNameFilters(const QStringList &filters)
{
    m_nameFilters = filters;
    foreach (SourceModel s, m_modelList) {
        s.model->setNameFilters(filters);
    }
}

QStringList FolderListModel::nameFilters() const
{
    return m_nameFilters;
}

void FolderListModel::setNameFilterDisables(bool enable)
{
    if (m_nameFilterDisables == enable) {
        return;
    }
    m_nameFilterDisables = enable;
    foreach (SourceModel s, m_modelList) {
        s.model->setNameFilterDisables(enable);
    }
}

bool FolderListModel::nameFilterDisables() const
{
    return m_nameFilterDisables;
}

void FolderListModel::setResolveSymlinks(bool enable)
{
    if (m_resolveSymlinks == enable) {
        return;
    }
    m_resolveSymlinks = enable;
    foreach (SourceModel s, m_modelList) {
        s.model->setResolveSymlinks(enable);
    }
}

bool FolderListModel::resolveSymlinks() const
{
    return m_resolveSymlinks;
}

bool FolderListModel::isRootIndex(const QModelIndex &index) const
{
    foreach (SourceModel s, m_modelList) {
        if (s.rootIndex.internalId() == index.internalId()) {
            return true;
        }
    }
    return false;
}

void FolderListModel::setWatcherRoot(bool b)
{
    if (m_bWatcherRoot == b) {
        return;
    }
    if (m_bWatcherRoot) {
        if (!m_watcher) {
            m_watcher = new QFileSystemWatcher(this);
        }
        connect(m_watcher,SIGNAL(directoryChanged(QString)),this,SLOT(directoryChanged(QString)));
        QStringList paths;
        foreach (SourceModel s, m_modelList) {
            paths.append(s.watchPath);
        }
        paths.removeDuplicates();
        m_watcher->addPaths(paths);
    } else {
        if (m_watcher) {
            delete m_watcher;
        }
        m_watcher = 0;
    }
}

bool FolderListModel::isWatcherRoot() const
{
    return m_bWatcherRoot;
}

QList<QModelIndex> FolderListModel::indexForPath(const QString &path) const
{
    QList<QModelIndex> indexs;
    foreach (SourceModel s, m_modelList) {
        QModelIndex sourceIndex = s.model->index(path);
        if (sourceIndex.isValid()) {
            indexs.append(this->mapFromSource(sourceIndex));
        }
    }
    return indexs;
}

bool FolderListModel::isRootSourceIndex(const QModelIndex &sourceIndex) const
{
    foreach (SourceModel s, m_modelList) {
        if (s.rootSourceIndex.internalId() == sourceIndex.internalId()) {
            return true;
        }
    }
    return false;
}

int FolderListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
//    if (!parent.isValid()) {
//        if (!m_modelList.isEmpty()) {
//            return 4;
//        }
//        return 1;
//    }
//    QModelIndex sourceIndex = mapToSource(parent);
//    return sourceIndex.model()->columnCount(sourceIndex);
}

QModelIndex FolderListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    if (!parent.isValid()) {
        QModelIndex sourceIndex = m_modelList[row].rootSourceIndex;
        if (sourceIndex.column() != column) {
            sourceIndex = sourceIndex.sibling(sourceIndex.row(),column);
            m_indexMap.insert(sourceIndex.internalId(),m_modelList[row].model);
        }
        return createIndex(row,column,sourceIndex.internalPointer());
    }
    const QModelIndex sourceParent = mapToSource(parent);
    const QModelIndex sourceIndex = sourceParent.model()->index(row, column, sourceParent);
    Q_ASSERT(sourceIndex.isValid());
    return mapFromSource(sourceIndex);
}

QModelIndex FolderListModel::parent(const QModelIndex &child) const
{
    Q_ASSERT(child.isValid() ? child.model() == this : true);
    if (isRootIndex(child)) {
        return QModelIndex();
    }
    const QModelIndex sourceIndex = mapToSource(child);
    const QModelIndex sourceParent = sourceIndex.parent();
    return mapFromSource(sourceParent);
}

int FolderListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_modelList.size();
    }
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    QModelIndex sourceIndex = mapToSource(parent);
    return sourceIndex.model()->rowCount(sourceIndex);
}

static QString fileSize(qint64 bytes)
{
    // According to the Si standard KB is 1000 bytes, KiB is 1024
    // but on windows sizes are calculated by dividing by 1024 so we do what they do.
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    const qint64 tb = 1024 * gb;
    if (bytes >= tb)
        return QFileSystemModel::tr("%1 TB").arg(QLocale().toString(qreal(bytes) / tb, 'f', 3));
    if (bytes >= gb)
        return QFileSystemModel::tr("%1 GB").arg(QLocale().toString(qreal(bytes) / gb, 'f', 2));
    if (bytes >= mb)
        return QFileSystemModel::tr("%1 MB").arg(QLocale().toString(qreal(bytes) / mb, 'f', 1));
    if (bytes >= kb)
        return QFileSystemModel::tr("%1 KB").arg(QLocale().toString(bytes / kb));
    return QFileSystemModel::tr("%1 bytes").arg(QLocale().toString(bytes));
}

QVariant FolderListModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid()) {
        return QVariant();
    }
    QModelIndex sourceIndex = mapToSource(proxyIndex);
    if (role == Qt::ToolTipRole) {
        MyFileSystemModel *model = (MyFileSystemModel*)sourceIndex.model();
        if (model->isDir(sourceIndex)) {
            return QDir::toNativeSeparators(model->filePath(sourceIndex));
        } else {
            QFileInfo info = model->fileInfo(sourceIndex);
            return QString("%1\n%2\n%3")
                    .arg(QDir::toNativeSeparators(info.filePath()))
                    .arg(fileSize(info.size()))
                    .arg(info.lastModified().toString(Qt::SystemLocaleDate));
        }
    }
    return sourceIndex.model()->data(sourceIndex,role);
}


QVariant FolderListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (m_modelList.isEmpty()) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        const QModelIndex proxyIndex = index(0, section);
        QModelIndex sourceIndex = mapToSource(proxyIndex);
        int sourceSection = sourceIndex.column();
        return sourceIndex.model()->headerData(sourceSection,orientation,role);
    } else {
        const QModelIndex proxyIndex = index(section, 0);
        QModelIndex sourceIndex = mapToSource(proxyIndex);
        int sourceSection = sourceIndex.row();
        return sourceIndex.model()->headerData(sourceSection,orientation,role);
    }
}

QMap<int, QVariant> FolderListModel::itemData(const QModelIndex &proxyIndex) const
{
    return QAbstractItemModel::itemData(proxyIndex);
}

Qt::ItemFlags FolderListModel::flags(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    return sourceIndex.model()->flags(sourceIndex);
}

bool FolderListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QModelIndex sourceIndex = mapToSource(index);
    return ((QAbstractItemModel*)sourceIndex.model())->setData(sourceIndex,value,role);
}

bool FolderListModel::setItemData(const QModelIndex &index, const QMap< int, QVariant >& roles)
{
    return QAbstractItemModel::setItemData(index, roles);
}

bool FolderListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal) {
        const QModelIndex proxyIndex = index(0, section);
        QModelIndex sourceIndex = mapToSource(proxyIndex);
        int sourceSection = sourceIndex.column();
        return ((QAbstractItemModel*)sourceIndex.model())->setHeaderData(sourceSection,orientation,value,role);
    } else {
        const QModelIndex proxyIndex = index(section, 0);
        QModelIndex sourceIndex = mapToSource(proxyIndex);
        int sourceSection = sourceIndex.row();
        return ((QAbstractItemModel*)sourceIndex.model())->setHeaderData(sourceSection,orientation,value,role);
    }
}


QModelIndex FolderListModel::buddy(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    return mapFromSource(sourceIndex.model()->buddy(sourceIndex));
}

bool FolderListModel::insertColumns(int column, int count, const QModelIndex& parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    QModelIndex sourceIndex = mapToSource(parent);
    return ((QAbstractItemModel*)sourceIndex.model())->insertColumns(column, count, sourceIndex);
}

/*!
    \reimp
 */
bool FolderListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    QModelIndex sourceIndex = mapToSource(parent);
    return ((QAbstractItemModel*)sourceIndex.model())->insertRows(row, count, sourceIndex);
}

bool FolderListModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    QModelIndex sourceIndex = mapToSource(parent);
    return ((QAbstractItemModel*)sourceIndex.model())->removeColumns(column, count, sourceIndex);
}

bool FolderListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    if (!parent.isValid()) {
        return QAbstractItemModel::removeRows(row,count);
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return ((QAbstractItemModel*)sourceIndex.model())->removeRows(row, count, sourceIndex);
}

void FolderListModel::directoryChanged(const QString &path)
{
    if (!m_bWatcherRoot) {
        return;
    }
    foreach(SourceModel s, m_modelList) {
         if (s.watchPath == path && !QDir(s.rootPath).exists()) {
              this->removeRoot(s.rootIndex);
              break;
         }
    }
}

bool FolderListModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return false;
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return sourceIndex.model()->canFetchMore(sourceIndex);
}

void FolderListModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid()) {
        return;
    }
    QModelIndex sourceIndex = mapToSource(parent);
    QAbstractItemModel *model = (QAbstractItemModel*)sourceIndex.model();
    model->fetchMore(sourceIndex);
}

void FolderListModel::sort(int column, Qt::SortOrder order)
{
    foreach (SourceModel s, m_modelList) {
        s.model->sort(column,order);
    }
}

QSize FolderListModel::span(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QAbstractItemModel::span(index);
    }
    QModelIndex sourceIndex = mapToSource(index);
    return sourceIndex.model()->span(sourceIndex);
}

bool FolderListModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_modelList.size() > 0;
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return sourceIndex.model()->hasChildren(sourceIndex);
}

void FolderListModel::sourceColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    this->beginInsertColumns(mapFromSource(parent), start, end);
}

void FolderListModel::sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == m_model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == m_model : true);
    this->beginMoveColumns(this->mapFromSource(sourceParent), sourceStart, sourceEnd, this->mapFromSource(destParent), dest);
}

void FolderListModel::sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    this->beginRemoveColumns(this->mapFromSource(parent), start, end);
}

void FolderListModel::sourceColumnsInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->endInsertColumns();
}

void FolderListModel::sourceColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == m_model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == m_model : true);
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(destParent)
    Q_UNUSED(dest)
    this->endMoveColumns();
}

void FolderListModel::sourceColumnsRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->endRemoveColumns();
}

void FolderListModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    //Q_ASSERT(topLeft.isValid() ? topLeft.model() == m_model : true);
    //Q_ASSERT(bottomRight.isValid() ? bottomRight.model() == m_model : true);
    this->dataChanged(this->mapFromSource(topLeft), this->mapFromSource(bottomRight));
}

void FolderListModel::sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    this->headerDataChanged(orientation, first, last);
}

void FolderListModel::sourceLayoutAboutToBeChanged()
{
    if (ignoreNextLayoutAboutToBeChanged)
        return;

//    foreach(const QPersistentModelIndex &proxyPersistentIndex, this->persistentIndexList()) {
//        proxyIndexes << proxyPersistentIndex;
//        Q_ASSERT(proxyPersistentIndex.isValid());
//        const QPersistentModelIndex srcPersistentIndex = this->mapToSource(proxyPersistentIndex);
//        Q_ASSERT(srcPersistentIndex.isValid());
//        layoutChangePersistentIndexes << srcPersistentIndex;
//    }

    this->layoutAboutToBeChanged();
}

void FolderListModel::sourceLayoutChanged()
{
    if (ignoreNextLayoutChanged)
        return;

//    for (int i = 0; i < proxyIndexes.size(); ++i) {
//        this->changePersistentIndex(proxyIndexes.at(i), this->mapFromSource(layoutChangePersistentIndexes.at(i)));
//    }

//    layoutChangePersistentIndexes.clear();
//    proxyIndexes.clear();

    this->layoutChanged();
}

void FolderListModel::sourceModelAboutToBeReset()
{
    this->beginResetModel();
}

void FolderListModel::sourceModelReset()
{
    this->endResetModel();
}

void FolderListModel::sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    this->beginInsertRows(this->mapFromSource(parent), start, end);
}

void FolderListModel::sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == m_model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == m_model : true);
    this->beginMoveRows(this->mapFromSource(sourceParent), sourceStart, sourceEnd, this->mapFromSource(destParent), dest);
}

void FolderListModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    this->beginRemoveRows(this->mapFromSource(parent), start, end);
}

void FolderListModel::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->endInsertRows();
}

void FolderListModel::sourceRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == m_model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == m_model : true);
    Q_UNUSED(sourceParent)
    Q_UNUSED(sourceStart)
    Q_UNUSED(sourceEnd)
    Q_UNUSED(destParent)
    Q_UNUSED(dest)
    this->endMoveRows();
}

void FolderListModel::sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->endRemoveRows();
}
