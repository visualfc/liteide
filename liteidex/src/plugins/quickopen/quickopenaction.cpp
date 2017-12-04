#include "quickopenaction.h"
#include <QStandardItemModel>

QuickOpenAction::QuickOpenAction(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
}

QString QuickOpenAction::id() const
{
    return "quickopen/action";
}

QString QuickOpenAction::info() const
{
    return "Show Menu Actions";
}

void QuickOpenAction::activate()
{

}

QAbstractItemModel *QuickOpenAction::model() const
{
    return m_model;
}

void QuickOpenAction::updateModel()
{
    m_model->clear();
    LiteApi::IActionManager *manager = m_liteApp->actionManager();
    // ("menu/build", "menu/debug", "menu/edit", "menu/file", "menu/find", "menu/help", "menu/recent", "menu/view")
    // ("App", "Build", "Find", "Editor", "Document", "Debug", "JsonEdit", "GoPkg", "GolangEdit", "GoFmt", "Bookmarks")
}

QModelIndex QuickOpenAction::filterChanged(const QString &text)
{
    return QModelIndex();
}

void QuickOpenAction::indexChanged(const QModelIndex &index)
{

}

bool QuickOpenAction::selected(const QString &text, const QModelIndex &index)
{
    return false;
}
