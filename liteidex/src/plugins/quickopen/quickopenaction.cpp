/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: quickopenaction.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "quickopenaction.h"
#include <QStandardItem>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QAction>
#include <QTimer>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QuickOpenAction::QuickOpenAction(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IQuickOpen(parent), m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(0);
}

QString QuickOpenAction::id() const
{
    return "quickopen/command";
}

QString QuickOpenAction::info() const
{
    return tr("Show and Run Commands");
}

QString QuickOpenAction::placeholderText() const
{
    return QString();
}

void QuickOpenAction::activate()
{

}

QAbstractItemModel *QuickOpenAction::model() const
{
    return m_proxyModel;
}

QModelIndex QuickOpenAction::rootIndex() const
{
    return QModelIndex();
}

static QString makeTitle(const QString &id)
{
    return id[0].toUpper()+id.mid(1);
}

static QString makeMenuName(const QString &idMenu)
{
    if (idMenu.startsWith("menu/")) {
        return makeTitle(idMenu.mid(5));
    }
    return makeTitle(idMenu);
}

void QuickOpenAction::updateModel()
{
    m_model->clear();
    m_itemActionMap.clear();
    m_proxyModel->setFilterFixedString("");

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
        QString menuName = makeMenuName(idMenu);
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
        foreach (QAction *act, actionList) {
            if (!act->isEnabled()) {
                continue;
            }
            if (act->data().isNull()) {
                continue;
            }
            QString text = act->data().toString();
            if (text.isEmpty()) {
                continue;
            }
            QStandardItem *item = new QStandardItem(menuName+"."+text);
            QStandardItem *info = new QStandardItem();
            if (text != act->text()) {
                info->setText(act->text());
            }
            QStandardItem *shortcut = new QStandardItem(act->shortcut().toString(QKeySequence::NativeText));
            m_model->appendRow(QList<QStandardItem*>() << item << info << shortcut);
            m_itemActionMap.insert(item,act);
        }
    }
}

QModelIndex QuickOpenAction::filterChanged(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);

    for(int i = 0; i < m_proxyModel->rowCount(); i++) {
        QModelIndex index = m_proxyModel->index(i,0);
        QString name = index.data().toString();
        if (name.startsWith(text,Qt::CaseInsensitive)) {
            return index;
        }
    }
    if (m_proxyModel->rowCount() > 0)
        return m_proxyModel->index(0,0);
    return QModelIndex();
}

void QuickOpenAction::indexChanged(const QModelIndex &/*index*/)
{

}

bool QuickOpenAction::selected(const QString &/*text*/, const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
    QStandardItem *item = m_model->item(sourceIndex.row(),0);
    if (item) {
        QAction *act = m_itemActionMap.value(item);
        if (act) {
            QTimer::singleShot(1,act,SLOT(trigger()));
            return true;
        }
    }
    return false;
}

void QuickOpenAction::cancel()
{

}
