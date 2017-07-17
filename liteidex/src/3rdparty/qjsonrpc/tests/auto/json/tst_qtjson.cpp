/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest>

#include "json/qjsonarray.h"
#include "json/qjsonobject.h"
#include "json/qjsonvalue.h"
#include "json/qjsondocument.h"
#include <limits>

#define INVALID_UNICODE "\xCE\xBA\xE1"
#define UNICODE_NON_CHARACTER "\xEF\xBF\xBF"
#define UNICODE_DJE "\320\202" // Character from the Serbian Cyrillic alphabet

class tst_QtJson: public QObject
{
    Q_OBJECT
public:
    tst_QtJson(QObject *parent = 0);

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testValueSimple();
    void testNumbers();
    void testNumbers_2();
    void testNumbers_3();

    void testObjectSimple();
    void testObjectSmallKeys();
    void testArraySimple();
    void testValueObject();
    void testValueArray();
    void testObjectNested();
    void testArrayNested();
    void testArrayNestedEmpty();
    void testArrayComfortOperators();
    void testObjectNestedEmpty();

    void testValueRef();
    void testObjectIteration();
    void testArrayIteration();

    void testObjectFind();

    void testDocument();

    void nullValues();
    void nullArrays();
    void nullObject();
    void constNullObject();

    void keySorting();

    void undefinedValues();

    void fromVariant();
    void fromVariantMap();
    void toVariantMap();
    void toVariantList();

    void toJson();
    void toJsonSillyNumericValues();
    void toJsonLargeNumericValues();
    void fromJson();
    void fromJsonErrors();
    void fromBinary();
    void toAndFromBinary_data();
    void toAndFromBinary();
    void parseNumbers();
    void parseStrings();
    void parseDuplicateKeys();
    void testParser();

    void compactArray();
    void compactObject();

    void validation();

    void assignToDocument();

    void testDuplicateKeys();
    void testCompaction();
    void testDebugStream();
    void testCompactionError();

    void parseUnicodeEscapes();

    void assignObjects();
    void assignArrays();

    void testTrailingComma();
    void testDetachBug();
    void testJsonValueRefDefault();

    void valueEquals();
    void objectEquals_data();
    void objectEquals();
    void arrayEquals_data();
    void arrayEquals();

    void bom();
    void nesting();

    void longStrings();

    void arrayInitializerList();
    void objectInitializerList();

    void unicodeKeys();
    void garbageAtEnd();
};

tst_QtJson::tst_QtJson(QObject *parent) : QObject(parent)
{
}

void tst_QtJson::initTestCase()
{
}

void tst_QtJson::cleanupTestCase()
{
}

void tst_QtJson::init()
{
}

void tst_QtJson::cleanup()
{
}

void tst_QtJson::testValueSimple()
{
    QJsonObject object;
    object.insert("number", 999.);
    QJsonArray array;
    for (int i = 0; i < 10; ++i)
        array.append((double)i);

    QJsonValue value(true);
    QCOMPARE(value.type(), QJsonValue::Bool);
    QCOMPARE(value.toDouble(), 0.);
    QCOMPARE(value.toString(), QString());
    QCOMPARE(value.toBool(), true);
    QCOMPARE(value.toObject(), QJsonObject());
    QCOMPARE(value.toArray(), QJsonArray());
    QCOMPARE(value.toDouble(99.), 99.);
    QCOMPARE(value.toString(QString("test")), QString("test"));
    QCOMPARE(value.toObject(object), object);
    QCOMPARE(value.toArray(array), array);

    value = 999.;
    QCOMPARE(value.type(), QJsonValue::Double);
    QCOMPARE(value.toDouble(), 999.);
    QCOMPARE(value.toString(), QString());
    QCOMPARE(value.toBool(), false);
    QCOMPARE(value.toBool(true), true);
    QCOMPARE(value.toObject(), QJsonObject());
    QCOMPARE(value.toArray(), QJsonArray());

    value = QLatin1String("test");
    QCOMPARE(value.toDouble(), 0.);
    QCOMPARE(value.toString(), QLatin1String("test"));
    QCOMPARE(value.toBool(), false);
    QCOMPARE(value.toObject(), QJsonObject());
    QCOMPARE(value.toArray(), QJsonArray());
}

void tst_QtJson::testNumbers()
{
    {
        int numbers[] = {
            0,
            -1,
            1,
            (1<<26),
            (1<<27),
            (1<<28),
            -(1<<26),
            -(1<<27),
            -(1<<28),
            (1<<26) - 1,
            (1<<27) - 1,
            (1<<28) - 1,
            -((1<<26) - 1),
            -((1<<27) - 1),
            -((1<<28) - 1)
        };
        int n = sizeof(numbers)/sizeof(int);

        QJsonArray array;
        for (int i = 0; i < n; ++i)
            array.append((double)numbers[i]);

        QByteArray serialized = QJsonDocument(array).toJson();
        QJsonDocument json = QJsonDocument::fromJson(serialized);
        QJsonArray array2 = json.array();

        QCOMPARE(array.size(), array2.size());
        for (int i = 0; i < array.size(); ++i) {
            QCOMPARE(array.at(i).type(), QJsonValue::Double);
            QCOMPARE(array.at(i).toDouble(), (double)numbers[i]);
            QCOMPARE(array2.at(i).type(), QJsonValue::Double);
            QCOMPARE(array2.at(i).toDouble(), (double)numbers[i]);
        }
    }

    {
        qint64 numbers[] = {
            0,
            -1,
            1,
            (1ll<<54),
            (1ll<<55),
            (1ll<<56),
            -(1ll<<54),
            -(1ll<<55),
            -(1ll<<56),
            (1ll<<54) - 1,
            (1ll<<55) - 1,
            (1ll<<56) - 1,
            -((1ll<<54) - 1),
            -((1ll<<55) - 1),
            -((1ll<<56) - 1)
        };
        int n = sizeof(numbers)/sizeof(qint64);

        QJsonArray array;
        for (int i = 0; i < n; ++i)
            array.append((double)numbers[i]);

        QByteArray serialized = QJsonDocument(array).toJson();
        QJsonDocument json = QJsonDocument::fromJson(serialized);
        QJsonArray array2 = json.array();

        QCOMPARE(array.size(), array2.size());
        for (int i = 0; i < array.size(); ++i) {
            QCOMPARE(array.at(i).type(), QJsonValue::Double);
            QCOMPARE(array.at(i).toDouble(), (double)numbers[i]);
            QCOMPARE(array2.at(i).type(), QJsonValue::Double);
            QCOMPARE(array2.at(i).toDouble(), (double)numbers[i]);
        }
    }

    {
        double numbers[] = {
            0,
            -1,
            1,
            double(1ll<<54),
            double(1ll<<55),
            double(1ll<<56),
            double(-(1ll<<54)),
            double(-(1ll<<55)),
            double(-(1ll<<56)),
            double((1ll<<54) - 1),
            double((1ll<<55) - 1),
            double((1ll<<56) - 1),
            double(-((1ll<<54) - 1)),
            double(-((1ll<<55) - 1)),
            double(-((1ll<<56) - 1)),
            1.1,
            0.1,
            -0.1,
            -1.1,
            1e200,
            -1e200
        };
        int n = sizeof(numbers)/sizeof(double);

        QJsonArray array;
        for (int i = 0; i < n; ++i)
            array.append(numbers[i]);

        QByteArray serialized = QJsonDocument(array).toJson();
        QJsonDocument json = QJsonDocument::fromJson(serialized);
        QJsonArray array2 = json.array();

        QCOMPARE(array.size(), array2.size());
        for (int i = 0; i < array.size(); ++i) {
            QCOMPARE(array.at(i).type(), QJsonValue::Double);
            QCOMPARE(array.at(i).toDouble(), numbers[i]);
            QCOMPARE(array2.at(i).type(), QJsonValue::Double);
            QCOMPARE(array2.at(i).toDouble(), numbers[i]);
        }
    }

}

void tst_QtJson::testNumbers_2()
{
    // test cases from TC39 test suite for ECMAScript
    // http://hg.ecmascript.org/tests/test262/file/d067d2f0ca30/test/suite/ch08/8.5/8.5.1.js

    // Fill an array with 2 to the power of (0 ... -1075)
    double value = 1;
    double floatValues[1076], floatValues_1[1076];
    QJsonObject jObject;
    for (int power = 0; power <= 1075; power++) {
        floatValues[power] = value;
        jObject.insert(QString::number(power), QJsonValue(floatValues[power]));
        // Use basic math operations for testing, which are required to support 'gradual underflow' rather
        // than Math.pow etc..., which are defined as 'implementation dependent'.
        value = value * 0.5;
    }

    QJsonDocument jDocument1(jObject);
    QByteArray ba(jDocument1.toJson());

    QJsonDocument jDocument2(QJsonDocument::fromJson(ba));
    for (int power = 0; power <= 1075; power++) {
        floatValues_1[power] = jDocument2.object().value(QString::number(power)).toDouble();
#ifdef Q_OS_QNX
        if (power >= 970)
            QEXPECT_FAIL("", "See QTBUG-37066", Abort);
#endif
        QVERIFY2(floatValues[power] == floatValues_1[power], QString("floatValues[%1] != floatValues_1[%1]").arg(power).toLatin1());
    }

    // The last value is below min denorm and should round to 0, everything else should contain a value
    QVERIFY2(floatValues_1[1075] == 0, "Value after min denorm should round to 0");

    // Validate the last actual value is min denorm
    QVERIFY2(floatValues_1[1074] == 4.9406564584124654417656879286822e-324, QString("Min denorm value is incorrect: %1").arg(floatValues_1[1074]).toLatin1());

    // Validate that every value is half the value before it up to 1
    for (int index = 1074; index > 0; index--) {
        QVERIFY2(floatValues_1[index] != 0, QString("2**- %1 should not be 0").arg(index).toLatin1());

        QVERIFY2(floatValues_1[index - 1] == (floatValues_1[index] * 2), QString("Value should be double adjacent value at index %1").arg(index).toLatin1());
    }
}

void tst_QtJson::testNumbers_3()
{
    // test case from QTBUG-31926
    double d1 = 1.123451234512345;
    double d2 = 1.123451234512346;

    QJsonObject jObject;
    jObject.insert("d1", QJsonValue(d1));
    jObject.insert("d2", QJsonValue(d2));
    QJsonDocument jDocument1(jObject);
    QByteArray ba(jDocument1.toJson());

    QJsonDocument jDocument2(QJsonDocument::fromJson(ba));

    double d1_1(jDocument2.object().value("d1").toDouble());
    double d2_1(jDocument2.object().value("d2").toDouble());
    QVERIFY(d1_1 != d2_1);
}

void tst_QtJson::testObjectSimple()
{
    QJsonObject object;
    object.insert("number", 999.);
    QCOMPARE(object.value("number").type(), QJsonValue::Double);
    QCOMPARE(object.value("number").toDouble(), 999.);
    object.insert("string", QString::fromLatin1("test"));
    QCOMPARE(object.value("string").type(), QJsonValue::String);
    QCOMPARE(object.value("string").toString(), QString("test"));
    object.insert("boolean", true);
    QCOMPARE(object.value("boolean").toBool(), true);

    QStringList keys = object.keys();
    QVERIFY2(keys.contains("number"), "key number not found");
    QVERIFY2(keys.contains("string"), "key string not found");
    QVERIFY2(keys.contains("boolean"), "key boolean not found");

    // if we put a JsonValue into the JsonObject and retrieve
    // it, it should be identical.
    QJsonValue value(QLatin1String("foo"));
    object.insert("value", value);
    QCOMPARE(object.value("value"), value);

    int size = object.size();
    object.remove("boolean");
    QCOMPARE(object.size(), size - 1);
    QVERIFY2(!object.contains("boolean"), "key boolean should have been removed");

    QJsonValue taken = object.take("value");
    QCOMPARE(taken, value);
    QVERIFY2(!object.contains("value"), "key value should have been removed");

    QString before = object.value("string").toString();
    object.insert("string", QString::fromLatin1("foo"));
    QVERIFY2(object.value("string").toString() != before, "value should have been updated");

    size = object.size();
    QJsonObject subobject;
    subobject.insert("number", 42);
    subobject.insert(QLatin1String("string"), QLatin1String("foobar"));
    object.insert("subobject", subobject);
    QCOMPARE(object.size(), size+1);
    QJsonValue subvalue = object.take(QLatin1String("subobject"));
    QCOMPARE(object.size(), size);
    QCOMPARE(subvalue.toObject(), subobject);
    // make object detach by modifying it many times
    for (int i = 0; i < 64; ++i)
        object.insert(QLatin1String("string"), QLatin1String("bar"));
    QCOMPARE(object.size(), size);
    QCOMPARE(subvalue.toObject(), subobject);
}

