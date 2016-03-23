TARGET = dlvdebugger
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/litedebugapi/litedebugapi.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)
include(../../3rdparty/qtc_gdbmi/qtc_gdbmi.pri)

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
