/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2016 LiteIDE Team. All rights reserved.
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
// Module: actionmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class ActionContext : public IActionContext {
public:
    ActionContext(LiteApi::IApplication *app, const QString &name);
    virtual QString contextName() const;
    virtual ~ActionContext();
    virtual void regAction(QAction *act, const QString &id, const QString &defks, bool standard = false);
    virtual void regAction(QAction *act, const QString &id, const QKeySequence::StandardKey &def);
    virtual QStringList actionKeys() const;
    virtual ActionInfo *actionInfo(const QString &key) const;
    virtual void setActionShourtcuts(const QString &id, const QString &shortcuts);
protected:
    LiteApi::IApplication   *m_liteApp;
    QString m_name;
    QMap<QString,ActionInfo*> m_actionInfoMap;
};

class ActionManager : public IActionManager
{
    Q_OBJECT
public:
    ActionManager(QObject *parent = 0);
    virtual ~ActionManager();
    virtual bool initWithApp(IApplication *app);
    virtual QMenu *insertMenu(const QString &id, const QString &title, const QString &idBefore = QString());
    virtual QMenu *loadMenu(const QString &id);
    virtual void removeMenu(QMenu *menu);
    virtual QList<QString>  menuList() const;
    virtual QToolBar *insertToolBar(const QString &id, const QString &title, const QString &before = QString());
    virtual void insertToolBar(QToolBar *toolBar,const QString &before = QString());
    virtual QToolBar *loadToolBar(const QString &id);
    virtual void removeToolBar(QToolBar* toolBar);
    virtual QList<QString> toolBarList() const;
    virtual void insertViewMenu(VIEWMENU_ACTION_POS pos, QAction *act);
    virtual IActionContext *getActionContext(QObject *obj, const QString &name);
    virtual QStringList actionKeys() const;
    virtual ActionInfo *actionInfo(const QString &id) const;
    virtual void setActionShourtcuts(const QString &id, const QString &shortcuts);
    virtual QStringList actionContextNameList() const;
    virtual IActionContext *actionContextForName(const QString &name);
protected slots:
    void removeActionContext(QObject *obj);
public:
    static QList<QKeySequence> toShortcuts(const QString &ks);
    static QString formatShortcutsString(const QString &ks);
protected:
    QMap<QString,QMenu*>   m_idMenuMap;
    QMap<QString,QToolBar*> m_idToolBarMap;
    QMenu *m_viewMenu;
    QAction *m_baseToolBarAct;
    QAction *m_baseBrowserAct;
    QMap<QObject*,IActionContext*> m_objContextMap;
};

#endif // ACTIONMANAGER_H
