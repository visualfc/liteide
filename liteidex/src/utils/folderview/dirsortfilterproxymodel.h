#ifndef DIRSORTFILTERPROXYMODEL_H
#define DIRSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDir>

class DirSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    DirSortFilterProxyModel(QObject *parent);
    void setSorting(QDir::SortFlags sort);
    QDir::SortFlags sorting() const;
protected:
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
protected:
    QDir::SortFlags m_sorts;
};

#endif // DIRSORTFILTERPROXYMODEL_H
