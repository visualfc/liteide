#include <QtCore/QEventLoop>
#include <QtCore/QVariant>
#include <QtTest/QtTest>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#else
#include "json/qjsondocument.h"
#endif

#include "qjsonrpchttpclient.h"
#include "qjsonrpcservice.h"
#include "qjsonrpchttpserver.h"
#include "qjsonrpcmessage.h"

class TestQJsonRpcHttpServer: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void quickTest();
    void statusCodes_data();
    void statusCodes();
    void invalidMethod_data();
    void invalidMethod();
    void missingHeaders_data();
    void missingHeaders();
    void testAccessControlHeader();
    void testMissingAccessControlHeader();

private:
    // temporarily disabled
    void sslTest();

private:
    QSslConfiguration serverSslConfiguration;
    QSslConfiguration clientSslConfiguration;

};

Q_DECLARE_METATYPE(QNetworkAccessManager::Operation)

class TestService : public QJsonRpcService
{
    Q_OBJECT
    Q_CLASSINFO("serviceName", "service")
public:
    TestService(QObject *parent = 0)
        : QJsonRpcService(parent),
          m_called(0)
    {}

    void resetCount() { m_called = 0; }
    int callCount() const {
        return m_called;
    }

public Q_SLOTS:
    void noParam() const {}
    QString singleParam(const QString &string) const { return string; }
    QString multipleParam(const QString &first,
                          const QString &second,
                          const QString &third) const
    {
        return first + second + third;
    }

    void numberParameters(int intParam, double doubleParam, float floatParam)
    {
        Q_UNUSED(intParam)
        Q_UNUSED(doubleParam)
        Q_UNUSED(floatParam)
    }

    bool variantParameter(const QVariant &variantParam) const
    {
        return variantParam.toBool();
    }

    QVariant variantStringResult() {
        return "hello";
    }

    QVariantList variantListResult() {
        return QVariantList() << "one" << 2 << 3.0;
    }

    QVariantMap variantMapResult() {
        QVariantMap result;
        result["one"] = 1;
        result["two"] = 2.0;
        return result;
    }

    void increaseCalled() {
        m_called++;
    }

private:
    int m_called;
};

void TestQJsonRpcHttpServer::initTestCase()
{
    // setup ssl configuration for tests
    QList<QSslCertificate> caCerts =
        QSslCertificate::fromPath(QLatin1String(":/certs/qt-test-server-cacert.pem"));
    serverSslConfiguration.setCaCertificates(caCerts);
    serverSslConfiguration.setProtocol(QSsl::AnyProtocol);
}

void TestQJsonRpcHttpServer::quickTest()
{
    QJsonRpcHttpServer server;
    server.addService(new TestService);
    QVERIFY(server.listen(QHostAddress::LocalHost, 8118));

    QJsonRpcHttpClient client;
    client.setEndPoint("http://127.0.0.1:8118");

    QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.noParam");
    QJsonRpcMessage response = client.sendMessageBlocking(request);
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE(request.id(), response.id());
}

void TestQJsonRpcHttpServer::sslTest()
{
    QJsonRpcHttpServer server;
    server.setSslConfiguration(serverSslConfiguration);
    server.addService(new TestService);
    server.listen(QHostAddress::LocalHost, 8118);

    QJsonRpcHttpClient client;
    client.setEndPoint("http://127.0.0.1:8118");
    client.setSslConfiguration(serverSslConfiguration);

    QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.noParam");
    QJsonRpcMessage response = client.sendMessageBlocking(request);
    qDebug() << response;
    QVERIFY(response.type() != QJsonRpcMessage::Error);
    QCOMPARE(request.id(), response.id());
}

void TestQJsonRpcHttpServer::statusCodes_data()
{
    QTest::addColumn<QByteArray>("body");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("statusReason");
    QTest::addColumn<QByteArray>("contentType");

    {
        QJsonRpcMessage invalidMethod = QJsonRpcMessage::createRequest("invalidMethod");
        QTest::newRow("404-not-found") << invalidMethod.toJson() << 404
                                       << QByteArray("Not Found") << QByteArray("application/json");
    }

    {
        QTest::newRow("400-bad-request") << QByteArray("{\"jsonrpc\": \"2.0\", \"id\": 666}")
                                         << 400 << QByteArray("Bad Request") << QByteArray("application/json");
    }

    {
        QJsonRpcMessage invalidParameters =
            QJsonRpcMessage::createRequest("service.numberParameters", false);
        QTest::newRow("500-internal-server-error") << invalidParameters.toJson()
                                                   << 500 << QByteArray("Internal Server Error")
                                                   << QByteArray("application/json");
    }

    {
        QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.noParam");
        QTest::newRow("200-ok") << request.toJson() << 200
                                << QByteArray("OK") << QByteArray("application/json");
    }

    {
        QJsonRpcMessage request = QJsonRpcMessage::createRequest("service.noParam");
        QTest::newRow("200-composite-content-type") << request.toJson() << 200
                                << QByteArray("OK") << QByteArray("application/json;charset=UTF-8");
    }

    /*
     * TODO: support notifications
    {
        QJsonRpcMessage notification = QJsonRpcMessage::createNotification("service.noParam");
        QTest::newRow("204-no-content") << notification.toJson() << 204
                               << QByteArray("No Content") << QByteArray("OK") << QByteArray("application/json");
    }
    */
}

