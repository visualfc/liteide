TARGET = liteenv
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/liteenvapi/liteenvapi.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)

DEFINES += LITEENV_LIBRARY

SOURCES += liteenvplugin.cpp \
    envmanager.cpp \
    liteenvoptionfactory.cpp \
    liteenvoption.cpp \
    goenvmanager.cpp

HEADERS += liteenvplugin.h\
        liteenv_global.h \
    envmanager.h \
    liteenvoptionfactory.h \
    liteenvoption.h \
    goenvmanager.h

FORMS += \
    liteenvoption.ui

RESOURCES += \
    liteenv.qrc
