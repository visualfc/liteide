#ifndef DLVCLIENT_H
#define DLVCLIENT_H

#include <QTcpSocket>
#include <QDateTime>
#include "qjsonrpc/src/qjsonrpcsocket.h"
#include "dlvtypes.h"

class DlvClient : public QObject
{
public:
    DlvClient(QObject *parent);
    bool Connect(const QString &service);
    int ProcessPid() const;
    QDateTime LastModified() const;
    QStringList ListTypes(const QString &filter) const;
    QList<Variable> ListPackageVariables(const QString &filter, const LoadConfig &cfg) const;
    QList<Variable> ListLocalVariables(const EvalScope &scope, const LoadConfig &cfg) const;
    void callMethod(bool notification, const QString &method);
    void callCommand(const QString &cmd);
protected:
    bool call(const QString &method, const QObject *in, QObject *out, int timeout = 5000) const;
    bool call(const QString &method, const JsonDataIn *in, JsonDataOut *out, int timeout = 5000) const;
protected:
    QJsonRpcSocket *m_dlv;
};

#endif // DLVCLIENT_H
