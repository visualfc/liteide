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
#ifndef QJSONRPCGLOBAL_H
#define QJSONRPCGLOBAL_H

#include <QtGlobal>
#include <QMetaType>

// error codes defined by spec
namespace QJsonRpc {
    enum ErrorCode {
        NoError         = 0,
        ParseError      = -32700,           // Invalid JSON was received by the server.
                                            // An error occurred on the server while parsing the JSON text.
        InvalidRequest  = -32600,           // The JSON sent is not a valid Request object.
        MethodNotFound  = -32601,           // The method does not exist / is not available.
        InvalidParams   = -32602,           // Invalid method parameter(s).
        InternalError   = -32603,           // Internal JSON-RPC error.
        ServerErrorBase = -32000,           // Reserved for implementation-defined server-errors.
        UserError       = -32099,           // Anything after this is user defined
        TimeoutError    = -32100
    };
}
Q_DECLARE_METATYPE(QJsonRpc::ErrorCode)

#define qJsonRpcDebug if (qgetenv("QJSONRPC_DEBUG").isEmpty()); else qDebug

#ifdef QJSONRPC_SHARED
#   ifdef QJSONRPC_BUILD
#       define QJSONRPC_EXPORT Q_DECL_EXPORT
#   else
#       define QJSONRPC_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define QJSONRPC_EXPORT
#endif

#endif
