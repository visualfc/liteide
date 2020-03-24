/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2019 LiteIDE. All rights reserved.
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
// Module: golangapi.cpp
// Creator: visualfc <visualfc@gmail.com>

#include "golangapi.h"
#include <QFile>
#include <QSet>
#include <QRegExp>
#include <QTextStream>
#include <QFile>
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

GolangApi::GolangApi(QObject *parent) :
    LiteApi::IGolangApi(parent)
{
}

bool GolangApi::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    QTextStream stream(&file);
    stream.setCodec("utf-8");
    return loadStream(&stream);
}

bool GolangApi::loadStream(QTextStream *stream)
{
    m_pkgs.clear();

    //pkg syscall (darwin-386), const TIOCSBRK ideal-int

    //var
    //pkg archive/tar, var ErrFieldTooLong error

    //const
    //pkg archive/tar, const TypeBlock ideal-char
    //pkg crypto/sha512, const BlockSize ideal-int
    //pkg archive/zip, const Deflate uint16

    //type-const
    //pkg go/ast, type ObjKind int
    //pkg syscall, type Errno uintptr
    //pkg syscall (windows-amd64), const ECOMM Errno

    // func
    //pkg archive/tar, func NewReader(io.Reader) *Reader
    //pkg bytes, func FieldsFunc([]byte, func(rune) bool) [][]byte

    //type-var,method
    //pkg archive/tar, type Header struct
    //pkg archive/tar, type Header struct, AccessTime time.Time
    //pkg archive/tar, method (*Reader) Next() (*Header, error)
    //pkg archive/zip, method (*File) Open() (io.ReadCloser, error)
    //pkg bufio, method (ReadWriter) Available() int

    //type-interface
    //pkg container/heap, type Interface interface { Len, Less, Pop, Push, Swap }
    //pkg container/heap, type Interface interface, Len() int

    QRegExp reg("^pkg\\s([\\w\\-\\.\\/]+)(\\s\\(([\\w\\-]+)\\))?,\\s(\\w+)");
    QRegExp regm("\\(\\*?([\\w\\-]+)\\)\\s*(\\w+)");
    Package *lastPkg = 0;
    Type *lastType = 0;
    while (!stream->atEnd()) {
        QString line = stream->readLine();
        int pos = reg.indexIn(line);
        if (pos < 0) {
            continue;
        }
        // 1 pkgname
        // 2 ? (system)
        // 3 ? system
        // 4 const|func|method|var|type
        QString pkgName = reg.cap(1);
//        if (!reg.cap(3).isEmpty()) {
//            pkg = reg.cap(2)+"."+pkg;
//        }
        if (!lastPkg || lastPkg->name != pkgName) {
            lastPkg = m_pkgs.findPackage(pkgName);
            if (!lastPkg) {
                lastPkg = new Package(pkgName);
                m_pkgs.pkgList.append(lastPkg);
                lastType = 0;
            }
        }
        QString right = line.mid(reg.cap().length()).trimmed();
        QString flag = reg.cap(4);
        if (flag == "var") {
            ///pkg archive/tar, var ErrFieldTooLong error
            int pos = right.indexOf(" ");
            QString name = right.left(pos);
            if (pos != -1 && lastPkg->findValue(name) == 0) {
                lastPkg->valueList.append(new Value(VarApi,name,right.mid(pos+1)));
            }
        } else if (flag == "const") {
            //pkg syscall (windows-amd64), const ECOMM Errno
            int pos = right.indexOf(" ");
            QString name = right.left(pos);
            if (pos != -1 && lastPkg->findValue(name) == 0) {
                lastPkg->valueList.append(new Value(ConstApi,name,right.mid(pos+1)));
            }
        } else if (flag == "func") {
            //pkg bytes, func FieldsFunc([]byte, func(rune) bool) [][]byte
            int pos = right.indexOf("(");
            QString name = right.left(pos);
            if (pos != -1 && lastPkg->findValue(name) == 0) {
                lastPkg->valueList.append(new Value(FuncApi,name,right.mid(pos)));
            }
        } else if (flag == "method") {
            //pkg archive/tar, method (*Reader) Next() (*Header, error)
            //pkg archive/zip, method (*File) Open() (io.ReadCloser, error)
            //pkg bufio, method (ReadWriter) Available() int
            int pos = regm.indexIn(right);
            if (pos != -1) {
                QString typeName = regm.cap(1);
                QString name = regm.cap(2);
                QString exp = right.mid(regm.cap().length()).trimmed();
                if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
                    lastType = lastPkg->findType(typeName);
                    if (!lastType) {
                        lastType = new Type(StructApi,typeName,"struct");
                        lastPkg->typeList.append(lastType);
                    }
                }
                if (lastType->findValue(name) == 0) {
                    lastType->valueList.append(new Value(TypeMethodApi,name,exp));
                }
            }
        } else if (flag == "type") {
            //pkg go/ast, type ObjKind int
            //pkg archive/tar, type Header struct
            //pkg archive/tar, type Header struct, AccessTime time.Time
            //pkg container/heap, type Interface interface { Len, Less, Pop, Push, Swap }
            //pkg container/heap, type Interface interface, Len() int
            int pos = right.indexOf(" ");
            if (pos != -1) {
                QString typeName = right.left(pos);
                QString exp = right.mid(pos+1);
                if (exp == "struct") {
                    lastType = lastPkg->findType(typeName);
                    if (!lastType) {
                        lastType = new Type(StructApi,typeName,exp);
                        lastPkg->typeList.append(lastType);
                    }
                } else if (exp.startsWith("struct,")) {
                    QString last = exp.mid(7).trimmed();
                    int pos2 = last.indexOf(" ");
                    if (pos2 != -1) {
                        if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
                            lastType = lastPkg->findType(typeName);
                            if (!lastType) {
                                lastType = new Type(StructApi,typeName,"struct");
                                lastPkg->typeList.append(lastType);
                            }
                        }
                        QString name = last.left(pos2);
                        if (name == "embedded") {
                            QString emName = last.mid(pos2+1);
                            if (!lastType->embeddedList.contains(emName)) {
                                lastType->embeddedList.append(emName);
                            }
                        } else if (lastType->findValue(name) == 0){
                            lastType->valueList.append(new Value(TypeVarApi,name,last.mid(pos2+1)));
                        }
                    }
                } else if (exp.startsWith("interface {")) {
                    lastType = lastPkg->findType(typeName);
                    if (!lastType) {
                        lastType = new Type(InterfaceApi,typeName,exp);
                        lastPkg->typeList.append(lastType);
                    }
                } else if (exp.startsWith("interface,")) {
                    QString last = exp.mid(10).trimmed();
                    int pos2 = last.indexOf("(");
                    if (pos2 != -1) {
                        if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
                            lastType = lastPkg->findType(typeName);
                            if (!lastType) {
                                lastType = new Type(InterfaceApi,typeName,"struct");
                                lastPkg->typeList.append(lastType);
                            }
                        }
                        QString name = last.left(pos2);
                        if (lastType->findValue(name) == 0) {
                            lastType->valueList.append(new Value(TypeMethodApi,name,last.mid(pos2)));
                        }
                    }
                } else {
                    lastType = lastPkg->findType(typeName);
                    if (!lastType) {
                        lastType = new Type(TypeApi,typeName,exp);
                        lastPkg->typeList.append(lastType);
                    }
                }
            }
        }
    }

