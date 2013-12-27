/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2014 LiteIDE Team. All rights reserved.
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
// Module: fifotty.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "fifotty.h"

#include <QFile>
#include <QSocketNotifier>
#include <QTemporaryFile>
#include <QVarLengthArray>
#include <QDebug>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef Q_OS_SOLARIS
# include <sys/filio.h> // FIONREAD
#endif
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


FiFoTty::FiFoTty(QObject *parent) :
    LiteApi::ITty(parent)
{
}

FiFoTty::~FiFoTty()
{
    shutdown();
}


QString FiFoTty::serverName() const
{
    return m_serverPath;
}

QString FiFoTty::errorString() const
{
    return m_errorString;
}

bool FiFoTty::listen()
{
    if (!m_serverPath.isEmpty())
        return true;
    if (!m_serverPath.isEmpty())
        return true;
    QByteArray codedServerPath;
    forever {
        {
            QTemporaryFile tf;
            if (!tf.open()) {
                m_errorString = tr("Cannot create temporary file: %1").arg(tf.errorString());
                m_serverPath.clear();
                return false;
            }
            m_serverPath = tf.fileName();
        }
        // By now the temp file was deleted again
        codedServerPath = QFile::encodeName(m_serverPath);
        if (!::mkfifo(codedServerPath.constData(), 0600))
            break;
        if (errno != EEXIST) {
            m_errorString = tr("Cannot create FiFo %1: %2").
                            arg(m_serverPath, QString::fromLocal8Bit(strerror(errno)));
            m_serverPath.clear();
            return false;
        }
    }
    if ((m_serverFd = ::open(codedServerPath.constData(), O_RDWR|O_NONBLOCK)) < 0) {
        m_errorString = tr("Cannot open FiFo %1: %2").
                        arg(m_serverPath, QString::fromLocal8Bit(strerror(errno)));
        m_serverPath.clear();
        return false;
    }
    m_serverNotifier = new QSocketNotifier(m_serverFd, QSocketNotifier::Read, this);
    connect(m_serverNotifier, SIGNAL(activated(int)), SLOT(bytesAvailable()));
    return true;
}

void FiFoTty::shutdown()
{
    if (!m_serverPath.isEmpty()) {
        ::close(m_serverFd);
        ::unlink(QFile::encodeName(m_serverPath).constData());
        delete m_serverNotifier;
        m_serverPath.clear();
    }
}

void FiFoTty::write(const QByteArray &data)
{
    ::write(m_serverFd,data.data(),data.length());
}

void FiFoTty::bytesAvailable()
{  
    size_t nbytes = 0;
    if (::ioctl(m_serverFd, FIONREAD, (char *) &nbytes) < 0)
        return;

    QByteArray buff(nbytes,0);
    if (::read(m_serverFd, buff.data(), nbytes) != (int)nbytes)
        return;

    if (nbytes) // Skip EOF notifications
        emit byteDelivery(buff);
}