void tst_QtJson::testObjectSmallKeys()
{
    QJsonObject data1;
    data1.insert(QLatin1String("1"), 123);
    QVERIFY(data1.contains(QLatin1String("1")));
    QCOMPARE(data1.value(QLatin1String("1")).toDouble(), (double)123);
    data1.insert(QLatin1String("12"), 133);
    QCOMPARE(data1.value(QLatin1String("12")).toDouble(), (double)133);
    QVERIFY(data1.contains(QLatin1String("12")));
    data1.insert(QLatin1String("123"), 323);
    QCOMPARE(data1.value(QLatin1String("12")).toDouble(), (double)133);
    QVERIFY(data1.contains(QLatin1String("123")));
    QCOMPARE(data1.value(QLatin1String("123")).type(), QJsonValue::Double);
    QCOMPARE(data1.value(QLatin1String("123")).toDouble(), (double)323);
}

void tst_QtJson::testArraySimple()
{
    QJsonArray array;
    array.append(999.);
    array.append(QString::fromLatin1("test"));
    array.append(true);

    QJsonValue val = array.at(0);
    QCOMPARE(array.at(0).toDouble(), 999.);
    QCOMPARE(array.at(1).toString(), QString("test"));
    QCOMPARE(array.at(2).toBool(), true);
    QCOMPARE(array.size(), 3);

    // if we put a JsonValue into the JsonArray and retrieve
    // it, it should be identical.
    QJsonValue value(QLatin1String("foo"));
    array.append(value);
    QCOMPARE(array.at(3), value);

    int size = array.size();
    array.removeAt(2);
    --size;
    QCOMPARE(array.size(), size);

    QJsonValue taken = array.takeAt(0);
    --size;
    QCOMPARE(taken.toDouble(), 999.);
    QCOMPARE(array.size(), size);

    // check whether null values work
    array.append(QJsonValue());
    ++size;
    QCOMPARE(array.size(), size);
    QCOMPARE(array.last().type(), QJsonValue::Null);
    QCOMPARE(array.last(), QJsonValue());

    QCOMPARE(array.first().type(), QJsonValue::String);
    QCOMPARE(array.first(), QJsonValue(QLatin1String("test")));

    array.prepend(false);
    QCOMPARE(array.first().type(), QJsonValue::Bool);
    QCOMPARE(array.first(), QJsonValue(false));

    QCOMPARE(array.at(-1), QJsonValue(QJsonValue::Undefined));
    QCOMPARE(array.at(array.size()), QJsonValue(QJsonValue::Undefined));

    array.replace(0, -555.);
    QCOMPARE(array.first().type(), QJsonValue::Double);
    QCOMPARE(array.first(), QJsonValue(-555.));
    QCOMPARE(array.at(1).type(), QJsonValue::String);
    QCOMPARE(array.at(1), QJsonValue(QLatin1String("test")));
}

void tst_QtJson::testValueObject()
{
    QJsonObject object;
    object.insert("number", 999.);
    object.insert("string", QLatin1String("test"));
    object.insert("boolean", true);

    QJsonValue value(object);

    // if we don't modify the original JsonObject, toObject()
    // on the JsonValue should return the same object (non-detached).
    QCOMPARE(value.toObject(), object);

    // if we modify the original object, it should detach
    object.insert("test", QJsonValue(QLatin1String("test")));
    QVERIFY2(value.toObject() != object, "object should have detached");
}

void tst_QtJson::testValueArray()
{
    QJsonArray array;
    array.append(999.);
    array.append(QLatin1String("test"));
    array.append(true);

    QJsonValue value(array);

    // if we don't modify the original JsonArray, toArray()
    // on the JsonValue should return the same object (non-detached).
    QCOMPARE(value.toArray(), array);

    // if we modify the original array, it should detach
    array.append(QLatin1String("test"));
    QVERIFY2(value.toArray() != array, "array should have detached");
}

void tst_QtJson::testObjectNested()
{
    QJsonObject inner, outer;
    inner.insert("number", 999.);
    outer.insert("nested", inner);

    // if we don't modify the original JsonObject, value()
    // should return the same object (non-detached).
    QJsonObject value = outer.value("nested").toObject();
    QCOMPARE(value, inner);
    QCOMPARE(value.value("number").toDouble(), 999.);

    // if we modify the original object, it should detach and not
    // affect the nested object
    inner.insert("number", 555.);
    value = outer.value("nested").toObject();
    QVERIFY2(inner.value("number").toDouble() != value.value("number").toDouble(),
             "object should have detached");

    // array in object
    QJsonArray array;
    array.append(123.);
    array.append(456.);
    outer.insert("array", array);
    QCOMPARE(outer.value("array").toArray(), array);
    QCOMPARE(outer.value("array").toArray().at(1).toDouble(), 456.);

    // two deep objects
    QJsonObject twoDeep;
    twoDeep.insert("boolean", true);
    inner.insert("nested", twoDeep);
    outer.insert("nested", inner);
    QCOMPARE(outer.value("nested").toObject().value("nested").toObject(), twoDeep);
    QCOMPARE(outer.value("nested").toObject().value("nested").toObject().value("boolean").toBool(),
             true);
}

void tst_QtJson::testArrayNested()
{
    QJsonArray inner, outer;
    inner.append(999.);
    outer.append(inner);

    // if we don't modify the original JsonArray, value()
    // should return the same array (non-detached).
    QJsonArray value = outer.at(0).toArray();
    QCOMPARE(value, inner);
    QCOMPARE(value.at(0).toDouble(), 999.);

    // if we modify the original array, it should detach and not
    // affect the nested array
    inner.append(555.);
    value = outer.at(0).toArray();
    QVERIFY2(inner.size() != value.size(), "array should have detached");

    // objects in arrays
    QJsonObject object;
    object.insert("boolean", true);
    outer.append(object);
    QCOMPARE(outer.last().toObject(), object);
    QCOMPARE(outer.last().toObject().value("boolean").toBool(), true);

    // two deep arrays
    QJsonArray twoDeep;
    twoDeep.append(QJsonValue(QString::fromLatin1("nested")));
    inner.append(twoDeep);
    outer.append(inner);
    QCOMPARE(outer.last().toArray().last().toArray(), twoDeep);
    QCOMPARE(outer.last().toArray().last().toArray().at(0).toString(), QString("nested"));
}

void tst_QtJson::testArrayNestedEmpty()
{
    QJsonObject object;
    QJsonArray inner;
    object.insert("inner", inner);
    QJsonValue val = object.value("inner");
    QJsonArray value = object.value("inner").toArray();
    QCOMPARE(value.size(), 0);
    QCOMPARE(value, inner);
    QCOMPARE(value.size(), 0);
    object.insert("count", 0.);
    QCOMPARE(object.value("inner").toArray().size(), 0);
    QVERIFY(object.value("inner").toArray().isEmpty());
    QJsonDocument(object).toBinaryData();
    QCOMPARE(object.value("inner").toArray().size(), 0);
}

void tst_QtJson::testObjectNestedEmpty()
{
    QJsonObject object;
    QJsonObject inner;
    QJsonObject inner2;
    object.insert("inner", inner);
    object.insert("inner2", inner2);
    QJsonObject value = object.value("inner").toObject();
    QCOMPARE(value.size(), 0);
    QCOMPARE(value, inner);
    QCOMPARE(value.size(), 0);
    object.insert("count", 0.);
    QCOMPARE(object.value("inner").toObject().size(), 0);
    QCOMPARE(object.value("inner").type(), QJsonValue::Object);
    QJsonDocument(object).toBinaryData();
    QVERIFY(object.value("inner").toObject().isEmpty());
    QVERIFY(object.value("inner2").toObject().isEmpty());
    QJsonDocument doc = QJsonDocument::fromBinaryData(QJsonDocument(object).toBinaryData());
    QVERIFY(!doc.isNull());
    QJsonObject reconstituted(doc.object());
    QCOMPARE(reconstituted.value("inner").toObject().size(), 0);
    QCOMPARE(reconstituted.value("inner").type(), QJsonValue::Object);
    QCOMPARE(reconstituted.value("inner2").type(), QJsonValue::Object);
}

void tst_QtJson::testArrayComfortOperators()
{
    QJsonArray first;
    first.append(123.);
    first.append(QLatin1String("foo"));

    QJsonArray second = QJsonArray() << 123. << QLatin1String("foo");
    QCOMPARE(first, second);

    first = first + QLatin1String("bar");
    second += QLatin1String("bar");
    QCOMPARE(first, second);
}

void tst_QtJson::testValueRef()
{
    QJsonArray array;
    array.append(1.);
    array.append(2.);
    array.append(3.);
    array.append(4);
    array.append(4.1);
    array[1] = false;

    QCOMPARE(array.size(), 5);
    QCOMPARE(array.at(0).toDouble(), 1.);
    QCOMPARE(array.at(2).toDouble(), 3.);
#if QT_VERSION >= 0x050200
    QCOMPARE(array.at(3).toInt(), 4);
    QCOMPARE(array.at(4).toInt(), 0);
#endif
    QCOMPARE(array.at(1).type(), QJsonValue::Bool);
    QCOMPARE(array.at(1).toBool(), false);

    QJsonObject object;
    object[QLatin1String("key")] = true;
    QCOMPARE(object.size(), 1);
    object.insert(QLatin1String("null"), QJsonValue());
    QCOMPARE(object.value(QLatin1String("null")), QJsonValue());
    object[QLatin1String("null")] = 100.;
    QCOMPARE(object.value(QLatin1String("null")).type(), QJsonValue::Double);
    QJsonValue val = object[QLatin1String("null")];
    QCOMPARE(val.toDouble(), 100.);
    QCOMPARE(object.size(), 2);

    array[1] = array[2] = object[QLatin1String("key")] = 42;
    QCOMPARE(array[1], array[2]);
    QCOMPARE(array[2], object[QLatin1String("key")]);
    QCOMPARE(object.value(QLatin1String("key")), QJsonValue(42));
}

