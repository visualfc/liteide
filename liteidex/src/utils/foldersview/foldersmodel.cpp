#include "foldersmodel.h"
#include <QDateTime>
#include <QDebug>

MyFileSystemModel::MyFileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{

}


FoldersModel::FoldersModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_watcher = new QFileSystemWatcher(this);
    ignoreNextLayoutAboutToBeChanged = false;
    ignoreNextLayoutChanged = false;
    m_filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs;
    m_resolveSymlinks = false;
    m_nameFilterDisables = true;
    connect(m_watcher,SIGNAL(directoryChanged(QString)),this,SLOT(directoryChanged(QString)));
}

FoldersModel::~FoldersModel()
{
    clear();
}

void FoldersModel::clear()
{
    if (m_modelList.isEmpty()) {
        return;
    }
    this->beginRemoveRows(QModelIndex(),0,m_modelList.size());
    this->removeRows(0,m_modelList.size());
    foreach (SourceModel s, m_modelList) {
        m_watcher->removePath(s.watchPath);
        delete s.model;
    }
    m_modelList.clear();
    m_indexMap.clear();
    this->endRemoveRows();
}

QModelIndex FoldersModel::addRootPath(const QString &path)
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
    if (!m_watcher->directories().contains(m.watchPath)) {
        m_watcher->addPath(m.watchPath);
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

void FoldersModel::removeRootPath(const QString &path)
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

void FoldersModel::removeRoot(const QModelIndex &index)
{
    QMutableListIterator<SourceModel> i(m_modelList);
    while (i.hasNext()) {
        SourceModel s = i.next();
        if (s.rootIndex == index) {
            this->beginRemoveRows(QModelIndex(),index.row(),index.row());
            int n = 0;
            foreach (SourceModel m, m_modelList) {
                if (m.watchPath == s.watchPath) {
                    n++;
                }
            }
            if (n <= 1) {
                m_watcher->removePath(s.watchPath);
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

bool FoldersModel::isRootPath(const QString &path)
{
    QString rootPath = QDir::cleanPath(QDir::fromNativeSeparators(path));
    foreach (SourceModel s, m_modelList) {
        if (s.rootPath == rootPath) {
            return true;
        }
    }
    return false;
}

QList<QModelIndex> FoldersModel::rootIndexs() const
{
    QList<QModelIndex> indexs;
    foreach (SourceModel s, m_modelList) {
        indexs.append(s.rootIndex);
    }
    return indexs;
}

QStringList FoldersModel::rootPathList() const
{
    QStringList paths;
    foreach (SourceModel s, m_modelList) {
        paths.append(s.rootPath);
    }
    return paths;
}

MyFileSystemModel *FoldersModel::findSource(const QModelIndex &proxyIndex) const
{
    return (MyFileSystemModel*)m_indexMap[proxyIndex.internalId()];
}

QModelIndex FoldersModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();
//    int row = sourceIndex.row();
//    for (int i = 0; i < m_modelList.size(); i++) {
//        if (m_modelList[i].rootSourceIndex.internalId() == sourceIndex.internalId()) {
//            row = i;
//            break;
//        }
//    }
    QModelIndex index = createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
    m_indexMap.insert(sourceIndex.internalId(),(QAbstractItemModel*)sourceIndex.model());
    return index;
}

QModelIndex FoldersModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }
//    int row = proxyIndex.row();
//    for (int i = 0; i < m_modelList.size(); i++) {
//        if (m_modelList[i].rootIndex.internalId() == proxyIndex.internalId()) {
//            row = i;
//            break;
//        }
//    }
    MyFileSystemModel *model = findSource(proxyIndex);
    return model->createIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalPointer());
}

QString FoldersModel::filePath(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->filePath(sourceIndex);
}

QString FoldersModel::fileName(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QString();
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->fileName(sourceIndex);
}

QFileInfo FoldersModel::fileInfo(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QFileInfo();
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->fileInfo(sourceIndex);
}

bool FoldersModel::isDir(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return true;
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->isDir(sourceIndex);
}

QModelIndex FoldersModel::mkdir(const QModelIndex &parent, const QString &name)
{
    if (!parent.isValid()) {
        return QModelIndex();
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return ((MyFileSystemModel*)sourceIndex.model())->mkdir(sourceIndex,name);
}

bool FoldersModel::rmdir(const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->rmdir(sourceIndex);
}

bool FoldersModel::remove(const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QModelIndex sourceIndex = mapToSource(index);
    return ((MyFileSystemModel*)sourceIndex.model())->remove(sourceIndex);
}

void FoldersModel::setFilter(QDir::Filters filters)
{
    m_filters = filters;
    foreach (SourceModel s, m_modelList) {
        s.model->setFilter(filters);
    }
}

QDir::Filters FoldersModel::filter() const
{
    return m_filters;
}

void FoldersModel::setNameFilters(const QStringList &filters)
{
    m_nameFilters = filters;
    foreach (SourceModel s, m_modelList) {
        s.model->setNameFilters(filters);
    }
}

QStringList FoldersModel::nameFilters() const
{
    return m_nameFilters;
}

void FoldersModel::setNameFilterDisables(bool enable)
{
    if (m_nameFilterDisables == enable) {
        return;
    }
    m_nameFilterDisables = enable;
    foreach (SourceModel s, m_modelList) {
        s.model->setNameFilterDisables(enable);
    }
}

bool FoldersModel::nameFilterDisables() const
{
    return m_nameFilterDisables;
}

void FoldersModel::setResolveSymlinks(bool enable)
{
    if (m_resolveSymlinks == enable) {
        return;
    }
    m_resolveSymlinks = enable;
    foreach (SourceModel s, m_modelList) {
        s.model->setResolveSymlinks(enable);
    }
}

bool FoldersModel::resolveSymlinks() const
{
    return m_resolveSymlinks;
}

bool FoldersModel::isRootIndex(const QModelIndex &index) const
{
    foreach (SourceModel s, m_modelList) {
        if (s.rootIndex.internalId() == index.internalId()) {
            return true;
        }
    }
    return false;
}

bool FoldersModel::isRootSourceIndex(const QModelIndex &sourceIndex) const
{
    foreach (SourceModel s, m_modelList) {
        if (s.rootSourceIndex.internalId() == sourceIndex.internalId()) {
            return true;
        }
    }
    return false;
}

int FoldersModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (!m_modelList.isEmpty()) {
            return 4;
        }
        return 1;
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return sourceIndex.model()->columnCount(sourceIndex);
}

QModelIndex FoldersModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex FoldersModel::parent(const QModelIndex &child) const
{
    Q_ASSERT(child.isValid() ? child.model() == this : true);
    if (isRootIndex(child)) {
        return QModelIndex();
    }
    const QModelIndex sourceIndex = mapToSource(child);
    const QModelIndex sourceParent = sourceIndex.parent();
    return mapFromSource(sourceParent);
}

int FoldersModel::rowCount(const QModelIndex &parent) const
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

QVariant FoldersModel::data(const QModelIndex &proxyIndex, int role) const
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


QVariant FoldersModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QMap<int, QVariant> FoldersModel::itemData(const QModelIndex &proxyIndex) const
{
    return QAbstractItemModel::itemData(proxyIndex);
}

Qt::ItemFlags FoldersModel::flags(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    return sourceIndex.model()->flags(sourceIndex);
}

bool FoldersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QModelIndex sourceIndex = mapToSource(index);
    return ((QAbstractItemModel*)sourceIndex.model())->setData(sourceIndex,value,role);
}

