#include "quickopenhelp.h"
#include <QStandardItemModel>

QuickOpenHelp::QuickOpenHelp(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
}

QString QuickOpenHelp::id() const
{
    return "quickopen/help";
}

QString QuickOpenHelp::info() const
{
    return tr("Show Quick Open Help");
}

QAbstractItemModel *QuickOpenHelp::model() const
{
    return m_model;
}

void QuickOpenHelp::updateModel()
{
    LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
    m_model->clear();
    QMapIterator<QString,LiteApi::IQuickOpen*> i(mgr->filterMap());
    while(i.hasNext()) {
        i.next();
        m_model->appendRow(QList<QStandardItem*>() << new QStandardItem(i.key()) << new QStandardItem(i.value()->info()));
    }
}

QModelIndex QuickOpenHelp::filter(const QString &text)
{
    if (!text.isEmpty()) {
        LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
        LiteApi::IQuickOpen *p = mgr->findBySymbol(text);
        if (p) {
            mgr->showBySymbol(text);
        }
    }
    return m_model->index(0,0);
}

bool QuickOpenHelp::selected(const QString &text, const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QString key = index.data(0).toString();
    LiteApi::IQuickOpenManager *mgr = LiteApi::getQuickOpenManager(m_liteApp);
    mgr->showBySymbol(key);
    return false;
}
