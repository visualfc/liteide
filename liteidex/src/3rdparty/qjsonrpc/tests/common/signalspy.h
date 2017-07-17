#ifndef SIGNALSPY_H
#define SIGNALSPY_H

#if QT_VERSION >= 0x050000
typedef QSignalSpy SignalSpy;
#else
#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qobject.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>
#include <QtTest/qtesteventloop.h>

class QVariant;
class SignalSpy: public QObject, public QList<QList<QVariant> >
{
public:
    explicit SignalSpy(const QObject *obj, const char *aSignal)
        : m_waiting(false)
    {
        static const int memberOffset = QObject::staticMetaObject.methodCount();
        if (!obj) {
            qWarning("SignalSpy: Cannot spy on a null object");
            return;
        }

        if (!aSignal) {
            qWarning("SignalSpy: Null signal name is not valid");
            return;
        }

        if (((aSignal[0] - '0') & 0x03) != QSIGNAL_CODE) {
            qWarning("SignalSpy: Not a valid signal, use the SIGNAL macro");
            return;
        }

        const QByteArray ba = QMetaObject::normalizedSignature(aSignal + 1);
        const QMetaObject * const mo = obj->metaObject();
        const int sigIndex = mo->indexOfMethod(ba.constData());
        if (sigIndex < 0) {
            qWarning("SignalSpy: No such signal: '%s'", ba.constData());
            return;
        }

        if (!QMetaObject::connect(obj, sigIndex, this, memberOffset,
                    Qt::DirectConnection, 0)) {
            qWarning("SignalSpy: QMetaObject::connect returned false. Unable to connect.");
            return;
        }
        sig = ba;
        initArgs(mo->method(sigIndex), obj);
    }

    inline bool isValid() const { return !sig.isEmpty(); }
    inline QByteArray signal() const { return sig; }

    bool wait(int timeout = 5)
    {
        Q_ASSERT(!m_waiting);
        const int origCount = count();
        m_waiting = true;
        m_loop.enterLoop(timeout);
        m_waiting = false;
        return count() > origCount;
    }

    int qt_metacall(QMetaObject::Call call, int methodId, void **a)
    {
        methodId = QObject::qt_metacall(call, methodId, a);
        if (methodId < 0)
            return methodId;

        if (call == QMetaObject::InvokeMetaMethod) {
            if (methodId == 0) {
                appendArgs(a);
            }
            --methodId;
        }
        return methodId;
    }

private:
    void initArgs(const QMetaMethod &member)
    {
        initArgs(member, 0);
    }

    void initArgs(const QMetaMethod &member, const QObject *obj)
    {
        Q_UNUSED(obj)
        QList<QByteArray> params = member.parameterTypes();
        for (int i = 0; i < params.count(); ++i) {
            int tp = QMetaType::type(params.at(i).constData());
            if (tp == QMetaType::Void)
                qWarning("Don't know how to handle '%s', use qRegisterMetaType to register it.",
                         params.at(i).constData());
            args << tp;
        }
    }

    void appendArgs(void **a)
    {
        QList<QVariant> list;
        for (int i = 0; i < args.count(); ++i) {
            QMetaType::Type type = static_cast<QMetaType::Type>(args.at(i));
            list << QVariant(type, a[i + 1]);
        }
        append(list);

        if (m_waiting)
            m_loop.exitLoop();
    }

    // the full, normalized signal name
    QByteArray sig;
    // holds the QMetaType types for the argument list of the signal
    QVector<int> args;

    QTestEventLoop m_loop;
    bool m_waiting;
};
#endif
#endif