void TestQJsonRpcHttpServer::statusCodes()
{
    QFETCH(QByteArray, body);
    QFETCH(int, statusCode);
    QFETCH(QByteArray, statusReason);
    QFETCH(QByteArray, contentType);

    QJsonRpcHttpServer server;
    server.addService(new TestService);
    QVERIFY(server.listen(QHostAddress::LocalHost, 8118));

    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://127.0.0.1:8118"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    request.setRawHeader("Accept", "application/json-rpc");

    QScopedPointer<QNetworkReply> reply(manager.post(request, body));
    connect(reply.data(), SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), statusCode);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray(), statusReason);
}

void TestQJsonRpcHttpServer::invalidMethod_data()
{
    QTest::addColumn<QNetworkAccessManager::Operation>("operation");
    QTest::newRow("head-request") << QNetworkAccessManager::HeadOperation;
    QTest::newRow("put-request") << QNetworkAccessManager::PutOperation;
    QTest::newRow("delete-request") << QNetworkAccessManager::DeleteOperation;
}

void TestQJsonRpcHttpServer::invalidMethod()
{
    QJsonRpcHttpServer server;
    server.addService(new TestService);
    QVERIFY(server.listen(QHostAddress::LocalHost, 8118));

    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://127.0.0.1:8118"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json-rpc");
    QJsonRpcMessage requestMessage = QJsonRpcMessage::createRequest("service.noParam");

    QScopedPointer<QNetworkReply> reply;
    QFETCH(QNetworkAccessManager::Operation, operation);
    switch (operation) {
    case QNetworkAccessManager::HeadOperation:
        reply.reset(manager.head(request));
        break;

    case QNetworkAccessManager::PutOperation:
        reply.reset(manager.put(request, requestMessage.toJson()));
        break;

    case QNetworkAccessManager::DeleteOperation:
        reply.reset(manager.deleteResource(request));
        break;

    default:
        QFAIL("untested operation");
    }

    connect(reply.data(), SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 405);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray(), QByteArray("Method Not Allowed"));
}

void TestQJsonRpcHttpServer::missingHeaders_data()
{
    QTest::addColumn<QNetworkRequest>("request");

    {
        QNetworkRequest request(QUrl("http://127.0.0.1:8118"));
        request.setRawHeader("Accept", "application/json-rpc");
        QTest::newRow("no-content-type") << request;
    }

    {
        QNetworkRequest request(QUrl("http://127.0.0.1:8118"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QTest::newRow("no-accept") << request;
    }

    {
        QNetworkRequest request(QUrl("http://127.0.0.1:8118"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Accept", "application/invalid");
        QTest::newRow("no-invalid-accept") << request;
    }

    {
        QNetworkRequest request(QUrl("http://127.0.0.1:8118"));
        QTest::newRow("no-jsonpc-headers") << request;
    }
}

void TestQJsonRpcHttpServer::missingHeaders()
{
    QJsonRpcHttpServer server;
    server.addService(new TestService);
    QVERIFY(server.listen(QHostAddress::LocalHost, 8118));

    QFETCH(QNetworkRequest, request);
    QNetworkAccessManager manager;
    QJsonRpcMessage requestMessage = QJsonRpcMessage::createRequest("service.noParam");
    QScopedPointer<QNetworkReply> reply(manager.post(request, requestMessage.toJson()));
    connect(reply.data(), SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 400);
    QCOMPARE(reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray(), QByteArray("Bad Request"));
}

void TestQJsonRpcHttpServer::testMissingAccessControlHeader()
{
    QJsonRpcHttpServer server;
    server.addService(new TestService);
    QVERIFY(server.listen(QHostAddress::LocalHost, 8118));

    //OPTIONS call *missing* headers on reply
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://127.0.0.1:8118"));
    request.setRawHeader("content-type", "text/plain");

    QScopedPointer<QNetworkReply> reply(manager.sendCustomRequest(request,QByteArray("OPTIONS")));
    QJsonRpcMessage request_client = QJsonRpcMessage::createRequest("service.noParam");
    connect(reply.data(), SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(reply->rawHeader("Access-Control-Allow-Origin"), QByteArray(""));
    QCOMPARE(reply->rawHeader("Access-Control-Allow-Methods"), QByteArray(""));
    QCOMPARE(reply->rawHeader("Access-Control-Allow-Headers"), QByteArray(""));
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
}

void TestQJsonRpcHttpServer::testAccessControlHeader()
{
    QJsonRpcHttpServer server;
    server.addService(new TestService);
    QVERIFY(server.listen(QHostAddress::LocalHost, 8118));

    //OPTIONS call check headers on reply
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://127.0.0.1:8118"));
    request.setRawHeader("access-control-request-method", "POST");
    request.setRawHeader("access-control-request-headers", "accept, content-type");
    request.setRawHeader("content-type", "text/plain");
    request.setRawHeader("origin", "http://127.0.0.1:8118");

    QScopedPointer<QNetworkReply> reply(manager.sendCustomRequest(request,QByteArray("OPTIONS")));
    QJsonRpcMessage request_client = QJsonRpcMessage::createRequest("service.noParam");
    connect(reply.data(), SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    connect(reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QVERIFY(reply->hasRawHeader("Access-Control-Allow-Origin"));
    QCOMPARE(reply->rawHeader("Access-Control-Allow-Origin"), QByteArray("http://127.0.0.1:8118"));
    QVERIFY(reply->hasRawHeader("Access-Control-Allow-Methods"));
    QCOMPARE(reply->rawHeader("Access-Control-Allow-Methods"), QByteArray("POST"));
    QVERIFY(reply->hasRawHeader("Access-Control-Allow-Headers"));
    QCOMPARE(reply->rawHeader("Access-Control-Allow-Headers"), QByteArray("accept, content-type"));
}

QTEST_MAIN(TestQJsonRpcHttpServer)
#include "tst_qjsonrpchttpserver.moc"
