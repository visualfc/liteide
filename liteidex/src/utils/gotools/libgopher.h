/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2018 LiteIDE Team. All rights reserved.
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
// Module: libgopher.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef LIBGOPHER_H
#define LIBGOPHER_H

#include <stddef.h> /* for ptrdiff_t below */
#include <QtGlobal>

#ifndef GO_CGO_EXPORT_PROLOGUE_H
#define GO_CGO_EXPORT_PROLOGUE_H

typedef struct { const char *p; ptrdiff_t n; } _GoString_;

#endif

/* Start of preamble from import "C" comments.  */

#ifndef GO_CGO_PROLOGUE_H
#define GO_CGO_PROLOGUE_H

typedef struct {const char *data; int size;} TString;
typedef signed char GoInt8;
typedef unsigned char GoUint8;
typedef short GoInt16;
typedef unsigned short GoUint16;
typedef int GoInt32;
typedef unsigned int GoUint32;
typedef long long GoInt64;
typedef unsigned long long GoUint64;

typedef qptrdiff GoInt;
typedef quintptr GoUint;

typedef quintptr GoUintptr;
typedef float GoFloat32;
typedef double GoFloat64;

typedef _GoString_ GoString;
typedef void *GoMap;
typedef void *GoChan;
typedef struct { void *t; void *v; } GoInterface;
typedef struct { void *data; GoInt len; GoInt cap; } GoSlice;

#endif

/* End of boilerplate cgo prologue.  */

#ifdef __cplusplus
extern "C" {
#endif

extern void Setenv(TString* p0, TString* p1);

extern void SetBuildEnv(TString* p0, TString* p1);

extern void ClearBuildEnv();

extern void InvokeAsync(TString* p0, TString* p1, TString* p2, TString* p3, void* p4, void* p5);

extern GoInt32 Invoke(GoString p0, GoString p1, GoString p2, GoString p3, GoString* p4, GoString* p5);

#ifdef __cplusplus
}
#endif


#endif // LIBGOPHER_H
