#include "varianthelper.h"

#include <QString>
#include <QVariant>
#include <QMap>
#include <QMetaProperty>
#include <QStringList>
#include <QDebug>

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