bool FoldersModel::setItemData(const QModelIndex &index, const QMap< int, QVariant >& roles)
{
    return QAbstractItemModel::setItemData(index, roles);
}

bool FoldersModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
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


QModelIndex FoldersModel::buddy(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    return mapFromSource(sourceIndex.model()->buddy(sourceIndex));
}

bool FoldersModel::insertColumns(int column, int count, const QModelIndex& parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    QModelIndex sourceIndex = mapToSource(parent);
    return ((QAbstractItemModel*)sourceIndex.model())->insertColumns(column, count, sourceIndex);
}

/*!
    \reimp
 */
bool FoldersModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    QModelIndex sourceIndex = mapToSource(parent);
    return ((QAbstractItemModel*)sourceIndex.model())->insertRows(row, count, sourceIndex);
}

bool FoldersModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    QModelIndex sourceIndex = mapToSource(parent);
    return ((QAbstractItemModel*)sourceIndex.model())->removeColumns(column, count, sourceIndex);
}

bool FoldersModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    if (!parent.isValid()) {
        return QAbstractItemModel::removeRows(row,count);
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return ((QAbstractItemModel*)sourceIndex.model())->removeRows(row, count, sourceIndex);
}

void FoldersModel::directoryChanged(const QString &path)
{
    foreach(SourceModel s, m_modelList) {
         if (s.watchPath == path && !QDir(s.rootPath).exists()) {
              this->removeRoot(s.rootIndex);
              break;
         }
    }
}

