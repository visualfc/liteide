#ifndef QUICKOPENLINES_H
#define QUICKOPENLINES_H

#include "../api/quickopenapi/quickopenapi.h"

class QStandardItemModel;
class QuickOpenLines : public LiteApi::IQuickOpen
{
public:
    QuickOpenLines(LiteApi::IApplication *app, QObject *parent);
    virtual QString id() const;
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filter(const QString &text);
    virtual bool selected(const QString &text, const QModelIndex &index);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel    *m_model;
    bool update(const QString &text);
};

#endif // QUICKOPENLINES_H
