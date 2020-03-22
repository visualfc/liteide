#include "ptyqt.h"
#include <utility>

#ifdef Q_OS_WIN
#include "winptyprocess.h"
#endif

#ifdef Q_OS_UNIX
#include "unixptyprocess.h"
#endif

IPtyProcess *PtyQt::createPtyProcess(IPtyProcess::PtyType ptyType)
{
    switch (ptyType)
    {
#ifdef Q_OS_WIN
    case IPtyProcess::WinPty:
        return new WinPtyProcess();
        break;
#endif
#ifdef Q_OS_UNIX
    case IPtyProcess::UnixPty:
        return new UnixPtyProcess();
        break;
#endif
    case IPtyProcess::AutoPty:
    default:
        break;
    }

#ifdef Q_OS_WIN
        return new WinPtyProcess();
#endif
#ifdef Q_OS_UNIX
    return new UnixPtyProcess();
#endif
}
