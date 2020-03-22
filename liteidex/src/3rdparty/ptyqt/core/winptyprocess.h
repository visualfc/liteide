#ifndef WINPTYPROCESS_H
#define WINPTYPROCESS_H

#include "iptyprocess.h"
#define _WIN32_WINNT  0x0501
#include <windows.h>
#include <QLocalSocket>
#include "winpty.h"

class WinPtyProcess : public IPtyProcess
{
    Q_OBJECT
public:
    WinPtyProcess();
    virtual ~WinPtyProcess();

    bool startProcess(const QString &shellPath, const QStringList &arguments, QStringList environment, qint16 rows, qint16 cols);
    bool resize(qint16 cols, qint16 rows);
    bool kill();
    PtyType type();
    QString dumpDebugInfo();
    QIODevice *notifier();
    QByteArray readAll();
    qint64 write(const QByteArray &byteArray);
    bool isAvailable();
    void moveToThread(QThread *targetThread);
public slots:
    void disconnected();
private:
    winpty_t *m_ptyHandler;
    HANDLE m_innerHandle;
    QString m_conInName;
    QString m_conOutName;
    QLocalSocket *m_inSocket;
    QLocalSocket *m_outSocket;
};

#endif // WINPTYPROCESS_H