//    QRegExp reg("^pkg\\s([\\w\\/]+)(\\s\\(([\\w-]+)\\))?,\\s(\\w+)(\\s\\(\\*?([\\w\\-]+)\\))?\\s(\\w+)");
//    QRegExp reg2("^(\\w+),\\s+(\\w+)");
//    while (!stream->atEnd()) {
//        QString line = stream->readLine().trimmed();
//        // 1 pkgname
//        // 2 ? (system)
//        // 3 ? system
//        // 4 const|func|method|var|type
//        // 5 ? (method-type)
//        // 6 ? method-type
//        // 7 value
//        int pos = reg.indexIn(line);
//        if (pos != -1) {
//            QString typeVar;
//            int next = reg2.indexIn(line.right(line.length()-reg.cap().length()).trimmed());
//            if (next != -1) {
//                typeVar = reg2.cap(2);
//            }

//            //qDebug() << reg.cap() << reg.captureCount();
//            QString pkg = reg.cap(1);
//            //QString sys = reg.cap(3);
//            QString flag = reg.cap(4);
//            QString method_type = reg.cap(6);
//            QString value = reg.cap(7);

//            QMap<QString,Package>::iterator it = pkgMap.find(pkg);
//            if (it == pkgMap.end()) {
//                it = pkgMap.insert(pkg,Package());
//            }

