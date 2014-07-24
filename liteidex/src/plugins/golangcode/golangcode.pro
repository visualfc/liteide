TARGET = golangcode
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include(../../api/golangastapi/golangastapi.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)

DEFINES += GOLANGCODE_LIBRARY

SOURCES += golangcodeplugin.cpp \
        golangcode.cpp \
        golangcodeoptionfactory.cpp \
        golangcodeoption.cpp

HEADERS += golangcodeplugin.h\
        golangcode_global.h \
        golangcode.h \
        golangcodeoptionfactory.h \
        golangcodeoption.h

FORMS += \
        golangcodeoption.ui
