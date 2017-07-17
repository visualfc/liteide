#include <QStringList>
#include <QDateTime>

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#else
#include "json/qjsondocument.h"
#endif

#include "qjsonrpcsocket.h"
#include "qjsonrpcmessage.h"
#include "qjsonrpchttpserver_p.h"
#include "qjsonrpchttpserver.h"

QJsonRpcHttpServerSocket::QJsonRpcHttpServerSocket(QObject *parent)
    : QSslSocket(parent),
      m_requestParser(0)
{
    // initialize request parser
    m_requestParser = (http_parser*)malloc(sizeof(http_parser));
    http_parser_init(m_requestParser, HTTP_REQUEST);
    m_requestParserSettings.on_message_begin = onMessageBegin;
    m_requestParserSettings.on_url = onUrl;
    m_requestParserSettings.on_header_field = onHeaderField;
    m_requestParserSettings.on_header_value = onHeaderValue;
    m_requestParserSettings.on_headers_complete = onHeadersComplete;
    m_requestParserSettings.on_body = onBody;
    m_requestParserSettings.on_message_complete = onMessageComplete;
    m_requestParser->data = this;

    connect(this, SIGNAL(readyRead()), this, SLOT(readIncomingData()));
}

QJsonRpcHttpServerSocket::~QJsonRpcHttpServerSocket()
{
    free(m_requestParser);
}

static inline QByteArray statusMessageForCode(int code)
{
    switch (code) {
    case 200:
        return "OK";
    case 400:
        return "Bad Request";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 500:
        return "Internal Server Error";
    }

    return QByteArray();
}

qint64 QJsonRpcHttpServerSocket::writeData(const char *data, qint64 maxSize)
{
    m_responseBuffer.append(data, (int)maxSize);
    QJsonDocument document = QJsonDocument::fromJson(m_responseBuffer);
    if (document.isObject()) {
        // determine the HTTP code to respond with
        int statusCode = 200;
        QJsonRpcMessage message = QJsonRpcMessage::fromObject(document.object());
        switch (message.type()) {
        case QJsonRpcMessage::Error:
            switch (message.errorCode()) {
            case QJsonRpc::InvalidRequest:
                statusCode = 400;
                break;

            case QJsonRpc::MethodNotFound:
                statusCode = 404;
                break;

            default:
                statusCode = 500;
                break;
            }
            break;

        case QJsonRpcMessage::Invalid:
            statusCode = 400;
            break;

        case QJsonRpcMessage::Notification:
        case QJsonRpcMessage::Response:
        case QJsonRpcMessage::Request:
            statusCode = 200;
            break;
        }

        // header
        QByteArray responseHeader;
        responseHeader += "HTTP/1.1 " + QByteArray::number(statusCode) +" " + statusMessageForCode(statusCode) + "\r\n";

        if(m_requestHeaders.contains("origin")) {
          QString origin = m_requestHeaders["origin"];
          responseHeader += "Access-Control-Allow-Origin: " + origin.toUtf8() + "\r\n";
        }

        responseHeader += "Content-Type: application/json-rpc\r\n";
        responseHeader += "Content-Length: " + QByteArray::number(m_responseBuffer.size()) + "\r\n";
        responseHeader += "\r\n";

        // body
        m_responseBuffer.prepend(responseHeader);
        qint64 bytesWritten = QSslSocket::writeData(m_responseBuffer.constData(), m_responseBuffer.size());
        close();

        // then clear the buffer
        m_responseBuffer.clear();
        return bytesWritten;
    }

    return maxSize;
}

void QJsonRpcHttpServerSocket::sendOptionsResponse(int statusCode)
{
    QByteArray responseHeader;

    responseHeader += "HTTP/1.1 " + QByteArray::number(statusCode) +" " + statusMessageForCode(statusCode) + "\r\n";
    
    if(m_requestHeaders.contains("origin")) {
      QByteArray origin = m_requestHeaders["origin"].toLatin1();
      responseHeader += "Access-Control-Allow-Origin: " + origin + "\r\n";
    }

    if(m_requestHeaders.contains("access-control-request-method")) {
      QByteArray allowed_method = m_requestHeaders["access-control-request-method"].toLatin1();
      responseHeader += "Access-Control-Allow-Methods: " + allowed_method + "\r\n";
    }

    if(m_requestHeaders.contains("access-control-request-headers")) {
      QByteArray allowed_headers = m_requestHeaders["access-control-request-headers"].toLatin1();
      responseHeader += "Access-Control-Allow-Headers: " + allowed_headers + "\r\n";
    }

    responseHeader += "Content-Type: text/plain\r\n";
    responseHeader += "Connection: keep-alive\r\n";
    responseHeader += "\r\n";

    QSslSocket::writeData(responseHeader.constData(), responseHeader.size());
    close();
}