//            if (flag == "const") {
//                it->appendConst(value);
//                //constList.append(tag);
//            } else if (flag == "func") {
//                //funcList.append(tag);
//                it->appendFunc(value);
//            } else if (flag == "method") {
//                //methodList.append(tag);
//                it->appendTypeMethod(method_type,value);
//            } else if (flag == "type") {
//                //typeList.append(tag);
//                if (!typeVar.isEmpty()) {
//                    it->appendTypeVar(value,typeVar);
//                } else {
//                    it->insertType(value);
//                }
//            } else if (flag == "var") {
//                //varList.append(tag);
//                it->appendVar(value);
//            }
//        }
//    }
    return true;
}

QStringList GolangApi::all(int flag) const
{
    QStringList finds;
    foreach(Package *pkg, m_pkgs.pkgList) {
        finds.append(pkg->name);
        finds.append(pkg->name+".");
        foreach(Value *value, pkg->valueList) {
            if (flag & value->typ) {
                finds.append(pkg->name+"."+value->name);
            }
        }
        foreach(Type *type, pkg->typeList) {
            if (flag & type->typ) {
                QString name = pkg->name+"."+type->name;
                finds.append(name);
                name += ".";
                foreach (Value *value, type->valueList) {
                    if (flag & value->typ) {
                        finds.append(name+value->name);
                    }
                }
            }
        }
    }
    return finds;
}

static QStringList findType(Package *pkg, const QString &typeName, const QString &v)
{
    Type *typ = pkg->findType(typeName);
    if (typ) {
        Value *value = typ->findValue(v);
        if (value) {
            if (value->typ == TypeVarApi) {
                return QStringList() << pkg->name+"#"+typ->name << value->name;
            } else if (value->typ == TypeMethodApi ){
                return QStringList() << pkg->name+"#"+typ->name+"."+value->name << value->name;
            }
        } else {
            foreach(QString embedded, typ->embeddedList) {
                QStringList find = findType(pkg,embedded,v);
                if (!find.isEmpty()) {
                    return find;
                }
            }
        }
    }
    return QStringList();
}
/*
    NullApi = 0,
    PkgApi = 0x0001,
    ConstApi = 0x0002,
    VarApi = 0x0004,
    StructApi = 0x0008,
    InterfaceApi = 0x0010,
    TypeApi = 0x0020,
    FuncApi = 0x0040,
    TypeMethodApi = 0x0080,
    TypeVarApi = 0x0100,
*/

static QString typeName(LiteApi::PkgApiEnum api) {
    switch(api) {
    case LiteApi::NullApi:
        break;
    case LiteApi::PkgApi:
        return "package";
    case LiteApi::ConstApi:
        return "const";
    case LiteApi::VarApi:
        return "var";
    case LiteApi::StructApi:
        return "struct";
    case LiteApi::InterfaceApi:
        return "interface";
    case LiteApi::TypeApi:
        return "type";
    case LiteApi::FuncApi:
        return "func";
    case LiteApi::TypeMethodApi:
        return "method";
    case LiteApi::TypeVarApi:
        return "field";
    default:
        break;
    }
    return QString();
}

QString GolangApi::findDocInfo(const QString &tag) const
{
    int pos = tag.lastIndexOf("/");
    QString pkgName = tag.left(pos+1);
    QStringList all = tag.mid(pos+1).split(".",QString::SkipEmptyParts);
    if (all.size() >= 1) {
        pkgName += all.at(0);
        Package *pkg = m_pkgs.findPackage(pkgName);
        if (pkg) {
            if (all.size() == 1) {
                return "package "+pkgName;
            } else {
                Type *typ = pkg->findType(all.at(1));
                if (typ) {
                    if (all.size() == 2) {
                        return QString("type %1 %2").arg(typ->name).arg(typeName(typ->typ));
                    } else {
                        Value *value = typ->findValue(all.at(2));
                        if (value) {
                            if (value->typ == TypeVarApi) {
                                return  QString("field %1 %2").arg(value->name).arg(value->exp);
                            } else if (value->typ == TypeMethodApi ){
                                return  QString("method %1%2").arg(value->name).arg(value->exp);
                            }
                        }
                    }
                } else {
                    Value *value = pkg->findValue(all.at(1));
                    if (value) {
                        if (value->typ == VarApi) {
                            return QString("var %1 %2").arg(value->name).arg(value->exp);
                        } else if (value->typ == ConstApi) {
                            return QString("const %1 %2").arg(value->name).arg(value->exp);
                        } else if (value->typ == FuncApi) {
                            return QString("func %1%2").arg(value->name).arg(value->exp);
                        }
                    }
                }
            }
        }
    }
    return QString();
}

