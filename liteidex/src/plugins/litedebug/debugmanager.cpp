/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2015 LiteIDE Team. All rights reserved.
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
// Module: debugmanager.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "debugmanager.h"
#include "memory.h"

DebugManager::DebugManager(QObject *parent) :
    IDebuggerManager(parent),
    m_currentDebug(0)
{
}

DebugManager::~DebugManager()
{
    qDeleteAll(m_debugList);
}

void DebugManager::addDebugger(IDebugger *debug)
{
    m_debugList.append(debug);
}

void DebugManager::removeDebugger(IDebugger *debug)
{
    m_debugList.removeOne(debug);
}

IDebugger *DebugManager::findDebugger(const QString &mimeType)
{
    foreach(IDebugger *debug, m_debugList) {
        if (debug->mimeType() == mimeType) {
            return debug;
        }
    }
    return 0;
}

QList<IDebugger*> DebugManager::debuggerList() const
{
    return m_debugList;
}

void DebugManager::setCurrentDebugger(IDebugger *debug)
{
    m_currentDebug = debug;
    emit currentDebuggerChanged(m_currentDebug);
}

IDebugger *DebugManager::currentDebugger()
{
    return m_currentDebug;
}
