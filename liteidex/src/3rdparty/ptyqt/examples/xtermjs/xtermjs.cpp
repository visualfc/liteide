#include <QCoreApplication>
#include <QWebSocketServer>
#include <QWebSocket>
#include "ptyqt.h"
#include <QTimer>
#include <QProcessEnvironment>
#include <QSysInfo>

#define PORT 4242

#define COLS 87
#define ROWS 26

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    //start WebSockets server for receive connections from xterm.js
    QWebSocketServer wsServer("TestServer", QWebSocketServer::NonSecureMode);
    if (!wsServer.listen(QHostAddress::Any, PORT))
        return 1;

    QMap<QWebSocket *, IPtyProcess *> sessions;

    //create new session on new connection
    QObject::connect(&wsServer, &QWebSocketServer::newConnection, [&wsServer, &sessions]()
    {
        //handle new connection
        QWebSocket *wSocket = wsServer.nextPendingConnection();

        //use cmd.exe or bash, depends on target platform
        IPtyProcess::PtyType ptyType = IPtyProcess::WinPty;
        qint32 buildNumber = QSysInfo::kernelVersion().split(".").last().toInt();
        if (buildNumber >= CONPTY_MINIMAL_WINDOWS_VERSION)
        {
            qDebug() << "Use ConPty instead of WinPty";
            ptyType = IPtyProcess::ConPty;
        }

        //force select WinPty
        ptyType = IPtyProcess::WinPty;

        QString shellPath = "c:\\Windows\\system32\\cmd.exe";
        //shellPath = "C:\\Program\ Files\\Git\\bin\\bash.exe";
#ifdef Q_OS_UNIX
        shellPath = "/bin/sh";
        ptyType = IPtyProcess::UnixPty;
#endif

        //create new Pty instance
        IPtyProcess *pty = PtyQt::createPtyProcess(ptyType);

        qDebug() << "New connection" << wSocket->peerAddress() << wSocket->peerPort() << pty->pid();

        //start Pty process ()
        pty->startProcess(shellPath, QProcessEnvironment::systemEnvironment().toStringList(), COLS, ROWS);

        if (!pty->lastError().isEmpty())
        {
            qDebug() << pty->lastError();
            delete pty;
            return;
        }

        //connect read channel from Pty process to write channel on websocket
        QObject::connect(pty->notifier(), &QIODevice::readyRead, [wSocket, pty]()
        {
            QByteArray data = pty->readAll();
            //qDebug() << "< " << data;
            wSocket->sendTextMessage(data);
        });

        //connect read channel of Websocket to write channel of Pty process
        QObject::connect(wSocket, &QWebSocket::textMessageReceived, [wSocket, pty](const QString &message)
        {
            //qDebug() << "> " << message.size() << message.at(0) << message << message.toUtf8() << QString::fromUtf8(message.toUtf8());
            pty->write(message.toUtf8());
        });

        //for example handle disconnections, process crashes and stuff like that...
        auto endSessionHandler = [wSocket, &sessions]()
        {
            IPtyProcess *pty = sessions.value(wSocket);
            if (pty == 0)
                return; //because can be called twice

            sessions.remove(wSocket);

            qDebug() << "wSockMn" << wSocket << pty;

            if (wSocket->isValid())
                wSocket->close();
            wSocket->deleteLater();

            pty->kill();
            delete pty;
        };

        QObject::connect(wSocket, &QWebSocket::disconnected, endSessionHandler);

#ifdef Q_OS_UNIX
        QProcess *shellProcess = qobject_cast<QProcess *>(pty->notifier());
        QObject::connect(shellProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                         [endSessionHandler](int, QProcess::ExitStatus ) { endSessionHandler(); });
#else
        QLocalSocket *localSocket = qobject_cast<QLocalSocket *>(pty->notifier());
        QObject::connect(localSocket, &QLocalSocket::disconnected, endSessionHandler);
#endif

        //add connection to list of active connections
        sessions.insert(wSocket, pty);

        qDebug() << pty->size();
    });

    //stop eventloop if needed
    //QTimer::singleShot(5000, [](){ qApp->quit(); });

    //exec eventloop
    bool res = app.exec();

    QMapIterator<QWebSocket *, IPtyProcess *> it(sessions);
    while (it.hasNext())
    {
        it.next();

        it.key()->deleteLater();
        delete it.value();
    }
    sessions.clear();

    return res;
}
