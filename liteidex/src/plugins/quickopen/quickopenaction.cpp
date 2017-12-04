#include "quickopenaction.h"
#include <QStandardItemModel>
#include <QMenu>
#include <QAction>

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
    foreach (QString idMenu, manager->menuList()) {
        if (idMenu == "menu/recent") {
            continue;
        }
        QMenu *menu = manager->loadMenu(idMenu);
        if (!menu) {
            continue;
        }
        QAction *menuAct = menu->menuAction();
        if (!menuAct) {
            continue;
        }
        QMenu *realMenu = menuAct->menu();
        if (!realMenu) {
            continue;
        }
        QList<QAction*> actionList;
        foreach (QAction *act, realMenu->actions()) {
            if (act->isSeparator()) {
                continue;
            }
            QMenu *childMenu = act->menu();
            if (childMenu) {
                foreach (QAction *act, childMenu->actions()) {
                    if (act->isSeparator()) {
                        continue;
                    }
                    if (act->menu() != 0) {
                        continue;
                    }
                    actionList << act;
                }
            } else {
                actionList << act;
            }
        }
    }
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
