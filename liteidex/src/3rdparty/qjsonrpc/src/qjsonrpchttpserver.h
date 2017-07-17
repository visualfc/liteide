#ifndef QJSONRPCHTTPSERVER_H
#define QJSONRPCHTTPSERVER_H

#include <QSslConfiguration>
#include <QTcpServer>
#include <QHash>

#include "qjsonrpcabstractserver.h"
#include "qjsonrpcglobal.h"

class QJsonRpcHttpServerPrivate;
class QJSONRPC_EXPORT QJsonRpcHttpServer : public QTcpServer,
                                           public QJsonRpcAbstractServer
{
    Q_OBJECT
public:
    QJsonRpcHttpServer(QObject *parent = 0);
    ~QJsonRpcHttpServer();

    QSslConfiguration sslConfiguration() const;
    void setSslConfiguration(const QSslConfiguration &config);

    virtual int connectedClientCount() const;

Q_SIGNALS:
    void clientConnected();
    void clientDisconnected();

public Q_SLOTS:
    virtual void notifyConnectedClients(const QJsonRpcMessage &message);
    virtual void notifyConnectedClients(const QString &method, const QJsonArray &params);

protected:
#if QT_VERSION >= 0x050000
    virtual void incomingConnection(qintptr socketDescriptor);
#else
    virtual void incomingConnection(int socketDescriptor);
#endif

private Q_SLOTS:
    void processIncomingMessage(const QJsonRpcMessage &message);

private:
    Q_DECLARE_PRIVATE(QJsonRpcHttpServer)
    Q_DISABLE_COPY(QJsonRpcHttpServer)
    QScopedPointer<QJsonRpcHttpServerPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_socketDisconnected())

};

#endif
