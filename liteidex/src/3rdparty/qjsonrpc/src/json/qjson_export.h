/*
 * Copyright (C) 2012-2013 Matt Broadstone
 * Contact: http://bitbucket.org/devonit/qjsonrpc
 *
 * This file is part of the QJsonRpc Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */
#ifndef QJSON_EXPORT_H
#define QJSON_EXPORT_H

#ifdef QJSONRPC_SHARED
#   ifdef QJSONRPC_BUILD
#       define QJSON_EXPORT Q_DECL_EXPORT
#   else
#       define QJSON_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define QJSON_EXPORT
#endif

#endif
