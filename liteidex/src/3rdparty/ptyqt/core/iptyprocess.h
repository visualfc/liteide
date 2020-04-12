#ifndef IPTYPROCESS_H
#define IPTYPROCESS_H

#include <QString>
#include <QStringList>
#include <QDebug>

//#ifdef Q_OS_WIN
//#include <QLocalSocket>
//#endif

#define CONPTY_MINIMAL_WINDOWS_VERSION 18309

class IPtyProcess : public QObject
{
    Q_OBJECT
public:
    enum PtyType
    {
        UnixPty = 0,
        WinPty = 1,
        ConPty = 2,
        AutoPty = 3
    };

    IPtyProcess()
        : m_pid(0)
        , m_trace(false)
    {  }
    virtual ~IPtyProcess() { }

    virtual bool startProcess(const QString &shellPath, const QStringList &arguments, const QString &workingDirectory, QStringList environment, qint16 cols, qint16 rows) = 0;
    virtual bool resize(qint16 cols, qint16 rows) = 0;
    virtual bool kill() = 0;
    virtual PtyType type() = 0;
    virtual QString dumpDebugInfo() = 0;
    virtual QIODevice *notifier() = 0;
    virtual QByteArray readAll() = 0;
    virtual qint64 write(const QByteArray &byteArray) = 0;
    virtual bool isAvailable() = 0;
    virtual void moveToThread(QThread *targetThread) = 0;
    virtual bool hasProcessList() const = 0;
    qint64 pid() { return m_pid; }
    QPair<qint16, qint16> size() { return m_size; }
    const QString lastError() { return m_lastError; }
    bool toggleTrace() { m_trace = !m_trace; return m_trace; }
signals:
    void exited();
protected:
    QString m_shellPath;
    QString m_lastError;
    qint64 m_pid;
    QPair<qint16, qint16> m_size; //cols / rows
    bool m_trace;
};

#endif // IPTYPROCESS_H
