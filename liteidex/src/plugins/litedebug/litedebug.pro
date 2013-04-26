TARGET = litedebug
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/litedebugapi/litedebugapi.pri)
include(../../api/litebuildapi/litebuildapi.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/textoutput/textoutput.pri)
include (../../utils/colorstyle/colorstyle.pri)

DEFINES += LITEDEBUG_LIBRARY

SOURCES += litedebugplugin.cpp \
    debugmanager.cpp \
    debugwidget.cpp \
    litedebug.cpp \
    litedebugoptionfactory.cpp \
    litedebugoption.cpp \
    selectexternaldialog.cpp

HEADERS += litedebugplugin.h\
        litedebug_global.h \
    debugmanager.h \
    debugwidget.h \
    litedebug.h \
    litedebugoptionfactory.h \
    litedebugoption.h \
    selectexternaldialog.h

RESOURCES += \
    litedebug.qrc

FORMS += \
    litedebugoption.ui \
    selectexternaldialog.ui