void QJsonRpcHttpServerSocket::sendErrorResponse(int statusCode)
{
    QByteArray responseHeader;
    responseHeader += "HTTP/1.1 " + QByteArray::number(statusCode) +" " + statusMessageForCode(statusCode) + "\r\n";
    responseHeader += "\r\n";

    QSslSocket::writeData(responseHeader.constData(), responseHeader.size());
    close();
}

void QJsonRpcHttpServerSocket::readIncomingData()
{
    QByteArray requestBuffer = readAll();
    http_parser_execute(m_requestParser, &m_requestParserSettings,
                        requestBuffer.constData(), requestBuffer.size());
}

int QJsonRpcHttpServerSocket::onBody(http_parser *parser, const char *at, size_t length)
{
    QJsonRpcHttpServerSocket *request = (QJsonRpcHttpServerSocket *)parser->data;
    request->m_requestPayload = QByteArray(at, length);
    return 0;
}

int QJsonRpcHttpServerSocket::onMessageComplete(http_parser *parser)
{
    QJsonRpcHttpServerSocket *request = (QJsonRpcHttpServerSocket *)parser->data;
    QJsonRpcMessage message = QJsonRpcMessage::fromJson(request->m_requestPayload);
    Q_EMIT request->messageReceived(message);
    return 0;
}

int QJsonRpcHttpServerSocket::onHeadersComplete(http_parser *parser)
{
    QJsonRpcHttpServerSocket *request = (QJsonRpcHttpServerSocket *)parser->data;

    if (parser->method == HTTP_OPTIONS) {
        qJsonRpcDebug() << Q_FUNC_INFO << "OPTIONS method" << parser->method;
        request->sendOptionsResponse(200);
        return 0;
    }

    // need to add the final headers received
    if (!request->m_currentHeaderField.isEmpty() && !request->m_currentHeaderValue.isEmpty()) {
        request->m_requestHeaders.insert(request->m_currentHeaderField.toLower(), request->m_currentHeaderValue);
        request->m_currentHeaderField.clear();
        request->m_currentHeaderValue.clear();
    }

    if (parser->method != HTTP_GET && parser->method != HTTP_POST) {
        // NOTE: close the socket, cleanup, delete, etc..
        qJsonRpcDebug() << Q_FUNC_INFO << "invalid method: " << parser->method;
        request->sendErrorResponse(405);
        return -1;
    }

    // check headers
    // see: http://www.jsonrpc.org/historical/json-rpc-over-http.html#http-header
    QStringList requiredHeaders =
        QStringList() << "content-type" << "content-length" << "accept";
    foreach (QString requiredHeader, requiredHeaders) {
        if (!request->m_requestHeaders.contains(requiredHeader)) {
            qJsonRpcDebug() << Q_FUNC_INFO << "error: " << request->m_requestHeaders;
            request->sendErrorResponse(400);
            return -1;
        }
    }

    QStringList supportedContentTypes =
        QStringList() << "application/json-rpc" << "application/json" << "application/jsonrequest";
    QString contentType = request->m_requestHeaders.value("content-type");
    bool foundSupportedContentType = false;
    foreach (QString supportedContentType, supportedContentTypes) {
        if (contentType.contains(supportedContentType)) {
            foundSupportedContentType = true;
            break;
        }
    }

    QString acceptType = request->m_requestHeaders.value("accept");
    if (!foundSupportedContentType || !supportedContentTypes.contains(acceptType)) {
        // NOTE: signal the error
        qJsonRpcDebug() << Q_FUNC_INFO << "invalid content or accept type";
        request->sendErrorResponse(400);
        return -1;
    }

    return 0;
}

int QJsonRpcHttpServerSocket::onHeaderField(http_parser *parser, const char *at, size_t length)
{
    QJsonRpcHttpServerSocket *request = (QJsonRpcHttpServerSocket *)parser->data;
    if (!request->m_currentHeaderField.isEmpty() && !request->m_currentHeaderValue.isEmpty()) {
        request->m_requestHeaders.insert(request->m_currentHeaderField.toLower(), request->m_currentHeaderValue);
        request->m_currentHeaderField.clear();
        request->m_currentHeaderValue.clear();
    }

    request->m_currentHeaderField.append(QString::fromUtf8(at, length));
    return 0;
}