void tst_QtJson::testObjectIteration()
{
    QJsonObject object;

    for (QJsonObject::iterator it = object.begin(); it != object.end(); ++it)
        QVERIFY(false);

    const QString property = "kkk";
    object.insert(property, 11);
    object.take(property);
    for (QJsonObject::iterator it = object.begin(); it != object.end(); ++it)
        QVERIFY(false);

    for (int i = 0; i < 10; ++i)
        object[QString::number(i)] = (double)i;

    QCOMPARE(object.size(), 10);

    QCOMPARE(object.begin()->toDouble(), object.constBegin()->toDouble());

    for (QJsonObject::iterator it = object.begin(); it != object.end(); ++it) {
        QJsonValue value = it.value();
        QCOMPARE((double)it.key().toInt(), value.toDouble());
    }

    {
        QJsonObject object2 = object;
        QVERIFY(object == object2);

        QJsonValue val = *object2.begin();
        object2.erase(object2.begin());
        QCOMPARE(object.size(), 10);
        QCOMPARE(object2.size(), 9);

        for (QJsonObject::const_iterator it = object2.constBegin(); it != object2.constEnd(); ++it) {
            QJsonValue value = it.value();
            QVERIFY(it.value() != val);
            QCOMPARE((double)it.key().toInt(), value.toDouble());
        }
    }

    {
        QJsonObject object2 = object;
        QVERIFY(object == object2);

        QJsonObject::iterator it = object2.find(QString::number(5));
        object2.erase(it);
        QCOMPARE(object.size(), 10);
        QCOMPARE(object2.size(), 9);
    }

    {
        QJsonObject::Iterator it = object.begin();
        it += 5;
        QCOMPARE(QJsonValue(it.value()).toDouble(), 5.);
        it -= 3;
        QCOMPARE(QJsonValue(it.value()).toDouble(), 2.);
        QJsonObject::Iterator it2 = it + 5;
        QCOMPARE(QJsonValue(it2.value()).toDouble(), 7.);
        it2 = it - 1;
        QCOMPARE(QJsonValue(it2.value()).toDouble(), 1.);
    }

    {
        QJsonObject::ConstIterator it = object.constBegin();
        it += 5;
        QCOMPARE(QJsonValue(it.value()).toDouble(), 5.);
        it -= 3;
        QCOMPARE(QJsonValue(it.value()).toDouble(), 2.);
        QJsonObject::ConstIterator it2 = it + 5;
        QCOMPARE(QJsonValue(it2.value()).toDouble(), 7.);
        it2 = it - 1;
        QCOMPARE(QJsonValue(it2.value()).toDouble(), 1.);
    }

    QJsonObject::Iterator it = object.begin();
    while (!object.isEmpty())
        it = object.erase(it);
    QCOMPARE(object.size() , 0);
    QVERIFY(it == object.end());
}

void tst_QtJson::testArrayIteration()
{
    QJsonArray array;
    for (int i = 0; i < 10; ++i)
        array.append(i);

    QCOMPARE(array.size(), 10);

    int i = 0;
    for (QJsonArray::iterator it = array.begin(); it != array.end(); ++it, ++i) {
        QJsonValue value = (*it);
        QCOMPARE((double)i, value.toDouble());
    }

    QCOMPARE(array.begin()->toDouble(), array.constBegin()->toDouble());

    {
        QJsonArray array2 = array;
        QVERIFY(array == array2);

        QJsonValue val = *array2.begin();
        array2.erase(array2.begin());
        QCOMPARE(array.size(), 10);
        QCOMPARE(array2.size(), 9);

        i = 1;
        for (QJsonArray::const_iterator it = array2.constBegin(); it != array2.constEnd(); ++it, ++i) {
            QJsonValue value = (*it);
            QCOMPARE((double)i, value.toDouble());
        }
    }

    {
        QJsonArray::Iterator it = array.begin();
        it += 5;
        QCOMPARE(QJsonValue((*it)).toDouble(), 5.);
        it -= 3;
        QCOMPARE(QJsonValue((*it)).toDouble(), 2.);
        QJsonArray::Iterator it2 = it + 5;
        QCOMPARE(QJsonValue(*it2).toDouble(), 7.);
        it2 = it - 1;
        QCOMPARE(QJsonValue(*it2).toDouble(), 1.);
    }

    {
        QJsonArray::ConstIterator it = array.constBegin();
        it += 5;
        QCOMPARE(QJsonValue((*it)).toDouble(), 5.);
        it -= 3;
        QCOMPARE(QJsonValue((*it)).toDouble(), 2.);
        QJsonArray::ConstIterator it2 = it + 5;
        QCOMPARE(QJsonValue(*it2).toDouble(), 7.);
        it2 = it - 1;
        QCOMPARE(QJsonValue(*it2).toDouble(), 1.);
    }

    QJsonArray::Iterator it = array.begin();
    while (!array.isEmpty())
        it = array.erase(it);
    QCOMPARE(array.size() , 0);
    QVERIFY(it == array.end());
}

void tst_QtJson::testObjectFind()
{
    QJsonObject object;
    for (int i = 0; i < 10; ++i)
        object[QString::number(i)] = i;

    QCOMPARE(object.size(), 10);

    QJsonObject::iterator it = object.find(QLatin1String("1"));
    QCOMPARE((*it).toDouble(), 1.);
    it = object.find(QLatin1String("11"));
    QVERIFY((*it).type() == QJsonValue::Undefined);
    QVERIFY(it == object.end());

    QJsonObject::const_iterator cit = object.constFind(QLatin1String("1"));
    QCOMPARE((*cit).toDouble(), 1.);
    cit = object.constFind(QLatin1String("11"));
    QVERIFY((*it).type() == QJsonValue::Undefined);
    QVERIFY(it == object.end());
}

void tst_QtJson::testDocument()
{
    QJsonDocument doc;
    QCOMPARE(doc.isEmpty(), true);
    QCOMPARE(doc.isArray(), false);
    QCOMPARE(doc.isObject(), false);

    QJsonObject object;
    doc.setObject(object);
    QCOMPARE(doc.isEmpty(), false);
    QCOMPARE(doc.isArray(), false);
    QCOMPARE(doc.isObject(), true);

    object.insert(QLatin1String("Key"), QLatin1String("Value"));
    doc.setObject(object);
    QCOMPARE(doc.isEmpty(), false);
    QCOMPARE(doc.isArray(), false);
    QCOMPARE(doc.isObject(), true);
    QVERIFY(doc.object() == object);
    QVERIFY(doc.array() == QJsonArray());

    doc = QJsonDocument();
    QCOMPARE(doc.isEmpty(), true);
    QCOMPARE(doc.isArray(), false);
    QCOMPARE(doc.isObject(), false);

    QJsonArray array;
    doc.setArray(array);
    QCOMPARE(doc.isEmpty(), false);
    QCOMPARE(doc.isArray(), true);
    QCOMPARE(doc.isObject(), false);

    array.append(QLatin1String("Value"));
    doc.setArray(array);
    QCOMPARE(doc.isEmpty(), false);
    QCOMPARE(doc.isArray(), true);
    QCOMPARE(doc.isObject(), false);
    QVERIFY(doc.array() == array);
    QVERIFY(doc.object() == QJsonObject());

    QJsonObject outer;
    outer.insert(QLatin1String("outerKey"), 22);
    QJsonObject inner;
    inner.insert(QLatin1String("innerKey"), 42);
    outer.insert(QLatin1String("innter"), inner);
    QJsonArray innerArray;
    innerArray.append(23);
    outer.insert(QLatin1String("innterArray"), innerArray);

    QJsonDocument doc2(outer.value(QLatin1String("innter")).toObject());
    QVERIFY(doc2.object().contains(QLatin1String("innerKey")));
    QCOMPARE(doc2.object().value(QLatin1String("innerKey")), QJsonValue(42));

    QJsonDocument doc3;
    doc3.setObject(outer.value(QLatin1String("innter")).toObject());
    QCOMPARE(doc3.isArray(), false);
    QCOMPARE(doc3.isObject(), true);
    QVERIFY(doc3.object().contains(QLatin1String("innerKey")));
    QCOMPARE(doc3.object().value(QLatin1String("innerKey")), QJsonValue(42));

    QJsonDocument doc4(outer.value(QLatin1String("innterArray")).toArray());
    QCOMPARE(doc4.isArray(), true);
    QCOMPARE(doc4.isObject(), false);
    QCOMPARE(doc4.array().size(), 1);
    QCOMPARE(doc4.array().at(0), QJsonValue(23));

    QJsonDocument doc5;
    doc5.setArray(outer.value(QLatin1String("innterArray")).toArray());
    QCOMPARE(doc5.isArray(), true);
    QCOMPARE(doc5.isObject(), false);
    QCOMPARE(doc5.array().size(), 1);
    QCOMPARE(doc5.array().at(0), QJsonValue(23));
}

void tst_QtJson::nullValues()
{
    QJsonArray array;
    array.append(QJsonValue());

    QCOMPARE(array.size(), 1);
    QCOMPARE(array.at(0), QJsonValue());

    QJsonObject object;
    object.insert(QString("key"), QJsonValue());
    QCOMPARE(object.contains("key"), true);
    QCOMPARE(object.size(), 1);
    QCOMPARE(object.value("key"), QJsonValue());
}

void tst_QtJson::nullArrays()
{
    QJsonArray nullArray;
    QJsonArray nonNull;
    nonNull.append(QLatin1String("bar"));

    QCOMPARE(nullArray, QJsonArray());
    QVERIFY(nullArray != nonNull);
    QVERIFY(nonNull != nullArray);

    QCOMPARE(nullArray.size(), 0);
    QCOMPARE(nullArray.takeAt(0), QJsonValue(QJsonValue::Undefined));
    QCOMPARE(nullArray.first(), QJsonValue(QJsonValue::Undefined));
    QCOMPARE(nullArray.last(), QJsonValue(QJsonValue::Undefined));
    nullArray.removeAt(0);
    nullArray.removeAt(-1);

    nullArray.append(QString("bar"));
    nullArray.removeAt(0);

    QCOMPARE(nullArray.size(), 0);
    QCOMPARE(nullArray.takeAt(0), QJsonValue(QJsonValue::Undefined));
    QCOMPARE(nullArray.first(), QJsonValue(QJsonValue::Undefined));
    QCOMPARE(nullArray.last(), QJsonValue(QJsonValue::Undefined));
    nullArray.removeAt(0);
    nullArray.removeAt(-1);
}

void tst_QtJson::nullObject()
{
    QJsonObject nullObject;
    QJsonObject nonNull;
    nonNull.insert(QLatin1String("foo"), QLatin1String("bar"));

    QCOMPARE(nullObject, QJsonObject());
    QVERIFY(nullObject != nonNull);
    QVERIFY(nonNull != nullObject);

    QCOMPARE(nullObject.size(), 0);
    QCOMPARE(nullObject.keys(), QStringList());
    nullObject.remove("foo");
    QCOMPARE(nullObject, QJsonObject());
    QCOMPARE(nullObject.take("foo"), QJsonValue(QJsonValue::Undefined));
    QCOMPARE(nullObject.contains("foo"), false);

    nullObject.insert("foo", QString("bar"));
    nullObject.remove("foo");

    QCOMPARE(nullObject.size(), 0);
    QCOMPARE(nullObject.keys(), QStringList());
    nullObject.remove("foo");
    QCOMPARE(nullObject, QJsonObject());
    QCOMPARE(nullObject.take("foo"), QJsonValue(QJsonValue::Undefined));
    QCOMPARE(nullObject.contains("foo"), false);
}

void tst_QtJson::constNullObject()
{
    const QJsonObject nullObject;
    QJsonObject nonNull;
    nonNull.insert(QLatin1String("foo"), QLatin1String("bar"));

    QCOMPARE(nullObject, QJsonObject());
    QVERIFY(nullObject != nonNull);
    QVERIFY(nonNull != nullObject);

    QCOMPARE(nullObject.size(), 0);
    QCOMPARE(nullObject.keys(), QStringList());
    QCOMPARE(nullObject, QJsonObject());
    QCOMPARE(nullObject.contains("foo"), false);
    QCOMPARE(nullObject["foo"], QJsonValue(QJsonValue::Undefined));
}

void tst_QtJson::keySorting()
{
    const char *json = "{ \"B\": true, \"A\": false }";
    QJsonDocument doc = QJsonDocument::fromJson(json);

    QCOMPARE(doc.isObject(), true);

    QJsonObject o = doc.object();
    QCOMPARE(o.size(), 2);
    QJsonObject::const_iterator it = o.constBegin();
    QCOMPARE(it.key(), QLatin1String("A"));
    ++it;
    QCOMPARE(it.key(), QLatin1String("B"));
}

