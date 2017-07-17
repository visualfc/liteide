#include <QtTest>
#include <QPointer>
#include <QTcpSocket>

#include "http_parser.h"
#include "testhttpserver.h"

class TestHttpServerPrivate
{
public:
    TestHttpServerPrivate();
    virtual ~TestHttpServerPrivate();

    // http parsing
    http_parser *httpParser;
    http_parser_settings httpParserSettings;
    QByteArray currentHeaderField;
    QByteArray currentHeaderValue;

    static int onMessageBegin(http_parser *parser);
    static int onUrl(http_parser *parser, const char *at, size_t length);
    static int onHeaderField(http_parser *parser, const char *at, size_t length);
    static int onHeaderValue(http_parser *parser, const char *at, size_t length);
    static int onHeadersComplete(http_parser *parser);
    static int onBody(http_parser *parser, const char *at, size_t length);
    static int onMessageComplete(http_parser *parser);

    // private slots
    void _q_socketReadyRead();
    void _q_socketError(QAbstractSocket::SocketError error);
    void _q_handleRequest();

    TestHttpServerRequestHandler *requestHandler;
    QPointer<QTcpSocket> socket;

    QNetworkAccessManager::Operation operation;
    QNetworkRequest request;
    QByteArray body;
};

TestHttpServerPrivate::TestHttpServerPrivate()
    : httpParser(0),
      requestHandler(0)
{
    // initialize request parser
    httpParser = (http_parser*)malloc(sizeof(http_parser));
    http_parser_init(httpParser, HTTP_REQUEST);
    httpParserSettings.on_message_begin = onMessageBegin;
    httpParserSettings.on_url = onUrl;
    httpParserSettings.on_header_field = onHeaderField;
    httpParserSettings.on_header_value = onHeaderValue;
    httpParserSettings.on_headers_complete = onHeadersComplete;
    httpParserSettings.on_body = onBody;
    httpParserSettings.on_message_complete = onMessageComplete;
    httpParser->data = this;
}

TestHttpServerPrivate::~TestHttpServerPrivate()
{
    free(httpParser);
}

int TestHttpServerPrivate::onMessageBegin(http_parser *parser)
{
    TestHttpServerPrivate *priv = (TestHttpServerPrivate *)parser->data;
    priv->request = QNetworkRequest();
    priv->currentHeaderField.clear();
    priv->currentHeaderValue.clear();
    priv->body.clear();
    return 0;
}

int TestHttpServerPrivate::onUrl(http_parser *parser, const char *at, size_t length)
{
    TestHttpServerPrivate *priv = (TestHttpServerPrivate *)parser->data;
    QUrl requestUrl = priv->request.url();
    requestUrl.setPath(QByteArray(at, length));
    priv->request.setUrl(requestUrl);
    return 0;
}

int TestHttpServerPrivate::onHeaderField(http_parser *parser, const char *at, size_t length)
{
    TestHttpServerPrivate *priv = (TestHttpServerPrivate *)parser->data;
    if (!priv->currentHeaderField.isEmpty() && !priv->currentHeaderValue.isEmpty()) {
        priv->request.setRawHeader(priv->currentHeaderField, priv->currentHeaderValue);
        priv->currentHeaderField.clear();
        priv->currentHeaderValue.clear();
    }

    priv->currentHeaderField.append(at, length);
    return 0;
}

int TestHttpServerPrivate::onHeaderValue(http_parser *parser, const char *at, size_t length)
{
    TestHttpServerPrivate *priv = (TestHttpServerPrivate *)parser->data;
    priv->currentHeaderValue.append(at, length);
    return 0;
}

int TestHttpServerPrivate::onHeadersComplete(http_parser *parser)
{
    Q_UNUSED(parser);
    return 0;
}

int TestHttpServerPrivate::onBody(http_parser *parser, const char *at, size_t length)
{
    TestHttpServerPrivate *priv = (TestHttpServerPrivate *)parser->data;
    priv->body.append(at, length);
    return 0;
}

