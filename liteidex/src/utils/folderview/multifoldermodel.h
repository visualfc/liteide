#ifndef MULTIFOLDERMODEL_H
#define MULTIFOLDERMODEL_H

#include "multiindexmodel.h"
#include <QDir>
#include <QMap>
#include <QFileSystemModel>

class MultiFolderModel : public MultiIndexModel
{
public:
    explicit MultiFolderModel(QObject* parent = 0);
    virtual ~MultiFolderModel();
public:
    QModelIndex addRootPath(const QString &path);
    void removeRootPath(const QString &path);
    void removeRoot(const QModelIndex &index);

    bool isRootPath(const QString &path) const;
    bool isRootIndex(const QModelIndex &index) const;
    void clearAll();
    void reloadAll();

    QList<QModelIndex> rootIndexs() const;
    QStringList rootPathList() const;

    QString filePath(const QModelIndex &index) const;
    QString fileName(const QModelIndex &index) const;
    QFileInfo fileInfo(const QModelIndex &index) const;

    bool isDir(const QModelIndex &index) const;
    qint64 size(const QModelIndex &index) const;
    QString type(const QModelIndex &index) const;
    QDateTime lastModified(const QModelIndex &index) const;
    QFile::Permissions permissions(const QModelIndex &index) const;

    bool remove(const QModelIndex &index);
    bool rmdir(const QModelIndex &index);

    QList<QModelIndex> indexForPath(const QString &path) const;
public:
    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;

    void setSorting(QDir::SortFlags sort);
    QDir::SortFlags sorting() const;

    void setResolveSymlinks(bool enable);
    bool resolveSymlinks() const;

    void setReadOnly(bool enable);
    bool isReadOnly() const;

    void setNameFilterDisables(bool enable);
    bool nameFilterDisables() const;

    void setNameFilters(const QStringList &filters);
    QStringList nameFilters() const;

public:
    virtual bool lessThan(const QAbstractItemModel *sourceModel, const QModelIndex &left, const QModelIndex &right) const;
protected:
    QDir::SortFlags m_sorts;
    QDir::Filters   m_filters;
    bool            m_resolveSymlinks;
    bool            m_isReadOnly;
    bool            m_nameFilterDisables;
    QStringList     m_nameFilters;
};

#endif // MULTIFOLDERMODEL_H
