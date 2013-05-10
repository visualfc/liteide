TARGET = golangcode
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include(../../api/golangastapi/golangastapi.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)
include (../../3rdparty/qtc_editutil/qtc_editutil.pri)

DEFINES += GOLANGCODE_LIBRARY

SOURCES += golangcodeplugin.cpp \
    golangcode.cpp

HEADERS += golangcodeplugin.h\
        golangcode_global.h \
    golangcode.h
