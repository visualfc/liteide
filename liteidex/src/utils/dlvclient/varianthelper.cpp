/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2017 LiteIDE. All rights reserved.
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
// Module: varianthelper.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "varianthelper.h"

#include <QString>
#include <QVariant>
#include <QMap>
#include <QMetaProperty>
#include <QStringList>
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

QMap<QString,QVariant> VariantHelper::ObjectToVariantMap( const QObject* object,
                              const QStringList& ignoredProperties)
{
  QMap<QString,QVariant> result;
  const QMetaObject *metaobject = object->metaObject();
  int count = metaobject->propertyCount();
  for (int i=0; i<count; ++i) {
    QMetaProperty metaproperty = metaobject->property(i);
    const char *name = metaproperty.name();

    if (ignoredProperties.contains(QLatin1String(name)) || (!metaproperty.isReadable()))
      continue;

    QVariant value = object->property(name);

    result[QLatin1String(name)] = value;
 }
  return result;
}

void VariantHelper::VarintMapToObject(const QMap<QString, QVariant> &variantMap, QObject *object)
{
    const QMetaObject *metaobject = object->metaObject();

    QMap<QString,QVariant>::const_iterator iter;
    for (iter = variantMap.constBegin(); iter != variantMap.constEnd(); ++iter) {
        object->setProperty(iter.key().toLatin1(),iter.value());

        int pIdx = metaobject->indexOfProperty( iter.key().toLatin1() );

        if ( pIdx < 0 ) {
            qDebug() << "waring not find property key" << iter.key();
            object->setProperty(iter.key().toLatin1(),iter.value());
            continue;
        }

        QMetaProperty metaproperty = metaobject->property( pIdx );
        QVariant::Type type = metaproperty.type();
        QVariant v( iter.value() );
        if ( v.canConvert( type ) ) {
            v.convert( type );
            metaproperty.write( object, v );
        } else if (QString(QLatin1String("QVariant")).compare(QLatin1String(metaproperty.typeName())) == 0) {
            metaproperty.write( object, v );
        }
    }
}
