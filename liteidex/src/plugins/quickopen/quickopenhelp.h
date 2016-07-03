#ifndef QUICKOPENHELP_H
#define QUICKOPENHELP_H

#include "quickopenapi/quickopenapi.h"

class QStandardItemModel;
class QuickOpenHelp : public LiteApi::IQuickOpen
{
    Q_OBJECT
public:
    QuickOpenHelp(LiteApi::IApplication *app, QObject *parent);
    virtual QString id() const;
    virtual QString info() const;
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filter(const QString &text);
    virtual bool selected(const QString &text, const QModelIndex &index);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel    *m_model;
    bool update(const QString &text);
};

#endif // QUICKOPENHELP_H
