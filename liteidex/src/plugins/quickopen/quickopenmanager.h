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
// Module: quickopenmanager.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef QUICKOPENMANAGER_H
#define QUICKOPENMANAGER_H

#include "liteapi/liteapi.h"
#include "quickopenapi/quickopenapi.h"
#include "quickopenwidget.h"

#include <QPointer>

using namespace LiteApi;

class QuickOpenFiles;
class QuickOpenManager : public IQuickOpenManager
{
    Q_OBJECT
public:
    QuickOpenManager(QObject *parent = 0);
    virtual ~QuickOpenManager();
    virtual bool initWithApp(IApplication *app);
public:
    virtual void addFilter(const QString &sym, IQuickOpen *filter);
    virtual void removeFilter(IQuickOpen *filter);
    virtual QList<IQuickOpen*> filterList() const;
    virtual void setCurrentFilter(IQuickOpen *filter);
    virtual IQuickOpen *currentFilter() const;
    virtual QModelIndex currentIndex() const;
public slots:
    void showQuickOpen();
    void hideQuickOpen();
    void filterChanged(const QString &text);
    void selected();
protected:
    QuickOpenWidget *m_widget;
    QAction     *m_quickOpenAct;
    QMap<QString,IQuickOpen*> m_filterMap;
    QString m_sym;
    QPointer<IQuickOpen> m_currentFilter;
    QuickOpenFiles *m_quickOpenFiles;
};

#endif // QUICKOPENMANAGER_H
