/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2018 LiteIDE. All rights reserved.
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
// Module: gotools.h
// Creator: visualfc <visualfc@gmail.com>

#ifndef GOTOOLS_H
#define GOTOOLS_H

#include <QObject>
#include <QString>
#include <QLibrary>
#include <QProcessEnvironment>
#include "liteapi/liteapi.h"
#include "libgopher.h"

typedef void (*InvokeAsyncFunc)(TString* p0, TString* p1, TString* p2, TString* p3, void* p4, void* p5);
typedef int (*InvokeFunc)(GoString p0, GoString p1, GoString p2, GoString p3, GoString* p4, GoString* p5);
typedef void (*SetenvFunc)(TString *p0, TString *p1);
typedef void (*SetBuildEnvFunc)(TString* p0, TString* p1);
typedef void (*ClearBuildEnvFunc)();

inline bool isUseGopher(LiteApi::IApplication *app)
{
    return app->settings()->value("LiteApp/UseLibgopher",false).toBool();
}

class GopherLib : public QObject
{
    Q_OBJECT
public:
    GopherLib(const QString &libname, QObject *parent = 0);
    bool isValid() const {
        return  fnInvoke != 0 && fnInvokeAsync != 0 && fnSetenv != 0 && fnSetBuildEnv != 0 && fnClearBuildEnv != 0;
    }
    void setEnvironment(const  QProcessEnvironment &environment)
    {
        foreach (QString key, environment.keys()) {
            setenv(key,environment.value(key));
        }
    }
    void setenv(const QString &key, const QString &value)
    {
        QByteArray akey = key.toUtf8();
        QByteArray avalue = value.toUtf8();
        TString p0,p1;
        p0.size = akey.size();
        p0.data = akey.data();
        p1.size = avalue.size();
        p1.data = avalue.data();
        fnSetenv(&p0,&p1);
    }
    void setBuildEnv(const QString &key, const QString &value)
    {
        QByteArray akey = key.toUtf8();
        QByteArray avalue = value.toUtf8();
        TString p0,p1;
        p0.size = akey.size();
        p0.data = akey.data();
        p1.size = avalue.size();
        p1.data = avalue.data();
        fnSetBuildEnv(&p0,&p1);
    }
    void clearBuildEnv()
    {
        fnClearBuildEnv();
    }
    bool invokeArgs(const QStringList &args, const QString &sin, QString &sout, QString &serr)
    {
        return invoke(args[0],QStringList(args.mid(1)).join(" "),sin,sout,serr);
    }
    bool invokeArgsData(const QStringList &args, const QByteArray &ain, QString &sout, QString &serr)
    {
        return invoke(args[0],QStringList(args.mid(1)).join(" "),ain,sout,serr);
    }
    bool invokeData(const QString &id, const QString &args, const QByteArray &ain, QString &sout, QString &serr)
    {
        QByteArray aid = id.toUtf8();
        QByteArray aargs = args.toUtf8();
        GoString p0,p1,p2,p3,p4,p5;
        p0.n = aid.length();
        p0.p = aid.data();
        p1.n = aargs.length();
        p1.p = aargs.data();
        p2.n = 0;
        p2.p = 0;
        p3.n = ain.length();
        p3.p = ain.data();
        int r = fnInvoke(p0,p1,p2,p3,&p4,&p5);
        sout = QString::fromUtf8(p4.p,p4.n);
        serr = QString::fromUtf8(p5.p,p5.n);
        return r == 0;
    }
    bool invoke(const QString &id, const QString &args, const QString &sin, QString &sout, QString &serr)
    {
        return invokeData(id,args,sin.toUtf8(),sout,serr);
    }
    void invokeAsyncArgs(const QStringList &args, const QString &sin)
    {
        invokeAsync(args[0],QStringList(args.mid(1)).join(" "),sin);
    }
    void invokeAsyncArgsData(const QStringList &args, const QByteArray &ain)
    {
        invokeAsyncData(args[0],QStringList(args.mid(1)).join(" "),ain);
    }
    void invokeAsync(const QString &id, const QString &args, const QString &sin)
    {
        invokeAsyncData(id,args,sin.toUtf8());
    }
    void invokeAsyncData(const QString &id, const QString &args, const QByteArray &ain)
    {
        QByteArray aid = id.toUtf8();
        QByteArray aargs = args.toUtf8();
        TString p0,p1,p2,p3;
        p0.size = aid.length();
        p0.data = aid.data();
        p1.size = aargs.length();
        p1.data = aargs.data();
        p2.size = 0;
        p2.data = 0;
        p3.size = ain.length();
        p3.data = ain.data();
        fnInvokeAsync(&p0,&p1,&p2,&p3,(void*)this,(void*)(&GopherLib::s_write));
    }
    static int s_write(void *ctx, int flag, char *data, int size)
    {
        return ((GopherLib*)ctx)->write(flag,data,size);
    }
    virtual int write(int flag, const char *data, int size)
    {
        switch (flag) {
        case 0:
            emit finished(size == 0 ? 0 : 1, QString::fromUtf8(data,size));
            break;
        case 1:
            emit extOutput(QByteArray(data,size),false);
            break;
        case 2:
            emit extOutput(QByteArray(data,size),true);
            break;
        default:
            break;
        }
        return size;
    }
signals:
    void extOutput(const QByteArray &data, bool bError);
    void finished(int code, const QString &errMessage);
protected:
    QLibrary lib;
    SetenvFunc fnSetenv;
    SetBuildEnvFunc fnSetBuildEnv;
    ClearBuildEnvFunc fnClearBuildEnv;
    InvokeAsyncFunc fnInvokeAsync;
    InvokeFunc fnInvoke;
};

#endif //GOTOOLS_H
