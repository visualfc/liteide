#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QAbstractProxyModel>

namespace Core {

class ProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit ProxyModel(QObject *parent = 0);
    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void setSourceModel(QAbstractItemModel *sourceModel);

    // QAbstractProxyModel::sibling is broken in Qt 5
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const;
    // QAbstractProxyModel::supportedDragActions delegation is missing in Qt 5
    Qt::DropActions supportedDragActions() const;

private slots:
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void sourceRowsRemoved(const QModelIndex &parent, int start, int end);
    void sourceRowsInserted(const QModelIndex &parent, int start, int end);
    void sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
};

}

#endif // PROXYMODEL_H
