#ifndef PTYDEVICE_H
#define PTYDEVICE_H

#include "ptyqt.h"
#include <QProcessEnvironment>

class PtyDevice : public QIODevice
{
    Q_OBJECT
public:

    PtyDevice()
    {
        //use cmd.exe or bash, depends on target platform
        IPtyProcess::PtyType ptyType = IPtyProcess::WinPty;
#ifdef Q_OS_UNIX
        ptyType = IPtyProcess::UnixPty;
#endif

        //create new Pty instance
        m_pty = PtyQt::createPtyProcess(ptyType);

        connect(m_pty->notifier(), &QIODevice::readyRead, [this](){
            m_readBuffer.append(m_pty->readAll());
            emitReadyRead();
        });

        setOpenMode(QIODevice::ReadWrite);
    }
    ~PtyDevice() { delete m_pty; }

    void start()
    {
        QString shellPath = "c:\\Windows\\system32\\cmd.exe";
    #ifdef Q_OS_UNIX
        shellPath = "/bin/bash";
    #endif

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("TERM", "vt100");
        env.insert("COLUMNS", "80");
        env.insert("PS1", "> ");

        //start Pty process ()
        m_pty->startProcess(shellPath, env.toStringList(), 80, 24);
    }

    qint64 readData(char *data, qint64 maxlen)
    {
        qint64 readSize = qMin(maxlen, (qint64)m_readBuffer.size());

        QByteArray tmp = m_readBuffer.left(readSize);
        m_readBuffer.remove(0, readSize);
        memcpy(data, tmp.data(), tmp.size());

        return tmp.size();
    }

    qint64 writeData(const char *data, qint64 len)
    {
        qDebug() << "write" << QByteArray(data, len);
        return m_pty->write(QByteArray(data, len));
    }

    bool   isSequential() { return true; }
    qint64 bytesAvailable() { return m_readBuffer.size(); }
    qint64 size() { return m_readBuffer.size(); }

    void emitReadyRead()
    {
        emit readyRead();
    }

private:
    IPtyProcess *m_pty;

    QByteArray m_readBuffer;
};

#endif // PTYDEVICE_H
