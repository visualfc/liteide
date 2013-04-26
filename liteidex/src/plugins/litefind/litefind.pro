TARGET = litefind
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../utils/textoutput/textoutput.pri)

DEFINES += LITEFIND_LIBRARY

SOURCES += litefindplugin.cpp \
    filesearch.cpp \
    findeditor.cpp

HEADERS += litefindplugin.h\
        litefind_global.h \
    filesearch.h \
    findeditor.h
