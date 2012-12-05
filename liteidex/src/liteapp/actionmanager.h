/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// date: 2011-3-26
// $Id: actionmanager.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;


class ActionManager : public IActionManager
{
    Q_OBJECT
public:
    ActionManager(QObject *parent = 0);
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
protected:
    QMap<QString,QMenu*>   m_idMenuMap;
    QMap<QString,QToolBar*> m_idToolBarMap;
    QMenu *m_viewMenu;
    QMenu *m_viewToolMenu;
    QAction *m_baseToolBarAct;
    QAction *m_baseBrowserAct;
};

#endif // ACTIONMANAGER_H
