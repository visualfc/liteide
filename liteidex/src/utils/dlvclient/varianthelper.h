#ifndef VARIANTHELPER_H
#define VARIANTHELPER_H

#include <QVariant>
#include <QStringList>

class VariantHelper
{
public:
    static QMap<QString,QVariant> ObjectToVariantMap(const QObject *object, const QStringList &ignoredProperties = QStringList());
    static QMap<QString,QVariant> ObjectToVariant(const QObject *object, const QStringList &ignoredProperties = QStringList())
    {
        return ObjectToVariantMap(object,ignoredProperties);
    }
    static void VarintMapToObject(const QMap<QString,QVariant> &variantMap, QObject *object);
    static void VarintToQObject(const QVariant &variant, QObject *object)
    {
        VarintMapToObject(variant.toMap(),object);
    }
};

#endif // VARIANTHELPER_H
