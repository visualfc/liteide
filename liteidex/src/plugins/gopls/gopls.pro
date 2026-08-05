TARGET = gopls
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include (../../api/golangastapi/golangastapi.pri)
include (../../api/liteeditorapi/liteeditorapi.pri)
include (../../utils/fileutil/fileutil.pri)

SOURCES += goplsplugin.cpp \
    goplsclient.cpp

HEADERS += goplsplugin.h \
    goplsclient.h
