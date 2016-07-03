#ifndef QUICKOPENEDITOR_H
#define QUICKOPENEDITOR_H

#include "quickopenapi/quickopenapi.h"

class QStandardItemModel;
class QSortFilterProxyModel;
class QuickOpenEditor : public LiteApi::IQuickOpen
{
    Q_OBJECT
public:
    QuickOpenEditor(LiteApi::IApplication *app, QObject *parent = 0);
    virtual QString id() const;
    virtual QString info() const;
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filter(const QString &text);
    virtual bool selected(const QString &text, const QModelIndex &index);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
};

#endif // QUICKOPENEDITOR_H