int TestHttpServerPrivate::onMessageComplete(http_parser *parser)
{
    TestHttpServerPrivate *priv = (TestHttpServerPrivate *)parser->data;
    switch (parser->method) {
    case HTTP_HEAD:
        priv->operation = QNetworkAccessManager::HeadOperation;
        break;
    case HTTP_GET:
        priv->operation = QNetworkAccessManager::GetOperation;
        break;
    case HTTP_PUT:
        priv->operation = QNetworkAccessManager::PutOperation;
        break;
    case HTTP_POST:
        priv->operation = QNetworkAccessManager::PostOperation;
        break;
    case HTTP_DELETE:
        priv->operation = QNetworkAccessManager::DeleteOperation;
        break;
    default:
        priv->operation = QNetworkAccessManager::CustomOperation;
        break;
    }

    priv->_q_handleRequest();
    return 0;
}

void TestHttpServerPrivate::_q_socketReadyRead()
{
    Q_ASSERT(httpParser);
    Q_ASSERT(!socket.isNull());
    QByteArray buffer = socket->readAll();
    http_parser_execute(httpParser, &httpParserSettings, buffer.constData(), buffer.size());
}

void TestHttpServerPrivate::_q_socketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    Q_ASSERT(!socket.isNull());
    socket->close();
    socket->deleteLater();
}

void TestHttpServerPrivate::_q_handleRequest()
{
    if (!requestHandler) {
        qDebug() << Q_FUNC_INFO << "no request handler installed";
        return;
    }

    QByteArray response = requestHandler->handleRequest(operation, request, body);
    QCOMPARE(socket->write(response), (qint64)response.size());
}

class TestHttpServerSimpleRequestHandler : public TestHttpServerRequestHandler
{
public:
    explicit TestHttpServerSimpleRequestHandler(const QByteArray &d)
        : data(d)
    {
    }

    virtual QByteArray handleRequest(QNetworkAccessManager::Operation operation,
                                     const QNetworkRequest &request, const QByteArray &body)
    {
        Q_UNUSED(operation);
        Q_UNUSED(request);
        Q_UNUSED(body);
        return data;
    }

private:
    QByteArray data;

};

TestHttpServer::TestHttpServer(QObject *parent)
    : QTcpServer(parent),
      d_ptr(new TestHttpServerPrivate)
{
}

TestHttpServer::TestHttpServer(TestHttpServerRequestHandler *requestHandler, QObject *parent)
    : QTcpServer(parent),
      d_ptr(new TestHttpServerPrivate)
{
    d_func()->requestHandler = requestHandler;
}

TestHttpServer::~TestHttpServer()
{
    Q_D(TestHttpServer);
    if (d->requestHandler) {
        delete d->requestHandler;
        d->requestHandler = 0;
    }
}

TestHttpServerRequestHandler *TestHttpServer::requestHandler() const
{
    Q_D(const TestHttpServer);
    return d->requestHandler;
}

void TestHttpServer::setRequestHandler(TestHttpServerRequestHandler *requestHandler)
{
    Q_D(TestHttpServer);
    if (d->requestHandler) {
        delete d->requestHandler;
        d->requestHandler = 0;
    }

    d->requestHandler = requestHandler;
}

void TestHttpServer::setResponseData(const QByteArray &data)
{
    setRequestHandler(new TestHttpServerSimpleRequestHandler(data));
}

#if QT_VERSION >= 0x050000
void TestHttpServer::incomingConnection(qintptr socketDescriptor)
#else
void TestHttpServer::incomingConnection(int socketDescriptor)
#endif
{
    Q_D(TestHttpServer);
    d->socket = new QTcpSocket(this);
    QVERIFY(d->socket->setSocketDescriptor(socketDescriptor));
    connect(d->socket.data(), SIGNAL(readyRead()), this, SLOT(_q_socketReadyRead()));
    connect(d->socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
                        this, SLOT(_q_socketError(QAbstractSocket::SocketError)));
}

#include "moc_testhttpserver.cpp"
