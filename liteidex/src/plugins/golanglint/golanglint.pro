TARGET = golanglint
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/processex/processex.pri)

DEFINES += GOLANGFMT_LIBRARY

SOURCES += golanglintplugin.cpp \
    golanglint.cpp \

HEADERS += golanglintplugin.h\
        golanglint_global.h \
    golanglint.h \

RESOURCES += \
    golanglint.qrc
