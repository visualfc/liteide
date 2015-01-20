TARGET = jsonedit
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../3rdparty/qjson/qjson.pri)
include(../../utils/editorutil/editorutil.pri)


DEFINES += JSONEDIT_LIBRARY

SOURCES += jsoneditplugin.cpp \
    jsonedit.cpp

HEADERS += jsoneditplugin.h\
        jsonedit_global.h \
    jsonedit.h
