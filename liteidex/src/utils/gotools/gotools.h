#ifndef GOTOOLS_H
#define GOTOOLS_H

#include <QObject>
#include <QString>
#include <QLibrary>
#include "liteapi/liteapi.h"
#include "libgopher.h"

typedef void (*InvokeAsyncFunc)(GoString p0, GoString p1, GoString p2, GoString p3, void* p4, void* p5);
typedef int (*InvokeFunc)(GoString p0, GoString p1, GoString p2, GoString p3, GoString* p4, GoString* p5);
typedef void (*SetenvFunc)(char* p0, int p1, char* p2, int p3);

inline bool isUseGopher(LiteApi::IApplication *app)
{
    return app->settings()->value("LiteApp/UseLibgopher",false).toBool();
}

class GopherLib : public QObject
{
    Q_OBJECT
public:
    GopherLib(QObject *parent = 0);
    bool isValid() const {
        return  fnInvoke != 0 && fnInvokeAsync != 0 && fnSetenv != 0;
    }
    void setenv(const QString &key, const QString &value)
    {
        QByteArray p0 = key.toUtf8();
        QByteArray p1 = value.toUtf8();
        fnSetenv(p0.data(),p0.size(),p1.data(),p1.size());
    }
    bool invokeArgs(const QStringList &args, const QString &sin, QString &sout, QString &serr)
    {
        return invoke(args[0],QStringList(args.mid(1)).join(" "),sin,sout,serr);
    }
    bool invoke(const QString &id, const QString &args, const QString &sin, QString &sout, QString &serr)
    {
        QByteArray aid = id.toUtf8();
        QByteArray aargs = args.toUtf8();
        QByteArray ain = sin.toUtf8();
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
    void invokeAsyncArgs(const QStringList &args, const QString &sin)
    {
        invokeAsync(args[0],QStringList(args.mid(1)).join(" "),sin);
    }
    void invokeAsync(const QString &id, const QString &args, const QString &sin)
    {
        QByteArray aid = id.toUtf8();
        QByteArray aargs = args.toUtf8();
        QByteArray ain = sin.toUtf8();
        GoString p0,p1,p2,p3;
        p0.n = aid.length();
        p0.p = aid.data();
        p1.n = aargs.length();
        p1.p = aargs.data();
        p2.n = ain.length();
        p2.p = ain.data();
        p3.n = 0;
        p3.p = 0;
        fnInvokeAsync(p0,p1,p2,p3,(void*)this,(void*)(&GopherLib::s_write));
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
            emit extStdout(QString::fromUtf8(data,size));
            break;
        case 2:
            emit extStderr(QString::fromUtf8(data,size));
            break;
        default:
            break;
        }
        return size;
    }
signals:
    void extStdout(const QString &data);
    void extStderr(const QString &data);
    void finished(int code, const QString &errMessage);
protected:
    QLibrary lib;
    SetenvFunc fnSetenv;
    InvokeAsyncFunc fnInvokeAsync;
    InvokeFunc fnInvoke;
};

#endif //GOTOOLS_H
