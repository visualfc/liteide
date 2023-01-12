#include "ptyqt.h"
#include <utility>

#ifdef Q_OS_WIN

#ifdef _MSC_VER
#include "conptyprocess.h"
#else
#include "winptyprocess.h"
#endif

#endif

#ifdef Q_OS_UNIX
#include "unixptyprocess.h"
#endif

IPtyProcess *PtyQt::createPtyProcess(IPtyProcess::PtyType ptyType)
{
    switch (ptyType)
    {
#ifdef Q_OS_WIN

#ifdef _MSC_VER
    case IPtyProcess::ConPty:
        return new ConPtyProcess();
        break;
#else
    case IPtyProcess::WinPty:
        return new WinPtyProcess();
        break;
#endif

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

#ifdef _MSC_VER
        return new ConPtyProcess();
#else
        return new WinPtyProcess();
#endif

#endif
#ifdef Q_OS_UNIX
    return new UnixPtyProcess();
#endif
}
