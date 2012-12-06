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
// Module: debugmanager.h
// Creator: visualfc <visualfc@gmail.com>



#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H

#include "litedebugapi/litedebugapi.h"

using namespace LiteApi;

class DebugManager : public LiteApi::IDebuggerManager
{
    Q_OBJECT
public:
    DebugManager(QObject *parent);
    virtual ~DebugManager();
    virtual void addDebugger(IDebugger *debug);
    virtual void removeDebugger(IDebugger *debug);
    virtual IDebugger *findDebugger(const QString &mimeType);
    virtual QList<IDebugger*> debuggerList() const;
    virtual void setCurrentDebugger(IDebugger *debug);
    virtual IDebugger *currentDebugger();
protected:
    QList<IDebugger*>  m_debugList;
    IDebugger *m_currentDebug;
};

#endif // DEBUGMANAGER_H