bool FoldersModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return false;
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return sourceIndex.model()->canFetchMore(sourceIndex);
}

void FoldersModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid()) {
        return;
    }
    QModelIndex sourceIndex = mapToSource(parent);
    QAbstractItemModel *model = (QAbstractItemModel*)sourceIndex.model();
    model->fetchMore(sourceIndex);
}

void FoldersModel::sort(int column, Qt::SortOrder order)
{
    foreach (SourceModel s, m_modelList) {
        s.model->sort(column,order);
    }
}

QSize FoldersModel::span(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QAbstractItemModel::span(index);
    }
    QModelIndex sourceIndex = mapToSource(index);
    return sourceIndex.model()->span(sourceIndex);
}

bool FoldersModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_modelList.size() > 0;
    }
    QModelIndex sourceIndex = mapToSource(parent);
    return sourceIndex.model()->hasChildren(sourceIndex);
}

void FoldersModel::sourceColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    this->beginInsertColumns(mapFromSource(parent), start, end);
}

void FoldersModel::sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == m_model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == m_model : true);
    this->beginMoveColumns(this->mapFromSource(sourceParent), sourceStart, sourceEnd, this->mapFromSource(destParent), dest);
}

void FoldersModel::sourceColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    this->beginRemoveColumns(this->mapFromSource(parent), start, end);
}

void FoldersModel::sourceColumnsInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->endInsertColumns();
}

void FoldersModel::sourceColumnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
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

void FoldersModel::sourceColumnsRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->endRemoveColumns();
}

void FoldersModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    //Q_ASSERT(topLeft.isValid() ? topLeft.model() == m_model : true);
    //Q_ASSERT(bottomRight.isValid() ? bottomRight.model() == m_model : true);
    this->dataChanged(this->mapFromSource(topLeft), this->mapFromSource(bottomRight));
}

void FoldersModel::sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    this->headerDataChanged(orientation, first, last);
}

void FoldersModel::sourceLayoutAboutToBeChanged()
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

void FoldersModel::sourceLayoutChanged()
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

void FoldersModel::sourceModelAboutToBeReset()
{
    this->beginResetModel();
}

void FoldersModel::sourceModelReset()
{
    this->endResetModel();
}

void FoldersModel::sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    this->beginInsertRows(this->mapFromSource(parent), start, end);
}

void FoldersModel::sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
{
    //Q_ASSERT(sourceParent.isValid() ? sourceParent.model() == m_model : true);
    //Q_ASSERT(destParent.isValid() ? destParent.model() == m_model : true);
    this->beginMoveRows(this->mapFromSource(sourceParent), sourceStart, sourceEnd, this->mapFromSource(destParent), dest);
}

void FoldersModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    this->beginRemoveRows(this->mapFromSource(parent), start, end);
}

void FoldersModel::sourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->endInsertRows();
}

void FoldersModel::sourceRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destParent, int dest)
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

void FoldersModel::sourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    //Q_ASSERT(parent.isValid() ? parent.model() == m_model : true);
    Q_UNUSED(parent)
    Q_UNUSED(start)
    Q_UNUSED(end)
    this->endRemoveRows();
}