void tst_QtJson::undefinedValues()
{
    QJsonObject object;
    object.insert("Key", QJsonValue(QJsonValue::Undefined));
    QCOMPARE(object.size(), 0);

    object.insert("Key", QLatin1String("Value"));
    QCOMPARE(object.size(), 1);
    QCOMPARE(object.value("Key").type(), QJsonValue::String);
    QCOMPARE(object.value("foo").type(), QJsonValue::Undefined);
    object.insert("Key", QJsonValue(QJsonValue::Undefined));
    QCOMPARE(object.size(), 0);
    QCOMPARE(object.value("Key").type(), QJsonValue::Undefined);

    QJsonArray array;
    array.append(QJsonValue(QJsonValue::Undefined));
    QCOMPARE(array.size(), 1);
    QCOMPARE(array.at(0).type(), QJsonValue::Null);

    QCOMPARE(array.at(1).type(), QJsonValue::Undefined);
    QCOMPARE(array.at(-1).type(), QJsonValue::Undefined);
}

void tst_QtJson::fromVariant()
{
    bool boolValue = true;
    int intValue = -1;
    uint uintValue = 1;
    long long longlongValue = -2;
    unsigned long long ulonglongValue = 2;
    float floatValue = 3.3f;
    double doubleValue = 4.4;
    QString stringValue("str");

    QStringList stringList;
    stringList.append(stringValue);
    stringList.append("str2");
    QJsonArray jsonArray_string;
    jsonArray_string.append(stringValue);
    jsonArray_string.append("str2");

    QVariantList variantList;
    variantList.append(boolValue);
    variantList.append(floatValue);
    variantList.append(doubleValue);
    variantList.append(stringValue);
    variantList.append(stringList);
    variantList.append(QVariant());
    QJsonArray jsonArray_variant;
    jsonArray_variant.append(boolValue);
    jsonArray_variant.append(floatValue);
    jsonArray_variant.append(doubleValue);
    jsonArray_variant.append(stringValue);
    jsonArray_variant.append(jsonArray_string);
    jsonArray_variant.append(QJsonValue());

    QVariantMap variantMap;
    variantMap["bool"] = boolValue;
    variantMap["float"] = floatValue;
    variantMap["string"] = stringValue;
    variantMap["array"] = variantList;
    QJsonObject jsonObject;
    jsonObject["bool"] = boolValue;
    jsonObject["float"] = floatValue;
    jsonObject["string"] = stringValue;
    jsonObject["array"] = jsonArray_variant;

    QCOMPARE(QJsonValue::fromVariant(QVariant(boolValue)), QJsonValue(boolValue));
    QCOMPARE(QJsonValue::fromVariant(QVariant(intValue)), QJsonValue(intValue));
    QCOMPARE(QJsonValue::fromVariant(QVariant(uintValue)), QJsonValue(static_cast<double>(uintValue)));
    QCOMPARE(QJsonValue::fromVariant(QVariant(longlongValue)), QJsonValue(longlongValue));
    QCOMPARE(QJsonValue::fromVariant(QVariant(ulonglongValue)), QJsonValue(static_cast<double>(ulonglongValue)));
    QCOMPARE(QJsonValue::fromVariant(QVariant(floatValue)), QJsonValue(static_cast<double>(floatValue)));
    QCOMPARE(QJsonValue::fromVariant(QVariant(doubleValue)), QJsonValue(doubleValue));
    QCOMPARE(QJsonValue::fromVariant(QVariant(stringValue)), QJsonValue(stringValue));
    QCOMPARE(QJsonValue::fromVariant(QVariant(stringList)), QJsonValue(jsonArray_string));
    QCOMPARE(QJsonValue::fromVariant(QVariant(variantList)), QJsonValue(jsonArray_variant));
    QCOMPARE(QJsonValue::fromVariant(QVariant(variantMap)), QJsonValue(jsonObject));
}

void tst_QtJson::fromVariantMap()
{
    QVariantMap map;
    map.insert(QLatin1String("key1"), QLatin1String("value1"));
    map.insert(QLatin1String("key2"), QLatin1String("value2"));
    QJsonObject object = QJsonObject::fromVariantMap(map);
    QCOMPARE(object.size(), 2);
    QCOMPARE(object.value(QLatin1String("key1")), QJsonValue(QLatin1String("value1")));
    QCOMPARE(object.value(QLatin1String("key2")), QJsonValue(QLatin1String("value2")));

    QVariantList list;
    list.append(true);
    list.append(QVariant());
    list.append(999.);
    list.append(QLatin1String("foo"));
    map.insert("list", list);
    object = QJsonObject::fromVariantMap(map);
    QCOMPARE(object.size(), 3);
    QCOMPARE(object.value(QLatin1String("key1")), QJsonValue(QLatin1String("value1")));
    QCOMPARE(object.value(QLatin1String("key2")), QJsonValue(QLatin1String("value2")));
    QCOMPARE(object.value(QLatin1String("list")).type(), QJsonValue::Array);
    QJsonArray array = object.value(QLatin1String("list")).toArray();
    QCOMPARE(array.size(), 4);
    QCOMPARE(array.at(0).type(), QJsonValue::Bool);
    QCOMPARE(array.at(0).toBool(), true);
    QCOMPARE(array.at(1).type(), QJsonValue::Null);
    QCOMPARE(array.at(2).type(), QJsonValue::Double);
    QCOMPARE(array.at(2).toDouble(), 999.);
    QCOMPARE(array.at(3).type(), QJsonValue::String);
    QCOMPARE(array.at(3).toString(), QLatin1String("foo"));
}

void tst_QtJson::toVariantMap()
{
    QCOMPARE(QMetaType::Type(QJsonValue(QJsonObject()).toVariant().type()), QMetaType::QVariantMap); // QTBUG-32524

    QJsonObject object;
    QVariantMap map = object.toVariantMap();
    QVERIFY(map.isEmpty());

    object.insert("Key", QString("Value"));
    object.insert("null", QJsonValue());
    QJsonArray array;
    array.append(true);
    array.append(999.);
    array.append(QLatin1String("string"));
    array.append(QJsonValue());
    object.insert("Array", array);

    map = object.toVariantMap();

    QCOMPARE(map.size(), 3);
    QCOMPARE(map.value("Key"), QVariant(QString("Value")));
    QCOMPARE(map.value("null"), QVariant());
    QCOMPARE(map.value("Array").type(), QVariant::List);
    QVariantList list = map.value("Array").toList();
    QCOMPARE(list.size(), 4);
    QCOMPARE(list.at(0), QVariant(true));
    QCOMPARE(list.at(1), QVariant(999.));
    QCOMPARE(list.at(2), QVariant(QLatin1String("string")));
    QCOMPARE(list.at(3), QVariant());
}

void tst_QtJson::toVariantList()
{
    QCOMPARE(QMetaType::Type(QJsonValue(QJsonArray()).toVariant().type()), QMetaType::QVariantList); // QTBUG-32524

    QJsonArray array;
    QVariantList list = array.toVariantList();
    QVERIFY(list.isEmpty());

    array.append(QString("Value"));
    array.append(QJsonValue());
    QJsonArray inner;
    inner.append(true);
    inner.append(999.);
    inner.append(QLatin1String("string"));
    inner.append(QJsonValue());
    array.append(inner);

    list = array.toVariantList();

    QCOMPARE(list.size(), 3);
    QCOMPARE(list[0], QVariant(QString("Value")));
    QCOMPARE(list[1], QVariant());
    QCOMPARE(list[2].type(), QVariant::List);
    QVariantList vlist = list[2].toList();
    QCOMPARE(vlist.size(), 4);
    QCOMPARE(vlist.at(0), QVariant(true));
    QCOMPARE(vlist.at(1), QVariant(999.));
    QCOMPARE(vlist.at(2), QVariant(QLatin1String("string")));
    QCOMPARE(vlist.at(3), QVariant());
}

void tst_QtJson::toJson()
{
    // Test QJsonDocument::Indented format
    {
        QJsonObject object;
        object.insert("\\Key\n", QString("Value"));
        object.insert("null", QJsonValue());
        QJsonArray array;
        array.append(true);
        array.append(999.);
        array.append(QLatin1String("string"));
        array.append(QJsonValue());
        array.append(QLatin1String("\\\a\n\r\b\tabcABC\""));
        object.insert("Array", array);

        QByteArray json = QJsonDocument(object).toJson();

        QByteArray expected =
                "{\n"
                "    \"Array\": [\n"
                "        true,\n"
                "        999,\n"
                "        \"string\",\n"
                "        null,\n"
                "        \"\\\\\\u0007\\n\\r\\b\\tabcABC\\\"\"\n"
                "    ],\n"
                "    \"\\\\Key\\n\": \"Value\",\n"
                "    \"null\": null\n"
                "}\n";
        QCOMPARE(json, expected);

        QJsonDocument doc;
        doc.setObject(object);
        json = doc.toJson();
        QCOMPARE(json, expected);

        doc.setArray(array);
        json = doc.toJson();
        expected =
                "[\n"
                "    true,\n"
                "    999,\n"
                "    \"string\",\n"
                "    null,\n"
                "    \"\\\\\\u0007\\n\\r\\b\\tabcABC\\\"\"\n"
                "]\n";
        QCOMPARE(json, expected);
    }

    // Test QJsonDocument::Compact format
    {
        QJsonObject object;
        object.insert("\\Key\n", QString("Value"));
        object.insert("null", QJsonValue());
        QJsonArray array;
        array.append(true);
        array.append(999.);
        array.append(QLatin1String("string"));
        array.append(QJsonValue());
        array.append(QLatin1String("\\\a\n\r\b\tabcABC\""));
        object.insert("Array", array);

        QByteArray json = QJsonDocument(object).toJson(QJsonDocument::Compact);
        QByteArray expected =
                "{\"Array\":[true,999,\"string\",null,\"\\\\\\u0007\\n\\r\\b\\tabcABC\\\"\"],\"\\\\Key\\n\":\"Value\",\"null\":null}";
        QCOMPARE(json, expected);

        QJsonDocument doc;
        doc.setObject(object);
        json = doc.toJson(QJsonDocument::Compact);
        QCOMPARE(json, expected);

        doc.setArray(array);
        json = doc.toJson(QJsonDocument::Compact);
        expected = "[true,999,\"string\",null,\"\\\\\\u0007\\n\\r\\b\\tabcABC\\\"\"]";
        QCOMPARE(json, expected);
    }

}

void tst_QtJson::toJsonSillyNumericValues()
{
    QJsonObject object;
    QJsonArray array;
    array.append(QJsonValue(std::numeric_limits<double>::infinity()));  // encode to: null
    array.append(QJsonValue(-std::numeric_limits<double>::infinity())); // encode to: null
    array.append(QJsonValue(std::numeric_limits<double>::quiet_NaN())); // encode to: null
    object.insert("Array", array);

    QByteArray json = QJsonDocument(object).toJson();

    QByteArray expected =
            "{\n"
            "    \"Array\": [\n"
            "        null,\n"
            "        null,\n"
            "        null\n"
            "    ]\n"
            "}\n";

    QCOMPARE(json, expected);

    QJsonDocument doc;
    doc.setObject(object);
    json = doc.toJson();
    QCOMPARE(json, expected);
}

