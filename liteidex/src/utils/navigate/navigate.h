/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: navigate.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef NAVIGATE_H
#define NAVIGATE_H

#include "liteapi/liteapi.h"

class NavigateBar : public QObject
{
    Q_OBJECT
public:
    NavigateBar(LiteApi::IApplication *app, QObject *parent);
    virtual ~NavigateBar();
    QToolBar* createToolBar(const QString &title, QWidget *parent);
    void LoadPath(const QString &path);
    QToolBar *toolBar() const {
        return  m_toolBar;
    }
public slots:
    void pathLinkActivated(const QString &path);
    void quickPathLinkActivated(const QString &path);
protected:
    QToolBar *createNavToolBar(QWidget *parent);
protected:
    LiteApi::IApplication *m_liteApp;
    QToolBar *m_toolBar;
    QAction *m_navHeadAct;
    QString m_filePath;
};

#endif // NAVIGATE_H
