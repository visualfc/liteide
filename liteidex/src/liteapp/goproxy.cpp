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
// Module: goproxy.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "goproxy.h"
#include "cdrv.h"
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

static GODRV_CALL godrv_call_fn = 0;

static int godrv_call(const QByteArray &id, const QByteArray &args, DRV_CALLBACK cb, void *ctx)
{
    if (godrv_call_fn == 0) {
        return -1;
    }
    return godrv_call_fn((char*)id.constData(),id.length(),(char*)args.constData(),args.length(),cb,ctx);
}

static void cdrv_callback(char *id, int id_size, char *reply, int len, int err, void *ctx)
{
    ((GoProxy*)(ctx))->callback(id,id_size,reply,len,err);
}

GoProxy::GoProxy(QObject *parent) :
    LiteApi::IGoProxy(parent)
{
    m_isRuning = false;
}

bool GoProxy::isValid() const
{
    return godrv_call_fn != 0;
}

bool GoProxy::hasProxy()
{
    return godrv_call_fn != 0;
}

bool GoProxy::isRunning() const
{
    return m_isRuning;
}

QByteArray GoProxy::commandId() const
{
    return m_id;
}

void GoProxy::writeStdin(const QByteArray &data)
{
    godrv_call("stdin",data,&cdrv_callback,this);
}

void GoProxy::call(const QByteArray &id, const QByteArray &args)
{
    m_id = id;
    m_isRuning = false;
    godrv_call(id,args,&cdrv_callback,this);
}

void GoProxy::callback(char *id, int id_size, char *reply, int reply_size, int flag)
{
    if (m_id != QByteArray(id,id_size)) {
        return;
    }
    if (flag == 0) {
        m_isRuning = true;
        emit started();
    } else if(flag == 1) {
        emit stdoutput(QByteArray(reply,reply_size));
    } else if (flag == 2) {
        emit stderror(QByteArray(reply,reply_size));
    } else if (flag == 3) {
        m_isRuning = false;
        emit finished(0,"");
    } else if (flag == 4) {
        m_isRuning = false;
        emit finished(2,QByteArray(reply,reply_size));
    }
}

void cdrv_init(void *fn)
{
    godrv_call_fn = (GODRV_CALL)fn;
}

void cdrv_cb(DRV_CALLBACK cb, char *id, int id_size, char *reply, int size, int err, void* ctx)
{
    cb(id,id_size,reply,size,err,ctx);
}