void tst_QtJson::toJsonLargeNumericValues()
{
    QJsonObject object;
    QJsonArray array;
    array.append(QJsonValue(1.234567)); // actual precision bug in Qt 5.0.0
    array.append(QJsonValue(1.7976931348623157e+308)); // JS Number.MAX_VALUE
    array.append(QJsonValue(5e-324));                  // JS Number.MIN_VALUE
    array.append(QJsonValue(std::numeric_limits<double>::min()));
    array.append(QJsonValue(std::numeric_limits<double>::max()));
    array.append(QJsonValue(std::numeric_limits<double>::epsilon()));
    array.append(QJsonValue(std::numeric_limits<double>::denorm_min()));
    array.append(QJsonValue(0.0));
    array.append(QJsonValue(-std::numeric_limits<double>::min()));
    array.append(QJsonValue(-std::numeric_limits<double>::max()));
    array.append(QJsonValue(-std::numeric_limits<double>::epsilon()));
    array.append(QJsonValue(-std::numeric_limits<double>::denorm_min()));
    array.append(QJsonValue(-0.0));
    array.append(QJsonValue(9007199254740992LL));  // JS Number max integer
    array.append(QJsonValue(-9007199254740992LL)); // JS Number min integer
    object.insert("Array", array);

    QByteArray json = QJsonDocument(object).toJson();

    QByteArray expected =
            "{\n"
            "    \"Array\": [\n"
            "        1.234567,\n"
            "        1.7976931348623157e+308,\n"
            //     ((4.9406564584124654e-324 == 5e-324) == true)
            // I can only think JavaScript has a special formatter to
            //  emit this value for this IEEE754 bit pattern.
            "        4.9406564584124654e-324,\n"
            "        2.2250738585072014e-308,\n"
            "        1.7976931348623157e+308,\n"
            "        2.2204460492503131e-16,\n"
            "        4.9406564584124654e-324,\n"
            "        0,\n"
            "        -2.2250738585072014e-308,\n"
            "        -1.7976931348623157e+308,\n"
            "        -2.2204460492503131e-16,\n"
            "        -4.9406564584124654e-324,\n"
            "        0,\n"
            "        9007199254740992,\n"
            "        -9007199254740992\n"
            "    ]\n"
            "}\n";

#ifdef Q_OS_QNX
    QEXPECT_FAIL("", "See QTBUG-37066", Continue);
#endif
    QCOMPARE(json, expected);

    QJsonDocument doc;
    doc.setObject(object);
    json = doc.toJson();
#ifdef Q_OS_QNX
    QEXPECT_FAIL("", "See QTBUG-37066", Continue);
#endif
    QCOMPARE(json, expected);
}

void tst_QtJson::fromJson()
{
    {
        QByteArray json = "[\n    true\n]\n";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), true);
        QCOMPARE(doc.isObject(), false);
        QJsonArray array = doc.array();
        QCOMPARE(array.size(), 1);
        QCOMPARE(array.at(0).type(), QJsonValue::Bool);
        QCOMPARE(array.at(0).toBool(), true);
        QCOMPARE(doc.toJson(), json);
    }
    /*{
        //regression test: test if unicode_control_characters are correctly decoded
        QByteArray json = "[\n    \"" UNICODE_NON_CHARACTER "\"\n]\n";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), true);
        QCOMPARE(doc.isObject(), false);
        QJsonArray array = doc.array();
        QCOMPARE(array.size(), 1);
        QCOMPARE(array.at(0).type(), QJsonValue::String);
        QCOMPARE(array.at(0).toString(), QString::fromUtf8(UNICODE_NON_CHARACTER));
        QCOMPARE(doc.toJson(), json);
    }*/
    {
        QByteArray json = "[]";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), true);
        QCOMPARE(doc.isObject(), false);
        QJsonArray array = doc.array();
        QCOMPARE(array.size(), 0);
    }
    {
        QByteArray json = "{}";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), false);
        QCOMPARE(doc.isObject(), true);
        QJsonObject object = doc.object();
        QCOMPARE(object.size(), 0);
    }
    {
        QByteArray json = "{\n    \"Key\": true\n}\n";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), false);
        QCOMPARE(doc.isObject(), true);
        QJsonObject object = doc.object();
        QCOMPARE(object.size(), 1);
        QCOMPARE(object.value("Key"), QJsonValue(true));
        QCOMPARE(doc.toJson(), json);
    }
    {
        QByteArray json = "[ null, true, false, \"Foo\", 1, [], {} ]";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), true);
        QCOMPARE(doc.isObject(), false);
        QJsonArray array = doc.array();
        QCOMPARE(array.size(), 7);
        QCOMPARE(array.at(0).type(), QJsonValue::Null);
        QCOMPARE(array.at(1).type(), QJsonValue::Bool);
        QCOMPARE(array.at(1).toBool(), true);
        QCOMPARE(array.at(2).type(), QJsonValue::Bool);
        QCOMPARE(array.at(2).toBool(), false);
        QCOMPARE(array.at(3).type(), QJsonValue::String);
        QCOMPARE(array.at(3).toString(), QLatin1String("Foo"));
        QCOMPARE(array.at(4).type(), QJsonValue::Double);
        QCOMPARE(array.at(4).toDouble(), 1.);
        QCOMPARE(array.at(5).type(), QJsonValue::Array);
        QCOMPARE(array.at(5).toArray().size(), 0);
        QCOMPARE(array.at(6).type(), QJsonValue::Object);
        QCOMPARE(array.at(6).toObject().size(), 0);
    }
    {
        QByteArray json = "{ \"0\": null, \"1\": true, \"2\": false, \"3\": \"Foo\", \"4\": 1, \"5\": [], \"6\": {} }";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), false);
        QCOMPARE(doc.isObject(), true);
        QJsonObject object = doc.object();
        QCOMPARE(object.size(), 7);
        QCOMPARE(object.value("0").type(), QJsonValue::Null);
        QCOMPARE(object.value("1").type(), QJsonValue::Bool);
        QCOMPARE(object.value("1").toBool(), true);
        QCOMPARE(object.value("2").type(), QJsonValue::Bool);
        QCOMPARE(object.value("2").toBool(), false);
        QCOMPARE(object.value("3").type(), QJsonValue::String);
        QCOMPARE(object.value("3").toString(), QLatin1String("Foo"));
        QCOMPARE(object.value("4").type(), QJsonValue::Double);
        QCOMPARE(object.value("4").toDouble(), 1.);
        QCOMPARE(object.value("5").type(), QJsonValue::Array);
        QCOMPARE(object.value("5").toArray().size(), 0);
        QCOMPARE(object.value("6").type(), QJsonValue::Object);
        QCOMPARE(object.value("6").toObject().size(), 0);
    }
    {
        QByteArray compactJson = "{\"Array\": [true,999,\"string\",null,\"\\\\\\u0007\\n\\r\\b\\tabcABC\\\"\"],\"\\\\Key\\n\": \"Value\",\"null\": null}";
        QJsonDocument doc = QJsonDocument::fromJson(compactJson);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), false);
        QCOMPARE(doc.isObject(), true);
        QJsonObject object = doc.object();
        QCOMPARE(object.size(), 3);
        QCOMPARE(object.value("\\Key\n").isString(), true);
        QCOMPARE(object.value("\\Key\n").toString(), QString("Value"));
        QCOMPARE(object.value("null").isNull(), true);
        QCOMPARE(object.value("Array").isArray(), true);
        QJsonArray array = object.value("Array").toArray();
        QCOMPARE(array.size(), 5);
        QCOMPARE(array.at(0).isBool(), true);
        QCOMPARE(array.at(0).toBool(), true);
        QCOMPARE(array.at(1).isDouble(), true);
        QCOMPARE(array.at(1).toDouble(), 999.);
        QCOMPARE(array.at(2).isString(), true);
        QCOMPARE(array.at(2).toString(), QLatin1String("string"));
        QCOMPARE(array.at(3).isNull(), true);
        QCOMPARE(array.at(4).isString(), true);
        QCOMPARE(array.at(4).toString(), QLatin1String("\\\a\n\r\b\tabcABC\""));
    }
}

void tst_QtJson::fromJsonErrors()
{
    {
        QJsonParseError error;
        QByteArray json = "{\n    \n\n";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::UnterminatedObject);
        QCOMPARE(error.offset, 8);
    }
    {
        QJsonParseError error;
        QByteArray json = "{\n    \"key\" 10\n";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::MissingNameSeparator);
        QCOMPARE(error.offset, 13);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    \n\n";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::UnterminatedArray);
        QCOMPARE(error.offset, 8);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n   1, true\n\n";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::UnterminatedArray);
        QCOMPARE(error.offset, 14);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n  1 true\n\n";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::MissingValueSeparator);
        QCOMPARE(error.offset, 7);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    nul";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalValue);
        QCOMPARE(error.offset, 7);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    nulzz";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalValue);
        QCOMPARE(error.offset, 10);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    tru";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalValue);
        QCOMPARE(error.offset, 7);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    trud]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalValue);
        QCOMPARE(error.offset, 10);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    fal";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalValue);
        QCOMPARE(error.offset, 7);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    falsd]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalValue);
        QCOMPARE(error.offset, 11);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    11111";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::TerminationByNumber);
        QCOMPARE(error.offset, 11);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    -1E10000]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalNumber);
        QCOMPARE(error.offset, 14);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    -1e-10000]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalNumber);
        QCOMPARE(error.offset, 15);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    \"\\u12\"]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalEscapeSequence);
        QCOMPARE(error.offset, 11);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    \"foo" INVALID_UNICODE "bar\"]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalUTF8String);
        QCOMPARE(error.offset, 14);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    \"";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::UnterminatedString);
        QCOMPARE(error.offset, 8);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    \"c" UNICODE_DJE "a\\u12\"]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalEscapeSequence);
        QCOMPARE(error.offset, 15);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    \"c" UNICODE_DJE "a" INVALID_UNICODE "bar\"]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::IllegalUTF8String);
        QCOMPARE(error.offset, 15);
    }
    {
        QJsonParseError error;
        QByteArray json = "[\n    \"c" UNICODE_DJE "a ]";
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        QVERIFY(doc.isEmpty());
        QCOMPARE(error.error, QJsonParseError::UnterminatedString);
        QCOMPARE(error.offset, 14);
    }
}

void tst_QtJson::fromBinary()
{
    QFile file(":/test.json");
    file.open(QFile::ReadOnly);
    QByteArray testJson = file.readAll();

    QJsonDocument doc = QJsonDocument::fromJson(testJson);
    QJsonDocument outdoc = QJsonDocument::fromBinaryData(doc.toBinaryData());
    QVERIFY(!outdoc.isNull());
    QVERIFY(doc == outdoc);

    QFile bfile(":/test.bjson");
    bfile.open(QFile::ReadOnly);
    QByteArray binary = bfile.readAll();

    QJsonDocument bdoc = QJsonDocument::fromBinaryData(binary);
    QVERIFY(!bdoc.isNull());
    QVERIFY(doc.toVariant() == bdoc.toVariant());
    QVERIFY(doc == bdoc);
}

void tst_QtJson::toAndFromBinary_data()
{
    QTest::addColumn<QString>("filename");
    QTest::newRow("test.json") << (":/test.json");
    QTest::newRow("test2.json") << (":/test2.json");
}

void tst_QtJson::toAndFromBinary()
{
    QFETCH(QString, filename);
    QFile file(filename);
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray data = file.readAll();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QVERIFY(!doc.isNull());
    QJsonDocument outdoc = QJsonDocument::fromBinaryData(doc.toBinaryData());
    QVERIFY(!outdoc.isNull());
    QVERIFY(doc == outdoc);
}

