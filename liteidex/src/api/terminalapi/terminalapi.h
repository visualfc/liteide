/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2020 LiteIDE. All rights reserved.
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
// Module: terminalapi.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef TERMINALAPI_H
#define TERMINALAPI_H

#include "liteapi/liteapi.h"

class QTreeView;
class QLineEdit;
namespace LiteApi {

class ITerminal : public QObject
{
    Q_OBJECT
public:
    ITerminal(QObject *parent = 0) : QObject(parent) {}
    virtual void openDefaultTerminal(const QString &workDir) = 0;
};

inline ITerminal* getTerminalManager(LiteApi::IApplication *app)
{
    return LiteApi::findExtensionObject<ITerminal*>(app,"LiteApi.ITerminal");
}

} //namespace LiteApi


#endif //TERMINALAPI_H

