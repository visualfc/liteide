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

    virtual bool startProcess(const QString &shellPath, const QStringList &arguments, const QString &workingDirectory, QStringList environment, qint16 rows, qint16 cols);
    virtual bool resize(qint16 cols, qint16 rows);
    virtual bool kill();
    virtual PtyType type();
    virtual QString dumpDebugInfo();
    virtual QIODevice *notifier();
    virtual QByteArray readAll();
    virtual qint64 write(const QByteArray &byteArray);
    virtual bool isAvailable();
    virtual void moveToThread(QThread *targetThread);
    virtual bool hasProcessList() const;
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
