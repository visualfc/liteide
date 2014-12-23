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
// Module: cdrv.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef CDRV_H
#define CDRV_H

#include "liteapp_global.h"

typedef void (*DRV_CALLBACK)(char *id, char *reply, int len, int err, void *ctx);
typedef int (*GODRV_CALL)(char* id,int id_size, char* args, int args_size, DRV_CALLBACK cb, void *ctx);

extern "C"
int LITEIDESHARED_EXPORT cdrv_main(int argc, char **argv);

extern "C"
void LITEIDESHARED_EXPORT cdrv_init(void *fn);

extern "C"
void LITEIDESHARED_EXPORT cdrv_cb(DRV_CALLBACK cb, char *id, char *reply, int size, int err, void* ctx);


#endif // CDRV_H
