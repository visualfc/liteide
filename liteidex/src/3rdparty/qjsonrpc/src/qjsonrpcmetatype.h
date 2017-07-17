/*
 * Copyright (C) 2012-2013 Matt Broadstone
 * Copyright (C) 2013 Fargier Sylvain
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
#ifndef QJSONRPCMETATYPE_H
#define QJSONRPCMETATYPE_H

#include <QMetaType>

template <typename T>
void qRegisterJsonRpcMetaType(const char *typeName, T * = 0)
{
    Q_UNUSED(typeName)
    QMetaType::registerConverter<T, QJsonValue>(&T::toJson);
    QMetaType::registerConverter<QJsonValue, T>(&T::fromJson);
}

#endif
