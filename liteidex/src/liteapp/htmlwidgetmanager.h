/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: htmlwidgetmanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-22
// $Id: htmlwidgetmanager.h,v 1.0 2012-11-22 visualfc Exp $

#ifndef HTMLWIDGETMANAGER_H
#define HTMLWIDGETMANAGER_H

#include "liteapi/litehtml.h"

using namespace LiteApi;

class HtmlWidgetManager : public IHtmlWidgetManager
{
    Q_OBJECT
public:
    explicit HtmlWidgetManager(QObject *parent = 0);
    virtual QStringList classNameList() const;
    virtual void addFactory(IHtmlWidgetFactory *factory);
    virtual QList<IHtmlWidgetFactory*> factoryList() const;
    virtual bool setDefaultClassName(const QString &className);
    virtual QString defaultClassName() const;
    virtual IHtmlWidget *create(QObject *parent);
    virtual IHtmlWidget *createByName(QObject *parent, const QString &className);
protected:
    QList<IHtmlWidgetFactory*> m_factoryList;
    QString m_defaultClassName;
};

#endif // HTMLWIDGETMANAGER_H
