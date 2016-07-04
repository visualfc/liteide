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
class GolangSymbol : public LiteApi::IDocumentSymbol
{
    Q_OBJECT
public:
    GolangSymbol(LiteApi::IApplication *app, QObject *parent = 0);
    virtual QString mimeType() const;
    virtual QAbstractItemModel *model() const;
    virtual void updateModel();
    virtual QModelIndex filter(const QString &text);
    virtual bool selected(const QString &text, const QModelIndex &index);
public slots:
    void finished(int code,QProcess::ExitStatus status);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel    *m_model;
    QSortFilterProxyModel *m_proxy;
    QProcess              *m_process;
};

class GolangSymbolFactory : public LiteApi::IDocumentSymbolFactory
{
public:
    GolangSymbolFactory(LiteApi::IApplication *app, QObject *parent = 0);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IDocumentSymbol *load(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
    GolangSymbol          *m_symbol;
};

#endif // GOLANGSYMBOL_H
