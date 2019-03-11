/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 visualfc. All rights reserved.
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
// Module: quickopenhelp.h
// Creator: visualfc <visualfc@gmail.com>

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
    virtual QString placeholderText() const;
    virtual void activate();
    virtual QAbstractItemModel *model() const;
    virtual QModelIndex rootIndex() const;
    virtual void updateModel();
    virtual QModelIndex filterChanged(const QString &text);
    virtual void indexChanged(const QModelIndex &index);
    virtual bool selected(const QString &text, const QModelIndex &index);
    virtual void cancel();
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel    *m_model;
    bool update(const QString &text);
};

#endif // QUICKOPENHELP_H
