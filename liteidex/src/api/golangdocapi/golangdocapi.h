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
// Module: golangdocapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOLANGDOCAPI_H
#define GOLANGDOCAPI_H

#include "liteapi/liteapi.h"

/*
openUrl(const QUrl &url);
url scheme
file : open html or plain file
list : url path only [pkg|cmd]
find : find pkg name
pdoc : show pkg doc
*/

namespace LiteApi {

enum PkgApiEnum {
    NullApi = 0,
    PkgApi = 0x0001,
    ConstApi = 0x0002,
    VarApi = 0x0004,
    StructApi = 0x0008,
    InterfaceApi = 0x0010,
    TypeApi = 0x0020,
    FuncApi = 0x0040,
    TypeMethodApi = 0x0080,
    TypeVarApi = 0x0100,
    AllTypeApi = StructApi | InterfaceApi | TypeApi,
    AllGolangApi = PkgApi | ConstApi | VarApi | StructApi | InterfaceApi | TypeApi | FuncApi | TypeMethodApi | TypeVarApi
};

class IGolangApi : public QObject
{
    Q_OBJECT
public:
    IGolangApi(QObject *parent) : QObject(parent) {}
public:
    virtual QStringList all(int flag = AllGolangApi) const = 0;
    virtual PkgApiEnum findExp(const QString &tag, QString &exp) const = 0;
    virtual QStringList findDocUrl(const QString &tag) const = 0;
    virtual QString findDocInfo(const QString &tag) const = 0;
};

class IGolangDoc : public IObject
{
    Q_OBJECT
public:
    IGolangDoc(QObject *parent) : IObject(parent) {}
public slots:
    virtual void openUrl(const QUrl &url, const QVariant &addin = QVariant()) = 0;
    virtual void activeBrowser() = 0;
};

inline IGolangDoc *getGolangDoc(LiteApi::IApplication *app)
{
    return LiteApi::findExtensionObject<IGolangDoc*>(app,"LiteApi.IGolangDoc");
}

}

#endif //GOLANGDOCAPI_H

