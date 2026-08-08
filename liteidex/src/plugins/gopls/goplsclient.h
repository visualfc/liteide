#ifndef GOPLSCLIENT_H
#define GOPLSCLIENT_H

#include <QObject>
#include <QHash>
#include <QJsonObject>
#include <QProcess>

class GoplsClient : public QObject
{
    Q_OBJECT
public:
    explicit GoplsClient(QObject *parent = 0);
    ~GoplsClient();

    bool start(const QString &program, const QString &workingDirectory,
               const QProcessEnvironment &environment, const QJsonObject &initializeParams);
    void stop();
    bool isRunning() const;
    int request(const QString &method, const QJsonValue &params);
    void notify(const QString &method, const QJsonValue &params = QJsonValue());
    void reply(int id, const QJsonValue &result);

signals:
    void initialized();
    void response(int id, const QString &method, const QJsonValue &result, const QJsonObject &error);
    void notification(const QString &method, const QJsonValue &params);
    void serverRequest(int id, const QString &method, const QJsonValue &params);
    void logMessage(const QString &message, bool error);
    void stopped();

private slots:
    void readOutput();
    void readError();
    void processFinished(int exitCode, QProcess::ExitStatus status);

private:
    void writeMessage(const QJsonObject &message);
    void dispatch(const QJsonObject &message);

    QProcess *m_process;
    QByteArray m_buffer;
    QHash<int,QString> m_requests;
    int m_nextId;
    int m_initializeId;
    bool m_stopping;
};

#endif // GOPLSCLIENT_H
