#include "dlvclient.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QUrl>
#include <QDebug>
#include "qjsonrpc/src/json/qjson_export.h"
#include "qjsonrpc/src/json/qjsondocument.h"

DlvClient::DlvClient(QObject *parent) : QObject(parent)
{

}

bool DlvClient::Connect(const QString &service)
{
    QUrl serviceUrl = QUrl::fromUserInput(service);
    QHostAddress serviceAddress(serviceUrl.host());
    QTcpSocket *tcpSocket = new QTcpSocket(this);
    int servicePort = serviceUrl.port() ? serviceUrl.port() : 5555;
    tcpSocket->connectToHost(serviceAddress, servicePort);
    if (!tcpSocket->waitForConnected(5000)) {
        qDebug("could not connect to host at %s:%d", serviceUrl.host().toLocal8Bit().data(),
               servicePort);
        return false;
    }
    m_dlv = new QJsonRpcSocket(tcpSocket,this);
    return true;
}

int DlvClient::ProcessPid() const
{
    ProcessPidIn in;
    ProcessPidOut out;
    call("ProcessPid",&in,&out);
    return out.Pid;
}

QDateTime DlvClient::LastModified() const
{
    LastModifiedIn in;
    LastModifiedOut out;
    call("LastModified",&in,&out);
    return out.Time;
}

QStringList DlvClient::ListTypes(const QString &filter) const
{
    ListTypesIn in;
    in.Filter = filter;
    ListTypesOut out;
    call("ListTypes",&in,&out);
    return out.Types;
}

QList<Variable> DlvClient::ListPackageVariables(const QString &filter, const LoadConfig &cfg) const
{
    ListPackageVarsIn in;
    in.Filter = filter;
    in.Cfg = cfg;
    ListPackageVarsOut out;
    call("ListPackageVars",&in,&out);
    return out.Variables;
}

QList<Variable> DlvClient::ListLocalVariables(const EvalScope &scope, const LoadConfig &cfg) const
{
    ListLocalVarsIn in;
    in.Scope = scope;
    in.Cfg = cfg;
    ListLocalVarsOut out;
    call("ListLocalVars",&in,&out);
    return out.Variables;
}

void DlvClient::callMethod(bool notification, const QString &method)
{
    QVariantList arguments;

    QJsonRpcMessage request = notification ?
        QJsonRpcMessage::createNotification(method, QJsonArray::fromVariantList(arguments)) :
        QJsonRpcMessage::createRequest(method, QJsonArray::fromVariantList(arguments));
    qDebug() << request.toJson();

    QJsonRpcMessage response = m_dlv->sendMessageBlocking(request, 5000);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());
        return ;
    }

    qDebug() << response.result();
    ProcessPidOut out;
    qDebug() << response.result().toVariant().toMap();
    //VariantHelper::VarintToQObject(response.result().toVariant(),&out);
    //qDebug() << out.pid();
}

void DlvClient::callCommand(const QString &cmd)
{
    QVariantMap m;
    m.insert("name",cmd);
    QVariantList arguments;
    arguments << m;

    bool notification = false;

    QString method = "RPCServer.Command";

    QJsonRpcMessage request = notification ?
        QJsonRpcMessage::createNotification(method, QJsonArray::fromVariantList(arguments)) :
        QJsonRpcMessage::createRequest(method, QJsonArray::fromVariantList(arguments));
    qDebug() << request.toJson();

    QJsonRpcMessage response = m_dlv->sendMessageBlocking(request, 5000);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());
        return ;
    }

    qDebug() << response.result();
}

bool DlvClient::call(const QString &method, const QObject *in, QObject *out, int timeout) const
{
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("RPCServer."+method, QJsonValue::fromVariant(VariantHelper::ObjectToVariant(in)));

    qDebug() << request.toJson();

    QJsonRpcMessage response = m_dlv->sendMessageBlocking(request, timeout);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());
        return false;
    }
    VariantHelper::VarintToQObject(response.result().toVariant(),out);
    return true;
}

bool DlvClient::call(const QString &method, const JsonDataIn *in, JsonDataOut *out, int timeout) const
{
    QVariantMap param;
    in->toMap(param);
    QJsonRpcMessage request = QJsonRpcMessage::createRequest("RPCServer."+method, QJsonValue::fromVariant(param));

    qDebug() << QJsonDocument(request.toObject()).toJson(QJsonDocument::Compact);

    QJsonRpcMessage response = m_dlv->sendMessageBlocking(request, timeout);
    if (response.type() == QJsonRpcMessage::Error) {
        qDebug("error(%d): %s", response.errorCode(), response.errorMessage().toLocal8Bit().data());
        return false;
    }
    out->fromMap(response.result().toVariant().toMap());
    return true;
    //VariantHelper::VarintToQObject(response.result().toVariant(),out);
}
