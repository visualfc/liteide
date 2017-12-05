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
// Module: golangsymbol.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGSYMBOL_H
#define GOLANGSYMBOL_H

#include <quickopenapi/quickopenapi.h>
#include <QProcess>

class QStandardItemModel;
class QSortFilterProxyModel;
class QProcess;
class GolangSymbol : public LiteApi::IQuickOpen
{
    Q_OBJECT
public:
    GolangSymbol(LiteApi::IApplication *app, QObject *parent = 0);
    virtual QString id() const;
    virtual QString info() const;
    virtual void activate();
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filterChanged(const QString &text);
    virtual void indexChanged(const QModelIndex &index);
    virtual bool selected(const QString &text, const QModelIndex &index);
    virtual void cancel();
    bool gotoIndex(const QModelIndex &index, bool saveHistroy);
public slots:
    void finished(int code,QProcess::ExitStatus status);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel    *m_model;
    QSortFilterProxyModel *m_proxy;
    QProcess              *m_process;
    Qt::CaseSensitivity    m_matchCase;
    bool                   m_importPath;
};

class GolangSymbolFactory : public LiteApi::IQuickOpenAdapter
{
public:
    GolangSymbolFactory(LiteApi::IApplication *app, QObject *parent = 0);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IQuickOpen *load(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
    GolangSymbol          *m_symbol;
};

#endif // GOLANGSYMBOL_H
