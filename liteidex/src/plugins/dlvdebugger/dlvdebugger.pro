TARGET = dlvdebugger
TEMPLATE = lib

QT += network

include(../../liteideplugin.pri)
include(../../api/litedebugapi/litedebugapi.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)
include(../../3rdparty/qtc_gdbmi/qtc_gdbmi.pri)
include(../../utils/dlvclient/dlvclient.pri)
include (../../3rdparty/qjsonrpc/qjsonrpc.pri)

DEFINES += GDBDEBUGER_LIBRARY

SOURCES += dlvdebuggerplugin.cpp \
    dlvdebugger.cpp \
    dlvdebuggeroptionfactory.cpp \
    dlvdebuggeroption.cpp

HEADERS += dlvdebuggerplugin.h\
        dlvdebugger_global.h \
    dlvdebugger.h \
    dlvdebuggeroptionfactory.h \
    dlvdebuggeroption.h

FORMS += \
    dlvdebuggeroption.ui
