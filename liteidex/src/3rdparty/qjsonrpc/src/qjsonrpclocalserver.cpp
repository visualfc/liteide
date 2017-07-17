#include <QLocalServer>
#include <QLocalSocket>

#include "qjsonrpcsocket.h"
#include "qjsonrpcabstractserver_p.h"
#include "qjsonrpclocalserver.h"

class QJsonRpcLocalServerPrivate : public QJsonRpcAbstractServerPrivate
{
public:
    QHash<QLocalSocket*, QJsonRpcSocket*> socketLookup;

};

QJsonRpcLocalServer::QJsonRpcLocalServer(QObject *parent)
#if defined(USE_QT_PRIVATE_HEADERS)
    : QLocalServer(*new QJsonRpcLocalServerPrivate, parent)
#else
    : QLocalServer(parent),
      d_ptr(new QJsonRpcLocalServerPrivate)
#endif
{
}

QJsonRpcLocalServer::~QJsonRpcLocalServer()
{
    Q_D(QJsonRpcLocalServer);
    foreach (QLocalSocket *socket, d->socketLookup.keys()) {
        socket->flush();
        socket->deleteLater();
    }
    d->socketLookup.clear();

    foreach (QJsonRpcSocket *client, d->clients)
        client->deleteLater();
    d->clients.clear();
}

int QJsonRpcLocalServer::connectedClientCount() const
{
    Q_D(const QJsonRpcLocalServer);
    return d->clients.size();
}

bool QJsonRpcLocalServer::addService(QJsonRpcService *service)
{
    if (!QJsonRpcServiceProvider::addService(service))
        return false;

    connect(service, SIGNAL(notifyConnectedClients(QJsonRpcMessage)),
               this, SLOT(notifyConnectedClients(QJsonRpcMessage)));
    connect(service, SIGNAL(notifyConnectedClients(QString,QJsonArray)),
               this, SLOT(notifyConnectedClients(QString,QJsonArray)));
    return true;
}

bool QJsonRpcLocalServer::removeService(QJsonRpcService *service)
{
    if (!QJsonRpcServiceProvider::removeService(service))
        return false;

    disconnect(service, SIGNAL(notifyConnectedClients(QJsonRpcMessage)),
                  this, SLOT(notifyConnectedClients(QJsonRpcMessage)));
    disconnect(service, SIGNAL(notifyConnectedClients(QString,QJsonArray)),
                  this, SLOT(notifyConnectedClients(QString,QJsonArray)));
    return true;
}

void QJsonRpcLocalServer::incomingConnection(quintptr socketDescriptor)
{
    Q_D(QJsonRpcLocalServer);
    QLocalSocket *localSocket = new QLocalSocket(this);
    if (!localSocket->setSocketDescriptor(socketDescriptor)) {
        qJsonRpcDebug() << Q_FUNC_INFO << "nextPendingConnection is null";
        localSocket->deleteLater();
        return;
    }

    QIODevice *device = qobject_cast<QIODevice*>(localSocket);
    QJsonRpcSocket *socket = new QJsonRpcSocket(device, this);
    connect(socket, SIGNAL(messageReceived(QJsonRpcMessage)),
              this, SLOT(_q_processMessage(QJsonRpcMessage)));
    d->clients.append(socket);
    connect(localSocket, SIGNAL(disconnected()), this, SLOT(_q_clientDisconnected()));
    d->socketLookup.insert(localSocket, socket);
    Q_EMIT clientConnected();
}

void QJsonRpcLocalServer::_q_clientDisconnected()
{
    Q_D(QJsonRpcLocalServer);
    QLocalSocket *localSocket = static_cast<QLocalSocket*>(sender());
    if (!localSocket) {
        qJsonRpcDebug() << Q_FUNC_INFO << "called with invalid socket";
        return;
    }

    if (d->socketLookup.contains(localSocket)) {
        QJsonRpcSocket *socket = d->socketLookup.take(localSocket);
        d->clients.removeAll(socket);
        socket->deleteLater();
    }

    localSocket->deleteLater();
    Q_EMIT clientDisconnected();
}

void QJsonRpcLocalServer::_q_processMessage(const QJsonRpcMessage &message)
{
    QJsonRpcSocket *socket = static_cast<QJsonRpcSocket*>(sender());
    if (!socket) {
        qJsonRpcDebug() << Q_FUNC_INFO << "called without service socket";
        return;
    }

    processMessage(socket, message);
}

void QJsonRpcLocalServer::notifyConnectedClients(const QJsonRpcMessage &message)
{
    Q_D(QJsonRpcLocalServer);
    d->_q_notifyConnectedClients(message);
}

void QJsonRpcLocalServer::notifyConnectedClients(const QString &method, const QJsonArray &params)
{
    Q_D(QJsonRpcLocalServer);
    d->_q_notifyConnectedClients(method, params);
}

#include "moc_qjsonrpclocalserver.cpp"
