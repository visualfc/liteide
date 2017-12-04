#ifndef QUICKOPENACTION_H
#define QUICKOPENACTION_H

#include "quickopenapi/quickopenapi.h"

class QStandardItemModel;
class QuickOpenAction : public LiteApi::IQuickOpen
{
public:
    QuickOpenAction(LiteApi::IApplication *app, QObject *parent = 0);
    virtual QString id() const;
    virtual QString info() const;
    virtual void activate();
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filterChanged(const QString &text);
    virtual void indexChanged(const QModelIndex &index);
    virtual bool selected(const QString &text, const QModelIndex &index);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel    *m_model;
};

#endif // QUICKOPENACTION_H
