#ifndef ABSTRACTMULTIPROXYMODEL_P_H
#define ABSTRACTMULTIPROXYMODEL_P_H

#include <QAbstractItemModel>
#include "abstractmultiproxymodel.h"

class AbstractMultiProxyModel;
class AbstractMultiProxyModelPrivate : public QObject //: public QAbstractItemModelPrivate
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(AbstractMultiProxyModel)
public:
    AbstractMultiProxyModelPrivate() {}
    QList<SourceModelIndex> indexList;
    inline bool indexValid(const QModelIndex &index) const {
         return (index.row() >= 0) && (index.column() >= 0) && (index.model() == q_func());
    }
    inline bool containsModel(QAbstractItemModel *model)
    {
        foreach (SourceModelIndex index, indexList) {
            if (index.model == model) {
                return true;
            }
        }
        return false;
    }
public slots:
    virtual void _q_sourceModelDestroyed();
public:
    QAbstractItemModel *q_ptr;
};

#endif // ABSTRACTMULTIPROXYMODEL_P_H

