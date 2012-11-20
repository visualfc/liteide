/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
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
// Module: golangapi.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-24
// $Id: golangapi.h,v 1.0 2012-4-24 visualfc Exp $

#ifndef GOLANGAPI_H
#define GOLANGAPI_H

#include "golangdocapi/golangdocapi.h"

#include <QTextStream>
#include <QSharedData>
#include <QThread>

using namespace LiteApi;

class Value
{
public:
    Value(): typ(NullApi) {}
    Value(PkgApiEnum _typ, const QString &_name, const QString &_exp) :
        typ(_typ), name(_name), exp(_exp) {}
public:
    PkgApiEnum typ;
    QString name;
    QString exp;
};

class Type
{
public:
    Type() : typ(NullApi){}
    Type(PkgApiEnum _typ, const QString &_name, const QString &_exp) :
        typ(_typ), name(_name), exp(_exp) {}
    ~Type() { clear(); }
    void clear() {
        qDeleteAll(valueList);
        valueList.clear();
    }
    bool IsNull() const { return typ == NullApi; }
    Value *findValue(const QString &valueName) {
        foreach(Value *val, valueList) {
            if (val->name == valueName) {
                return val;
            }
        }
        return 0;
    }
public:
    PkgApiEnum    typ;
    QString     name;
    QString     exp;
    QStringList     embeddedList;
    QList<Value*> valueList;
};

class Package
{
public:
    Package() : typ(PkgApi) {}
    Package(const QString &_name) :
        typ(PkgApi),name(_name) {}
    ~Package()  { clear(); }
    void clear() {
        qDeleteAll(valueList);
        qDeleteAll(typeList);
        valueList.clear();
        typeList.clear();
    }
    Type *findType(const QString &typeName) {
        foreach(Type *typ, typeList) {
            if (typ->name == typeName) {
                return typ;
            }
        }
        return 0;
    }
    Value *findValue(const QString &valueName) {
        foreach(Value *val, valueList) {
            if (val->name == valueName) {
                return val;
            }
        }
        return 0;
    }
public:
    PkgApiEnum    typ;
    QString       name;
    QList<Value*> valueList;
    QList<Type*>  typeList;
};

class Packages
{
public:
    Packages() {}
    ~Packages() { clear(); }
    void clear() {
        qDeleteAll(pkgList);
        pkgList.clear();
    }
    Package *findPackage(const QString &pkgName) const {
        foreach(Package *pkg, pkgList) {
            if (pkg->name == pkgName) {
                return pkg;
            }
        }
        return 0;
    }
public:
    QList<Package*> pkgList;
};

/*
class ValueData : public QSharedData
{
public:
    ValueData(): typ(NullApi) {}
    ValueData(const ValueData &other) :
        typ(other.typ), name(other.name),exp(other.exp)
    {}
    PkgApiEnum typ;
    QString name;
    QString exp;
};

class Value
{
public:
    Value() { d = new ValueData; }
    Value(PkgApiEnum typ, const QString &name, const QString &exp) {
        d = new ValueData;
        d->typ = typ;
        d->name = name;
        d->exp = exp;
    }
    Value(const Value &other) :
        d(other.d) {}
    bool IsNull() const { return d->typ == NullApi; }
    PkgApiEnum type() const { return d->typ; }
    QString name() const { return d->name; }
private:
    QSharedDataPointer<ValueData> d;
};

class TypeData : public QSharedData
{
public:
    TypeData() : typ(NullApi){}
    TypeData(const TypeData &other)
        : QSharedData(other),
          typ(other.typ),
          name(other.name),
          exp(other.exp),
          valueList(other.valueList)
    {}
    ~TypeData() {}
    PkgApiEnum    typ;
    QString     name;
    QString     exp;
    QList<Value> valueList;
};

class Type
{
public:
    Type() { d = new TypeData; }
    Type(PkgApiEnum typ, const QString &name, const QString &exp) {
        d = new TypeData;
        d->typ = typ;
        d->name = name;
        d->exp = exp;
    }
    Type(const Type &other)
        : d(other.d) {}
    bool IsNull() const { return d->typ == NullApi; }
    PkgApiEnum type() const { return d->typ; }
    QString name() const { return d->name; }
    QList<Value> valueList() const { return d->valueList; }
    void appendValue(const Value &value) { d->valueList.append(value); }
private:
     QSharedDataPointer<TypeData> d;
};

class PackageData : public QSharedData
{
public:
    PackageData() {}
    PackageData(const PackageData &other)
        : QSharedData(other),
          name(other.name),
          valueList(other.valueList),
          typeList(other.typeList)
    {}
    ~PackageData()  { }
    QString     name;
    QList<Value> valueList;
    QList<Type>  typeList;
};

class Package
{
public:
    Package(){d = new PackageData;}
    Package(const QString &name) {
        d = new PackageData;
        d->name = name;
    }

    Package(const Package &other)
        : d(other.d) {}
    ~Package() {}
    QString name() const { return d->name; }
    QList<Value> valueList() const {return d->valueList; }
    QList<Type> typeList() const { return d->typeList; }
    void appendValue(const Value &value) {
        d->valueList.append(value);
    }
    void appendType(const Type &typ) {
        d->typeList.append(typ);
    }
    Type findType(const QString &typeName) {
        QListIterator<Type> it(d->typeList);
        while (it.hasNext()) {
            Type typ = it.next();
            if (typ.name() == typeName) {
                return typ;
            }
        }
        return Type();
    }
protected:
    QSharedDataPointer<PackageData> d;
};
*/

class GolangApi : public LiteApi::IGolangApi
{
    Q_OBJECT
public:
    GolangApi(QObject *parent = 0);
    bool load(const QString &fileName);
    bool loadStream(QTextStream *stream);
    virtual QStringList all(int flag) const;
    virtual PkgApiEnum findExp(const QString &tag, QString &exp) const;
    virtual QStringList findDocUrl(const QString &tag) const;
    virtual QString findDocInfo(const QString &tag) const;
protected:
    Packages m_pkgs;
};

class GolangApiThread : public QThread
{
public:
    GolangApiThread(QObject *parent);
    ~GolangApiThread();
    void loadData(const QByteArray &data);
    void loadFile(const QString &fileName);
    LiteApi::IGolangApi* api() const;
    QStringList all() const;
    QByteArray data() const;
protected:
    virtual void run();
    QByteArray m_data;
    QString    m_file;
    QStringList m_all;
    GolangApi  *m_api;
};

#endif // GOLANGAPI_H
