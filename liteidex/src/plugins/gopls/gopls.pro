TARGET = gopls
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../api/liteenvapi/liteenvapi.pri)
include (../../api/golangastapi/golangastapi.pri)
include (../../api/liteeditorapi/liteeditorapi.pri)
include (../../api/litefindapi/litefindapi.pri)
include (../../utils/fileutil/fileutil.pri)

SOURCES += goplsplugin.cpp \
    goplsclient.cpp \
    goplssearchresults.cpp \
    goplsoption.cpp \
    goplsoptionfactory.cpp

HEADERS += goplsplugin.h \
    goplsclient.h \
    goplssearchresults.h \
    gopls_global.h \
    goplsoption.h \
    goplsoptionfactory.h

FORMS += goplsoption.ui