QStringList GolangApi::findDocUrl(const QString &tag) const
{
    int pos = tag.lastIndexOf("/");
    QString pkgName = tag.left(pos+1);
    QStringList all = tag.mid(pos+1).split(".",QString::SkipEmptyParts);
    if (all.size() >= 1) {
        pkgName += all.at(0);
        Package *pkg = m_pkgs.findPackage(pkgName);
        if (pkg) {
            if (all.size() == 1) {
                return QStringList() << pkgName << pkgName;
            } else {
                Type *typ = pkg->findType(all.at(1));
                if (typ) {
                    if (all.size() == 2) {
                        return QStringList() << pkgName+"#"+typ->name << typ->name;
                    } else {
                        return findType(pkg,typ->name,all.at(2));
                    }
                } else {
                    Value *value = pkg->findValue(all.at(1));
                    if (value) {
                        if (value->typ == VarApi) {
                            return QStringList() << pkgName+"#variables" << value->name;
                        } else if (value->typ == ConstApi) {
                            if (value->exp.startsWith("ideal-") ||
                                    value->exp == "uint16") {
                                return QStringList() << pkgName+"#constants" << value->name;
                            } else {
                                return QStringList() << pkgName+"#"+value->exp << value->name;
                            }
                        } else if (value->typ == FuncApi) {
                            return QStringList() << pkgName+"#"+value->name << value->name;
                        }
                    }
                }
            }
        }
    }
    return QStringList();
}

PkgApiEnum GolangApi::findExp(const QString &tag, QString &exp) const
{
    QStringList all = tag.split(".");
    if (all.size() >= 1) {
        Package *pkg = m_pkgs.findPackage(all.at(0));
        if (pkg) {
            if (all.size() == 1) {
                exp = "package";
                return pkg->typ;
            } else {
                Type *typ = pkg->findType(all.at(1));
                if (typ) {
                    if (all.size() == 2) {
                        exp = typ->exp;
                        return typ->typ;
                    } else {
                        Value *value = typ->findValue(all.at(2));
                        if (value) {
                            exp = value->exp;
                            return value->typ;
                        }
                    }
                } else {
                    Value *value = pkg->findValue(all.at(1));
                    if (value) {
                        exp = value->exp;
                        return value->typ;
                    }
                }
            }
        }
    }
    return NullApi;
}

GolangApiThread::GolangApiThread(QObject *parent)
    :QThread(parent)
{
    m_api = new GolangApi(this);
}

GolangApiThread::~GolangApiThread()
{
    if (this->isRunning()) {
        this->terminate();
    }
}

void GolangApiThread::loadData(const QByteArray &data)
{
    m_data = data;
    m_file.clear();
    QThread::start();
}

void GolangApiThread::loadFile(const QString &fileName)
{
    m_file = fileName;
    QThread::start();
}

LiteApi::IGolangApi* GolangApiThread::api() const
{
    return m_api;
}

QStringList GolangApiThread::all() const
{
    return m_all;
}

QByteArray GolangApiThread::data() const
{
    return m_data;
}

void GolangApiThread::run()
{
    if (!m_file.isEmpty()) {
        QFile f(m_file);
        if (!f.open(QFile::ReadOnly)) {
            return;
        }
        QTextStream s(&f);
        if (m_api->loadStream(&s)) {
            m_all = m_api->all(LiteApi::AllGolangApi);
        }
    } else {
        QTextStream s(&m_data);
        if (m_api->loadStream(&s)) {
            m_all = m_api->all(LiteApi::AllGolangApi);
        }
    }
}
