TARGET = jsonedit
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../3rdparty/qjson/qjson.pri)


DEFINES += JSONEDIT_LIBRARY

SOURCES += jsoneditplugin.cpp \
    jsonedit.cpp

HEADERS += jsoneditplugin.h\
        jsonedit_global.h \
    jsonedit.h