void tst_QtJson::parseNumbers()
{
    {
        // test number parsing
        struct Numbers {
            const char *str;
            int n;
        };
        Numbers numbers [] = {
            { "0", 0 },
            { "1", 1 },
            { "10", 10 },
            { "-1", -1 },
            { "100000", 100000 },
            { "-999", -999 }
        };
        int size = sizeof(numbers)/sizeof(Numbers);
        for (int i = 0; i < size; ++i) {
            QByteArray json = "[ ";
            json += numbers[i].str;
            json += " ]";
            QJsonDocument doc = QJsonDocument::fromJson(json);
            QVERIFY(!doc.isEmpty());
            QCOMPARE(doc.isArray(), true);
            QCOMPARE(doc.isObject(), false);
            QJsonArray array = doc.array();
            QCOMPARE(array.size(), 1);
            QJsonValue val = array.at(0);
            QCOMPARE(val.type(), QJsonValue::Double);
            QCOMPARE(val.toDouble(), (double)numbers[i].n);
        }
    }
    {
        // test number parsing
        struct Numbers {
            const char *str;
            double n;
        };
        Numbers numbers [] = {
            { "0", 0 },
            { "1", 1 },
            { "10", 10 },
            { "-1", -1 },
            { "100000", 100000 },
            { "-999", -999 },
            { "1.1", 1.1 },
            { "1e10", 1e10 },
            { "-1.1", -1.1 },
            { "-1e10", -1e10 },
            { "-1E10", -1e10 },
            { "1.1e10", 1.1e10 },
            { "1.1e308", 1.1e308 },
            { "-1.1e308", -1.1e308 },
            { "1.1e-308", 1.1e-308 },
            { "-1.1e-308", -1.1e-308 },
            { "1.1e+308", 1.1e+308 },
            { "-1.1e+308", -1.1e+308 },
            { "1.e+308", 1.e+308 },
            { "-1.e+308", -1.e+308 }
        };
        int size = sizeof(numbers)/sizeof(Numbers);
        for (int i = 0; i < size; ++i) {
            QByteArray json = "[ ";
            json += numbers[i].str;
            json += " ]";
            QJsonDocument doc = QJsonDocument::fromJson(json);
#ifdef Q_OS_QNX
            if (0 == QString::compare(numbers[i].str, "1.1e-308"))
                QEXPECT_FAIL("", "See QTBUG-37066", Abort);
#endif
            QVERIFY(!doc.isEmpty());
            QCOMPARE(doc.isArray(), true);
            QCOMPARE(doc.isObject(), false);
            QJsonArray array = doc.array();
            QCOMPARE(array.size(), 1);
            QJsonValue val = array.at(0);
            QCOMPARE(val.type(), QJsonValue::Double);
            QCOMPARE(val.toDouble(), numbers[i].n);
        }
    }
}

void tst_QtJson::parseStrings()
{
    const char *strings [] =
    {
        "Foo",
        "abc\\\"abc",
        "abc\\\\abc",
        "abc\\babc",
        "abc\\fabc",
        "abc\\nabc",
        "abc\\rabc",
        "abc\\tabc",
        "abc\\u0019abc"
//        "abc" UNICODE_DJE "abc",
//        UNICODE_NON_CHARACTER
    };
    int size = sizeof(strings)/sizeof(const char *);

    for (int i = 0; i < size; ++i) {
        QByteArray json = "[\n    \"";
        json += strings[i];
        json += "\"\n]\n";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), true);
        QCOMPARE(doc.isObject(), false);
        QJsonArray array = doc.array();
        QCOMPARE(array.size(), 1);
        QJsonValue val = array.at(0);
        QCOMPARE(val.type(), QJsonValue::String);

        QCOMPARE(doc.toJson(), json);
    }

    struct Pairs {
        const char *in;
        const char *out;
    };
    Pairs pairs [] = {
        { "abc\\/abc", "abc/abc" },
//        { "abc\\u0402abc", "abc" UNICODE_DJE "abc" },
        { "abc\\u0065abc", "abceabc" }
//        { "abc\\uFFFFabc", "abc" UNICODE_NON_CHARACTER "abc" }
    };
    size = sizeof(pairs)/sizeof(Pairs);

    for (int i = 0; i < size; ++i) {
        QByteArray json = "[\n    \"";
        json += pairs[i].in;
        json += "\"\n]\n";
        QByteArray out = "[\n    \"";
        out += pairs[i].out;
        out += "\"\n]\n";
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QVERIFY(!doc.isEmpty());
        QCOMPARE(doc.isArray(), true);
        QCOMPARE(doc.isObject(), false);
        QJsonArray array = doc.array();
        QCOMPARE(array.size(), 1);
        QJsonValue val = array.at(0);
        QCOMPARE(val.type(), QJsonValue::String);

        QCOMPARE(doc.toJson(), out);
    }

}

void tst_QtJson::parseDuplicateKeys()
{
    const char *json = "{ \"B\": true, \"A\": null, \"B\": false }";

    QJsonDocument doc = QJsonDocument::fromJson(json);
    QCOMPARE(doc.isObject(), true);

    QJsonObject o = doc.object();
    QCOMPARE(o.size(), 2);
    QJsonObject::const_iterator it = o.constBegin();
    QCOMPARE(it.key(), QLatin1String("A"));
    QCOMPARE(it.value(), QJsonValue());
    ++it;
    QCOMPARE(it.key(), QLatin1String("B"));
    QCOMPARE(it.value(), QJsonValue(false));
}

void tst_QtJson::testParser()
{
    QFile file(":/test.json");
    file.open(QFile::ReadOnly);
    QByteArray testJson = file.readAll();

    QJsonDocument doc = QJsonDocument::fromJson(testJson);
    QVERIFY(!doc.isEmpty());
}

void tst_QtJson::compactArray()
{
    QJsonArray array;
    array.append(QLatin1String("First Entry"));
    array.append(QLatin1String("Second Entry"));
    array.append(QLatin1String("Third Entry"));
    QJsonDocument doc(array);
    int s =  doc.toBinaryData().size();
    array.removeAt(1);
    doc.setArray(array);
    QVERIFY(s > doc.toBinaryData().size());
    s = doc.toBinaryData().size();
    QCOMPARE(doc.toJson(),
             QByteArray("[\n"
                        "    \"First Entry\",\n"
                        "    \"Third Entry\"\n"
                        "]\n"));

    array.removeAt(0);
    doc.setArray(array);
    QVERIFY(s > doc.toBinaryData().size());
    s = doc.toBinaryData().size();
    QCOMPARE(doc.toJson(),
             QByteArray("[\n"
                        "    \"Third Entry\"\n"
                        "]\n"));

    array.removeAt(0);
    doc.setArray(array);
    QVERIFY(s > doc.toBinaryData().size());
    s = doc.toBinaryData().size();
    QCOMPARE(doc.toJson(),
             QByteArray("[\n"
                        "]\n"));

}

void tst_QtJson::compactObject()
{
    QJsonObject object;
    object.insert(QLatin1String("Key1"), QLatin1String("First Entry"));
    object.insert(QLatin1String("Key2"), QLatin1String("Second Entry"));
    object.insert(QLatin1String("Key3"), QLatin1String("Third Entry"));
    QJsonDocument doc(object);
    int s =  doc.toBinaryData().size();
    object.remove(QLatin1String("Key2"));
    doc.setObject(object);
    QVERIFY(s > doc.toBinaryData().size());
    s = doc.toBinaryData().size();
    QCOMPARE(doc.toJson(),
             QByteArray("{\n"
                        "    \"Key1\": \"First Entry\",\n"
                        "    \"Key3\": \"Third Entry\"\n"
                        "}\n"));

    object.remove(QLatin1String("Key1"));
    doc.setObject(object);
    QVERIFY(s > doc.toBinaryData().size());
    s = doc.toBinaryData().size();
    QCOMPARE(doc.toJson(),
             QByteArray("{\n"
                        "    \"Key3\": \"Third Entry\"\n"
                        "}\n"));

    object.remove(QLatin1String("Key3"));
    doc.setObject(object);
    QVERIFY(s > doc.toBinaryData().size());
    s = doc.toBinaryData().size();
    QCOMPARE(doc.toJson(),
             QByteArray("{\n"
                        "}\n"));

}

void tst_QtJson::validation()
{
    // this basically tests that we don't crash on corrupt data
    QFile file(":/test.json");
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray testJson = file.readAll();
    QVERIFY(!testJson.isEmpty());

    QJsonDocument doc = QJsonDocument::fromJson(testJson);
    QVERIFY(!doc.isNull());

    QByteArray binary = doc.toBinaryData();

    // only test the first 1000 bytes. Testing the full file takes too long
    for (int i = 0; i < 1000; ++i) {
        QByteArray corrupted = binary;
        corrupted[i] = char(0xff);
        QJsonDocument doc = QJsonDocument::fromBinaryData(corrupted);
        if (doc.isNull())
            continue;
        QByteArray json = doc.toJson();
    }


    QFile file2(":/test3.json");
    file2.open(QFile::ReadOnly);
    testJson = file2.readAll();
    QVERIFY(!testJson.isEmpty());

    doc = QJsonDocument::fromJson(testJson);
    QVERIFY(!doc.isNull());

    binary = doc.toBinaryData();

    for (int i = 0; i < binary.size(); ++i) {
        QByteArray corrupted = binary;
        corrupted[i] = char(0xff);
        QJsonDocument doc = QJsonDocument::fromBinaryData(corrupted);
        if (doc.isNull())
            continue;
        QByteArray json = doc.toJson();

        corrupted = binary;
        corrupted[i] = 0x00;
        doc = QJsonDocument::fromBinaryData(corrupted);
        if (doc.isNull())
            continue;
        json = doc.toJson();
    }
}

void tst_QtJson::assignToDocument()
{
    {
        const char *json = "{ \"inner\": { \"key\": true } }";
        QJsonDocument doc = QJsonDocument::fromJson(json);

        QJsonObject o = doc.object();
        QJsonValue inner = o.value("inner");

        QJsonDocument innerDoc(inner.toObject());

        QVERIFY(innerDoc != doc);
        QVERIFY(innerDoc.object() == inner.toObject());
    }
    {
        const char *json = "[ [ true ] ]";
        QJsonDocument doc = QJsonDocument::fromJson(json);

        QJsonArray a = doc.array();
        QJsonValue inner = a.at(0);

        QJsonDocument innerDoc(inner.toArray());

        QVERIFY(innerDoc != doc);
        QVERIFY(innerDoc.array() == inner.toArray());
    }
}


void tst_QtJson::testDuplicateKeys()
{
    QJsonObject obj;
    obj.insert(QLatin1String("foo"), QLatin1String("bar"));
    obj.insert(QLatin1String("foo"), QLatin1String("zap"));
    QCOMPARE(obj.size(), 1);
    QCOMPARE(obj.value(QLatin1String("foo")).toString(), QLatin1String("zap"));
}

void tst_QtJson::testCompaction()
{
    // modify object enough times to trigger compactionCounter
    // and make sure the data is still valid
    QJsonObject obj;
    for (int i = 0; i < 33; ++i) {
        obj.remove(QLatin1String("foo"));
        obj.insert(QLatin1String("foo"), QLatin1String("bar"));
    }
    QCOMPARE(obj.size(), 1);
    QCOMPARE(obj.value(QLatin1String("foo")).toString(), QLatin1String("bar"));

    QJsonDocument doc = QJsonDocument::fromBinaryData(QJsonDocument(obj).toBinaryData());
    QVERIFY(!doc.isNull());
    QVERIFY(!doc.isEmpty());
    QCOMPARE(doc.isArray(), false);
    QCOMPARE(doc.isObject(), true);
    QVERIFY(doc.object() == obj);
}

