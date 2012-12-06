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
// Module: goproxy.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-17
// $Id: goproxy.cpp,v 1.0 2012-11-17 visualfc Exp $

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

static void cdrv_callback(char *id, char *reply, int len, int err, void *ctx)
{
    ((GoProxy*)(ctx))->callback(id,reply,len,err);
}

GoProxy::GoProxy(QObject *parent) :
    LiteApi::IGoProxy(parent)
{
}

bool GoProxy::isValid() const
{
    return godrv_call_fn != 0;
}

bool GoProxy::hasProxy()
{
    return godrv_call_fn != 0;
}

void GoProxy::call(const QByteArray &id, const QByteArray &args)
{    
    godrv_call(id,args,&cdrv_callback,this);
}

void GoProxy::callback(char *id, char *reply, int len, int err)
{
    if (err != 0) {
        emit error(id,err);
    } else {
        emit done(id,QByteArray(reply,len));
    }
}

void cdrv_init(void *fn)
{
    godrv_call_fn = (GODRV_CALL)fn;
}

void cdrv_cb(DRV_CALLBACK cb, char *id, char *reply, int size, int err, void* ctx)
{
    cb(id,reply,size,err,ctx);
}
