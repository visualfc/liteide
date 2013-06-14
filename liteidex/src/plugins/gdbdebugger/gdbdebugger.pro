TARGET = gdbdebugger
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/litedebugapi/litedebugapi.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)
include(../../3rdparty/qtc_gdbmi/qtc_gdbmi.pri)

DEFINES += GDBDEBUGER_LIBRARY

SOURCES += gdbdebuggerplugin.cpp \
    gdbdebugger.cpp \

HEADERS += gdbdebuggerplugin.h\
        gdbdebugger_global.h \
    gdbdebugger.h \