void tst_QtJson::testDebugStream()
{
    {
        // QJsonObject

        QJsonObject object;
        QTest::ignoreMessage(QtDebugMsg, "QJsonObject() ");
        qDebug() << object;

        object.insert(QLatin1String("foo"), QLatin1String("bar"));
        QTest::ignoreMessage(QtDebugMsg, "QJsonObject({\"foo\":\"bar\"}) ");
        qDebug() << object;
    }

    {
        // QJsonArray

        QJsonArray array;
        QTest::ignoreMessage(QtDebugMsg, "QJsonArray() ");
        qDebug() << array;

        array.append(1);
        array.append(QLatin1String("foo"));
        QTest::ignoreMessage(QtDebugMsg, "QJsonArray([1,\"foo\"]) ");
        qDebug() << array;
    }

    {
        // QJsonDocument

        QJsonDocument doc;
        QTest::ignoreMessage(QtDebugMsg, "QJsonDocument() ");
        qDebug() << doc;

        QJsonObject object;
        object.insert(QLatin1String("foo"), QLatin1String("bar"));
        doc.setObject(object);
        QTest::ignoreMessage(QtDebugMsg, "QJsonDocument({\"foo\":\"bar\"}) ");
        qDebug() << doc;

        QJsonArray array;
        array.append(1);
        array.append(QLatin1String("foo"));
        QTest::ignoreMessage(QtDebugMsg, "QJsonDocument([1,\"foo\"]) ");
        doc.setArray(array);
        qDebug() << doc;
    }

    {
        // QJsonValue

        QJsonValue value;

        QTest::ignoreMessage(QtDebugMsg, "QJsonValue(null) ");
        qDebug() << value;

        value = QJsonValue(true); // bool
        QTest::ignoreMessage(QtDebugMsg, "QJsonValue(bool, true) ");
        qDebug() << value;

        value = QJsonValue((double)4.2); // double
        QTest::ignoreMessage(QtDebugMsg, "QJsonValue(double, 4.2) ");
        qDebug() << value;

        value = QJsonValue((int)42); // int
        QTest::ignoreMessage(QtDebugMsg, "QJsonValue(double, 42) ");
        qDebug() << value;

        value = QJsonValue(QLatin1String("foo")); // string
        QTest::ignoreMessage(QtDebugMsg, "QJsonValue(string, \"foo\") ");
        qDebug() << value;

        QJsonArray array;
        array.append(1);
        array.append(QLatin1String("foo"));
        value = QJsonValue(array); // array
        QTest::ignoreMessage(QtDebugMsg, "QJsonValue(array, QJsonArray([1,\"foo\"]) ) ");
        qDebug() << value;

        QJsonObject object;
        object.insert(QLatin1String("foo"), QLatin1String("bar"));
        value = QJsonValue(object); // object
        QTest::ignoreMessage(QtDebugMsg, "QJsonValue(object, QJsonObject({\"foo\":\"bar\"}) ) ");
        qDebug() << value;
    }
}

void tst_QtJson::testCompactionError()
{
    QJsonObject schemaObject;
    schemaObject.insert("_Type", QLatin1String("_SchemaType"));
    schemaObject.insert("name", QLatin1String("Address"));
    schemaObject.insert("schema", QJsonObject());
    {
        QJsonObject content(schemaObject);
        QJsonDocument doc(content);
        QVERIFY(!doc.isNull());
        QByteArray hash = QCryptographicHash::hash(doc.toBinaryData(), QCryptographicHash::Md5).toHex();
        schemaObject.insert("_Version", QString::fromLatin1(hash.constData(), hash.size()));
    }

    QJsonObject schema;
    schema.insert("streetNumber", schema.value("number").toObject());
    schemaObject.insert("schema", schema);
    {
        QJsonObject content(schemaObject);
        content.remove("_Uuid");
        content.remove("_Version");
        QJsonDocument doc(content);
        QVERIFY(!doc.isNull());
        QByteArray hash = QCryptographicHash::hash(doc.toBinaryData(), QCryptographicHash::Md5).toHex();
        schemaObject.insert("_Version", QString::fromLatin1(hash.constData(), hash.size()));
    }
}

void tst_QtJson::parseUnicodeEscapes()
{
    const QByteArray json = "[ \"A\\u00e4\\u00C4\" ]";

    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonArray array = doc.array();

    QString result = QLatin1String("A");
    result += QChar(0xe4);
    result += QChar(0xc4);

    QCOMPARE(array.first().toString(), result);
}

void tst_QtJson::assignObjects()
{
    const char *json =
            "[ { \"Key\": 1 }, { \"Key\": 2 } ]";

    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonArray array = doc.array();

    QJsonObject object = array.at(0).toObject();
    QCOMPARE(object.value("Key").toDouble(), 1.);

    object = array.at(1).toObject();
    QCOMPARE(object.value("Key").toDouble(), 2.);
}

void tst_QtJson::assignArrays()
{
    const char *json =
            "[ [ 1 ], [ 2 ] ]";

    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonArray array = doc.array();

    QJsonArray inner = array.at(0).toArray()  ;
    QCOMPARE(inner.at(0).toDouble(), 1.);

    inner= array.at(1).toArray();
    QCOMPARE(inner.at(0).toDouble(), 2.);
}

void tst_QtJson::testTrailingComma()
{
    const char *jsons[] = { "{ \"Key\": 1, }", "[ { \"Key\": 1 }, ]" };

    for (unsigned i = 0; i < sizeof(jsons)/sizeof(jsons[0]); ++i) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(jsons[i], &error);
        QCOMPARE(error.error, QJsonParseError::MissingObject);
    }
}

void tst_QtJson::testDetachBug()
{
    QJsonObject dynamic;
    QJsonObject embedded;

    QJsonObject local;

    embedded.insert("Key1", QString("Value1"));
    embedded.insert("Key2", QString("Value2"));
    dynamic.insert(QLatin1String("Bogus"), QString("bogusValue"));
    dynamic.insert("embedded", embedded);
    local = dynamic.value("embedded").toObject();

    dynamic.remove("embedded");

    QCOMPARE(local.keys().size(),2);
    local.remove("Key1");
    local.remove("Key2");
    QCOMPARE(local.keys().size(), 0);

    local.insert("Key1", QString("anotherValue"));
    QCOMPARE(local.keys().size(), 1);
}

void tst_QtJson::valueEquals()
{
    QVERIFY(QJsonValue() == QJsonValue());
    QVERIFY(QJsonValue() != QJsonValue(QJsonValue::Undefined));
    QVERIFY(QJsonValue() != QJsonValue(true));
    QVERIFY(QJsonValue() != QJsonValue(1.));
    QVERIFY(QJsonValue() != QJsonValue(QJsonArray()));
    QVERIFY(QJsonValue() != QJsonValue(QJsonObject()));

    QVERIFY(QJsonValue(true) == QJsonValue(true));
    QVERIFY(QJsonValue(true) != QJsonValue(false));
    QVERIFY(QJsonValue(true) != QJsonValue(QJsonValue::Undefined));
    QVERIFY(QJsonValue(true) != QJsonValue());
    QVERIFY(QJsonValue(true) != QJsonValue(1.));
    QVERIFY(QJsonValue(true) != QJsonValue(QJsonArray()));
    QVERIFY(QJsonValue(true) != QJsonValue(QJsonObject()));

    QVERIFY(QJsonValue(1) == QJsonValue(1));
    QVERIFY(QJsonValue(1) != QJsonValue(2));
    QVERIFY(QJsonValue(1) == QJsonValue(1.));
    QVERIFY(QJsonValue(1) != QJsonValue(1.1));
    QVERIFY(QJsonValue(1) != QJsonValue(QJsonValue::Undefined));
    QVERIFY(QJsonValue(1) != QJsonValue());
    QVERIFY(QJsonValue(1) != QJsonValue(true));
    QVERIFY(QJsonValue(1) != QJsonValue(QJsonArray()));
    QVERIFY(QJsonValue(1) != QJsonValue(QJsonObject()));

    QVERIFY(QJsonValue(1.) == QJsonValue(1.));
    QVERIFY(QJsonValue(1.) != QJsonValue(2.));
    QVERIFY(QJsonValue(1.) != QJsonValue(QJsonValue::Undefined));
    QVERIFY(QJsonValue(1.) != QJsonValue());
    QVERIFY(QJsonValue(1.) != QJsonValue(true));
    QVERIFY(QJsonValue(1.) != QJsonValue(QJsonArray()));
    QVERIFY(QJsonValue(1.) != QJsonValue(QJsonObject()));

    QVERIFY(QJsonValue(QJsonArray()) == QJsonValue(QJsonArray()));
    QJsonArray nonEmptyArray;
    nonEmptyArray.append(true);
    QVERIFY(QJsonValue(QJsonArray()) != nonEmptyArray);
    QVERIFY(QJsonValue(QJsonArray()) != QJsonValue(QJsonValue::Undefined));
    QVERIFY(QJsonValue(QJsonArray()) != QJsonValue());
    QVERIFY(QJsonValue(QJsonArray()) != QJsonValue(true));
    QVERIFY(QJsonValue(QJsonArray()) != QJsonValue(1.));
    QVERIFY(QJsonValue(QJsonArray()) != QJsonValue(QJsonObject()));

    QVERIFY(QJsonValue(QJsonObject()) == QJsonValue(QJsonObject()));
    QJsonObject nonEmptyObject;
    nonEmptyObject.insert("Key", true);
    QVERIFY(QJsonValue(QJsonObject()) != nonEmptyObject);
    QVERIFY(QJsonValue(QJsonObject()) != QJsonValue(QJsonValue::Undefined));
    QVERIFY(QJsonValue(QJsonObject()) != QJsonValue());
    QVERIFY(QJsonValue(QJsonObject()) != QJsonValue(true));
    QVERIFY(QJsonValue(QJsonObject()) != QJsonValue(1.));
    QVERIFY(QJsonValue(QJsonObject()) != QJsonValue(QJsonArray()));

    QVERIFY(QJsonValue("foo") == QJsonValue(QLatin1String("foo")));
    QVERIFY(QJsonValue("foo") == QJsonValue(QString("foo")));
    QVERIFY(QJsonValue("\x66\x6f\x6f") == QJsonValue(QString("foo")));
    QVERIFY(QJsonValue("\x62\x61\x72") == QJsonValue("bar"));
    /*
    QVERIFY(QJsonValue(UNICODE_NON_CHARACTER) == QJsonValue(QString(UNICODE_NON_CHARACTER)));
    QVERIFY(QJsonValue(UNICODE_DJE) == QJsonValue(QString(UNICODE_DJE)));
    QVERIFY(QJsonValue("\xc3\xa9") == QJsonValue(QString("\xc3\xa9")));
    */
}

void tst_QtJson::objectEquals_data()
{
    QTest::addColumn<QJsonObject>("left");
    QTest::addColumn<QJsonObject>("right");
    QTest::addColumn<bool>("result");

    QTest::newRow("two defaults") << QJsonObject() << QJsonObject() << true;

    QJsonObject object1;
    object1.insert("property", 1);
    QJsonObject object2;
    object2["property"] = 1;
    QJsonObject object3;
    object3.insert("property1", 1);
    object3.insert("property2", 2);

    QTest::newRow("the same object (1 vs 2)") << object1 << object2 << true;
    QTest::newRow("the same object (3 vs 3)") << object3 << object3 << true;
    QTest::newRow("different objects (2 vs 3)") << object2 << object3 << false;
    QTest::newRow("object vs default") << object1 << QJsonObject() << false;

    QJsonObject empty;
    empty.insert("property", 1);
    empty.take("property");
    QTest::newRow("default vs empty") << QJsonObject() << empty << true;
    QTest::newRow("empty vs empty") << empty << empty << true;
    QTest::newRow("object vs empty") << object1 << empty << false;

    QJsonObject referencedEmpty;
    referencedEmpty["undefined"];
    QTest::newRow("referenced empty vs referenced empty") << referencedEmpty << referencedEmpty << true;
    QTest::newRow("referenced empty vs object") << referencedEmpty << object1 << false;

    QJsonObject referencedObject1;
    referencedObject1.insert("property", 1);
    referencedObject1["undefined"];
    QJsonObject referencedObject2;
    referencedObject2.insert("property", 1);
    referencedObject2["aaaaaaaaa"]; // earlier then "property"
    referencedObject2["zzzzzzzzz"]; // after "property"
    QTest::newRow("referenced object vs default") << referencedObject1 << QJsonObject() << false;
    QTest::newRow("referenced object vs referenced object") << referencedObject1 << referencedObject1 << true;
    QTest::newRow("referenced object vs object (different)") << referencedObject1 << object3 << false;
}