int QJsonRpcHttpServerSocket::onHeaderValue(http_parser *parser, const char *at, size_t length)
{
    QJsonRpcHttpServerSocket *request = (QJsonRpcHttpServerSocket *)parser->data;
    request->m_currentHeaderValue.append(QString::fromUtf8(at, length));
    return 0;
}

int QJsonRpcHttpServerSocket::onMessageBegin(http_parser *parser)
{
    QJsonRpcHttpServerSocket *request = (QJsonRpcHttpServerSocket *)parser->data;
    request->m_requestHeaders.clear();
    return 0;
}

int QJsonRpcHttpServerSocket::onUrl(http_parser *parser, const char *at, size_t length)
{
    Q_UNUSED(parser)
    Q_UNUSED(at)
    Q_UNUSED(length)
//    QString url = QString::fromLatin1(at, length);
//    qDebug() << "requested url: " << url;

    return 0;
}

QJsonRpcHttpServerRpcSocket::QJsonRpcHttpServerRpcSocket(QIODevice *device, QObject *parent)
    : QJsonRpcSocket(device, parent)
{
    disconnect(device, SIGNAL(readyRead()), this, SLOT(_q_processIncomingData()));
}

QJsonRpcHttpServer::QJsonRpcHttpServer(QObject *parent)
    : QTcpServer(parent),
      d_ptr(new QJsonRpcHttpServerPrivate(this))
{
}

QJsonRpcHttpServer::~QJsonRpcHttpServer()
{
}

QSslConfiguration QJsonRpcHttpServer::sslConfiguration() const
{
    Q_D(const QJsonRpcHttpServer);
    return d->sslConfiguration;
}

void QJsonRpcHttpServer::setSslConfiguration(const QSslConfiguration &config)
{
    Q_D(QJsonRpcHttpServer);
    d->sslConfiguration = config;
}

#if QT_VERSION >= 0x050000
void QJsonRpcHttpServer::incomingConnection(qintptr socketDescriptor)
#else
void QJsonRpcHttpServer::incomingConnection(int socketDescriptor)
#endif
{
    Q_D(QJsonRpcHttpServer);
    QJsonRpcHttpServerSocket *socket = new QJsonRpcHttpServerSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qJsonRpcDebug() << Q_FUNC_INFO << "unable to set socket descriptor";
        socket->deleteLater();
        return;
    }

    if (!d->sslConfiguration.isNull()) {
        socket->setSslConfiguration(d->sslConfiguration);
        socket->startServerEncryption();
        // connect ssl error signals etc

        // NOTE: unsafe
        connect(socket, SIGNAL(sslErrors(QList<QSslError>)), socket, SLOT(ignoreSslErrors()));
    }

    connect(socket, SIGNAL(disconnected()), this, SLOT(_q_socketDisconnected()));
    connect(socket, SIGNAL(messageReceived(QJsonRpcMessage)),
              this, SLOT(processIncomingMessage(QJsonRpcMessage)));
    QJsonRpcHttpServerRpcSocket *rpcSocket = new QJsonRpcHttpServerRpcSocket(socket, this);
    d->requestSocketLookup.insert(socket, rpcSocket);
}

void QJsonRpcHttpServer::processIncomingMessage(const QJsonRpcMessage &message)
{
    Q_D(QJsonRpcHttpServer);
    QJsonRpcHttpServerSocket *request = qobject_cast<QJsonRpcHttpServerSocket*>(sender());
    if (!request)
        return;

    QJsonRpcSocket *socket = d->requestSocketLookup.value(request);
    processMessage(socket, message);
}

int QJsonRpcHttpServer::connectedClientCount() const
{
    return 0;
}

void QJsonRpcHttpServer::notifyConnectedClients(const QJsonRpcMessage &message)
{
    Q_UNUSED(message);
}

void QJsonRpcHttpServer::notifyConnectedClients(const QString &method, const QJsonArray &params)
{
    Q_UNUSED(method);
    Q_UNUSED(params);
}

void QJsonRpcHttpServerPrivate::_q_socketDisconnected()
{
    Q_Q(QJsonRpcHttpServer);
    QJsonRpcHttpServerSocket *socket = qobject_cast<QJsonRpcHttpServerSocket*>(q->sender());
    if (!socket)
        return;

    QJsonRpcSocket *rpcSocket = requestSocketLookup.take(socket);
    rpcSocket->deleteLater();
    socket->deleteLater();
}

#include "moc_qjsonrpchttpserver.cpp"
