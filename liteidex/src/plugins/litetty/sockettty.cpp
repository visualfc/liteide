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
// Module: sockettty.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "sockettty.h"
#include <QLocalServer>
#include <QLocalSocket>
#include <QCoreApplication>
#include <stdlib.h>
#include <time.h>
#include "memory.h"

SocketTty::SocketTty(QObject *parent) :
    LiteApi::ITty(parent), m_server(0),m_socket(0)
{
}

SocketTty::~SocketTty()
{
    shutdown();
}

QString SocketTty::serverName() const
{
    return m_server->fullServerName();
}

QString SocketTty::errorString() const
{
    return m_server->errorString();
}

bool SocketTty::listen()
{
    if (m_server) {
        return m_server->isListening();
    }
    m_server = new QLocalServer(this);
    srand(time(0));
    connect(m_server, SIGNAL(newConnection()), SLOT(newConnectionAvailable()));
    return m_server->listen(QString::fromLatin1("liteide-%1-%2")
                            .arg(QCoreApplication::applicationPid())
                            .arg(rand()));
}

void SocketTty::shutdown()
{
    if (m_server) {
        delete m_server;
        m_server = 0;
        m_socket = 0;
    }
}

void SocketTty::write(const QByteArray &data)
{
    m_socket->write(data);
}

void SocketTty::newConnectionAvailable()
{
    if (m_socket)
        return;
    m_socket = m_server->nextPendingConnection();
    connect(m_socket, SIGNAL(readyRead()), SLOT(bytesAvailable()));
}

void SocketTty::bytesAvailable()
{
    emit byteDelivery(m_socket->readAll());
}