void tst_QtJson::objectEquals()
{
    QFETCH(QJsonObject, left);
    QFETCH(QJsonObject, right);
    QFETCH(bool, result);

    QCOMPARE(left == right, result);
    QCOMPARE(right == left, result);

    // invariants checks
    QCOMPARE(left, left);
    QCOMPARE(right, right);
    QCOMPARE(left != right, !result);
    QCOMPARE(right != left, !result);

    // The same but from QJsonValue perspective
    QCOMPARE(QJsonValue(left) == QJsonValue(right), result);
    QCOMPARE(QJsonValue(left) != QJsonValue(right), !result);
    QCOMPARE(QJsonValue(right) == QJsonValue(left), result);
    QCOMPARE(QJsonValue(right) != QJsonValue(left), !result);
}

void tst_QtJson::arrayEquals_data()
{
    QTest::addColumn<QJsonArray>("left");
    QTest::addColumn<QJsonArray>("right");
    QTest::addColumn<bool>("result");

    QTest::newRow("two defaults") << QJsonArray() << QJsonArray() << true;

    QJsonArray array1;
    array1.append(1);
    QJsonArray array2;
    array2.append(2111);
    array2[0] = 1;
    QJsonArray array3;
    array3.insert(0, 1);
    array3.insert(1, 2);

    QTest::newRow("the same array (1 vs 2)") << array1 << array2 << true;
    QTest::newRow("the same array (3 vs 3)") << array3 << array3 << true;
    QTest::newRow("different arrays (2 vs 3)") << array2 << array3 << false;
    QTest::newRow("array vs default") << array1 << QJsonArray() << false;

    QJsonArray empty;
    empty.append(1);
    empty.takeAt(0);
    QTest::newRow("default vs empty") << QJsonArray() << empty << true;
    QTest::newRow("empty vs default") << empty << QJsonArray() << true;
    QTest::newRow("empty vs empty") << empty << empty << true;
    QTest::newRow("array vs empty") << array1 << empty << false;
}

void tst_QtJson::arrayEquals()
{
    QFETCH(QJsonArray, left);
    QFETCH(QJsonArray, right);
    QFETCH(bool, result);

    QCOMPARE(left == right, result);
    QCOMPARE(right == left, result);

    // invariants checks
    QCOMPARE(left, left);
    QCOMPARE(right, right);
    QCOMPARE(left != right, !result);
    QCOMPARE(right != left, !result);

    // The same but from QJsonValue perspective
    QCOMPARE(QJsonValue(left) == QJsonValue(right), result);
    QCOMPARE(QJsonValue(left) != QJsonValue(right), !result);
    QCOMPARE(QJsonValue(right) == QJsonValue(left), result);
    QCOMPARE(QJsonValue(right) != QJsonValue(left), !result);
}

void tst_QtJson::bom()
{
    QFile file(":/bom.json");
    file.open(QFile::ReadOnly);
    QByteArray json = file.readAll();

    // Import json document into a QJsonDocument
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(!doc.isNull());
    QVERIFY(error.error == QJsonParseError::NoError);
}

void tst_QtJson::nesting()
{
    // check that we abort parsing too deeply nested json documents.
    // this is to make sure we don't crash because the parser exhausts the
    // stack.

    const char *array_data =
            "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
            "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
            "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
            "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
            "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
            "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
            "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
            "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["
            "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
            "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
            "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
            "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
            "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
            "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
            "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
            "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]";

    QByteArray json(array_data);
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(!doc.isNull());
    QVERIFY(error.error == QJsonParseError::NoError);

    json.prepend("[");
    json.append("]");
    doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(doc.isNull());
    QVERIFY(error.error == QJsonParseError::DeepNesting);

    json = QByteArray("true ");

    for (int i = 0; i < 1024; ++i) {
        json.prepend("{ \"Key\": ");
        json.append(" }");
    }

    doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(!doc.isNull());
    QVERIFY(error.error == QJsonParseError::NoError);

    json.prepend("[");
    json.append("]");
    doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(doc.isNull());
    QVERIFY(error.error == QJsonParseError::DeepNesting);

}

void tst_QtJson::longStrings()
{
    // test around 15 and 16 bit boundaries, as these are limits
    // in the data structures (for Latin1String in qjson_p.h)
    QString s(0x7ff0, 'a');
    for (int i = 0x7ff0; i < 0x8010; i++) {
        s.append("c");

        QMap <QString, QVariant> map;
        map["key"] = s;

        /* Create a QJsonDocument from the QMap ... */
        QJsonDocument d1 = QJsonDocument::fromVariant(QVariant(map));
        /* ... and a QByteArray from the QJsonDocument */
        QByteArray a1 = d1.toJson();

        /* Create a QJsonDocument from the QByteArray ... */
        QJsonDocument d2 = QJsonDocument::fromJson(a1);
        /* ... and a QByteArray from the QJsonDocument */
        QByteArray a2 = d2.toJson();
        QVERIFY(a1 == a2);
    }

    s = QString(0xfff0, 'a');
    for (int i = 0xfff0; i < 0x10010; i++) {
        s.append("c");

        QMap <QString, QVariant> map;
        map["key"] = s;

        /* Create a QJsonDocument from the QMap ... */
        QJsonDocument d1 = QJsonDocument::fromVariant(QVariant(map));
        /* ... and a QByteArray from the QJsonDocument */
        QByteArray a1 = d1.toJson();

        /* Create a QJsonDocument from the QByteArray ... */
        QJsonDocument d2 = QJsonDocument::fromJson(a1);
        /* ... and a QByteArray from the QJsonDocument */
        QByteArray a2 = d2.toJson();
        QVERIFY(a1 == a2);
    }
}

void tst_QtJson::testJsonValueRefDefault()
{
    QJsonObject empty;

    QCOMPARE(empty["n/a"].toString(), QString());
    QCOMPARE(empty["n/a"].toString("default"), QLatin1String("default"));

    QCOMPARE(empty["n/a"].toBool(), false);
    QCOMPARE(empty["n/a"].toBool(true), true);

    QCOMPARE(empty["n/a"].toInt(), 0);
    QCOMPARE(empty["n/a"].toInt(42), 42);

    QCOMPARE(empty["n/a"].toDouble(), 0.0);
    QCOMPARE(empty["n/a"].toDouble(42.0), 42.0);
}

void tst_QtJson::arrayInitializerList()
{
#ifndef Q_COMPILER_INITIALIZER_LISTS
    QSKIP("initializer_list is enabled only with c++11 support", SkipSingle);
#else
    QVERIFY(QJsonArray{}.isEmpty());
    QCOMPARE(QJsonArray{"one"}.count(), 1);
    QCOMPARE(QJsonArray{1}.count(), 1);

    {
        QJsonArray a{1.3, "hello", 0};
        QCOMPARE(QJsonValue(a[0]), QJsonValue(1.3));
        QCOMPARE(QJsonValue(a[1]), QJsonValue("hello"));
        QCOMPARE(QJsonValue(a[2]), QJsonValue(0));
        QCOMPARE(a.count(), 3);
    }
    {
        QJsonObject o;
        o["property"] = 1;
        QJsonArray a1 {o};
        QCOMPARE(a1.count(), 1);
        QCOMPARE(a1[0].toObject(), o);

        QJsonArray a2 {o, 23};
        QCOMPARE(a2.count(), 2);
        QCOMPARE(a2[0].toObject(), o);
        QCOMPARE(QJsonValue(a2[1]), QJsonValue(23));

        QJsonArray a3 { a1, o, a2 };
        QCOMPARE(QJsonValue(a3[0]), QJsonValue(a1));
        QCOMPARE(QJsonValue(a3[1]), QJsonValue(o));
        QCOMPARE(QJsonValue(a3[2]), QJsonValue(a2));

        QJsonArray a4 { 1, QJsonArray{1,2,3}, QJsonArray{"hello", 2}, QJsonObject{{"one", 1}} };
        QCOMPARE(a4.count(), 4);
        QCOMPARE(QJsonValue(a4[0]), QJsonValue(1));

        {
            QJsonArray a41 = a4[1].toArray();
            QJsonArray a42 = a4[2].toArray();
            QJsonObject a43 = a4[3].toObject();
            QCOMPARE(a41.count(), 3);
            QCOMPARE(a42.count(), 2);
            QCOMPARE(a43.count(), 1);

            QCOMPARE(QJsonValue(a41[2]), QJsonValue(3));
            QCOMPARE(QJsonValue(a42[1]), QJsonValue(2));
            QCOMPARE(QJsonValue(a43["one"]), QJsonValue(1));
        }
    }
#endif
}

void tst_QtJson::objectInitializerList()
{
#ifndef Q_COMPILER_INITIALIZER_LISTS
    QSKIP("initializer_list is enabled only with c++11 support", SkipSingle);
#else
    QVERIFY(QJsonObject{}.isEmpty());

    {   // one property
        QJsonObject one {{"one", 1}};
        QCOMPARE(one.count(), 1);
        QVERIFY(one.contains("one"));
        QCOMPARE(QJsonValue(one["one"]), QJsonValue(1));
    }
    {   // two properties
        QJsonObject two {
                           {"one", 1},
                           {"two", 2}
                        };
        QCOMPARE(two.count(), 2);
        QVERIFY(two.contains("one"));
        QVERIFY(two.contains("two"));
        QCOMPARE(QJsonValue(two["one"]), QJsonValue(1));
        QCOMPARE(QJsonValue(two["two"]), QJsonValue(2));
    }
    {   // nested object
        QJsonObject object{{"nested", QJsonObject{{"innerProperty", 2}}}};
        QCOMPARE(object.count(), 1);
        QVERIFY(object.contains("nested"));
        QVERIFY(object["nested"].isObject());

        QJsonObject nested = object["nested"].toObject();
        QCOMPARE(QJsonValue(nested["innerProperty"]), QJsonValue(2));
    }
    {   // nested array
        QJsonObject object{{"nested", QJsonArray{"innerValue", 2.1, "bum cyk cyk"}}};
        QCOMPARE(object.count(), 1);
        QVERIFY(object.contains("nested"));
        QVERIFY(object["nested"].isArray());

        QJsonArray nested = object["nested"].toArray();
        QCOMPARE(nested.count(), 3);
        QCOMPARE(QJsonValue(nested[0]), QJsonValue("innerValue"));
        QCOMPARE(QJsonValue(nested[1]), QJsonValue(2.1));
    }
#endif
}

void tst_QtJson::unicodeKeys()
{
    QByteArray json = "{"
                      "\"x\\u2090_1\": \"hello_1\","
                      "\"y\\u2090_2\": \"hello_2\","
                      "\"T\\u2090_3\": \"hello_3\","
                      "\"xyz_4\": \"hello_4\","
                      "\"abc_5\": \"hello_5\""
                      "}";

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QJsonObject o = doc.object();

    QCOMPARE(o.keys().size(), 5);
    Q_FOREACH (const QString &key, o.keys()) {
        QString suffix = key.mid(key.indexOf(QLatin1Char('_')));
        QCOMPARE(o[key].toString(), QString("hello") + suffix);
    }
}

void tst_QtJson::garbageAtEnd()
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson("{},", &error);
    QVERIFY(error.error == QJsonParseError::GarbageAtEnd);
    QVERIFY(error.offset == 2);
    QVERIFY(doc.isEmpty());

    doc = QJsonDocument::fromJson("{}    ", &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(!doc.isEmpty());
}

QTEST_MAIN(tst_QtJson)
#include "tst_qtjson.moc"
