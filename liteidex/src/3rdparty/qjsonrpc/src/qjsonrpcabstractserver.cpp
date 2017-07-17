#include "qjsonrpcsocket.h"
#include "qjsonrpcabstractserver_p.h"
#include "qjsonrpcabstractserver.h"

QJsonRpcAbstractServer::~QJsonRpcAbstractServer()
{
}

void QJsonRpcAbstractServerPrivate::_q_notifyConnectedClients(const QString &method,
                                                              const QJsonArray &params)
{
    QJsonRpcMessage notification =
        QJsonRpcMessage::createNotification(method, params);
    _q_notifyConnectedClients(notification);
}

void QJsonRpcAbstractServerPrivate::_q_notifyConnectedClients(const QJsonRpcMessage &message)
{
    for (int i = 0; i < clients.size(); ++i)
        clients[i]->notify(message);
}
