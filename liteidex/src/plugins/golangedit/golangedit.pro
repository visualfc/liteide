TARGET = golangedit
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/litefindapi/litefindapi.pri)
include(../../utils/processex/processex.pri)

DEFINES += GOLANGEDIT_LIBRARY

SOURCES += golangeditplugin.cpp \
    golangedit.cpp \
    golangfilesearch.cpp

HEADERS += golangeditplugin.h\
        golangedit_global.h \
    golangedit.h \
    golangfilesearch.h
