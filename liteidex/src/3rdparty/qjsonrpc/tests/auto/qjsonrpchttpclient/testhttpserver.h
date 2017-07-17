#ifndef TESTHTTPSERVER_H
#define TESTHTTPSERVER_H

#include <QTcpServer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

class TestHttpServerRequestHandler
{
public:
    virtual ~TestHttpServerRequestHandler() {}
    virtual QByteArray handleRequest(QNetworkAccessManager::Operation operation,
                                     const QNetworkRequest &request, const QByteArray &body) = 0;
};

class TestHttpServerPrivate;
class TestHttpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TestHttpServer(QObject *parent = 0);
    TestHttpServer(TestHttpServerRequestHandler *requestHandler, QObject *parent = 0);
    virtual ~TestHttpServer();

    TestHttpServerRequestHandler *requestHandler() const;
    void setRequestHandler(TestHttpServerRequestHandler *requestHandler);

    void setResponseData(const QByteArray &data);

protected:
#if QT_VERSION >= 0x050000
    virtual void incomingConnection(qintptr socketDescriptor);
#else
    virtual void incomingConnection(int socketDescriptor);
#endif

private:
    Q_DISABLE_COPY(TestHttpServer)
    Q_DECLARE_PRIVATE(TestHttpServer)
    QScopedPointer<TestHttpServerPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_socketReadyRead())
    Q_PRIVATE_SLOT(d_func(), void _q_socketError(QAbstractSocket::SocketError))

};

#endif
