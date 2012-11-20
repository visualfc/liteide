TARGET = litefind
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../utils/textoutput/textoutput.pri)

DEFINES += LITEFIND_LIBRARY

SOURCES += litefindplugin.cpp \
    findeditor.cpp \
    replaceeditor.cpp \
    filesearch.cpp

HEADERS += litefindplugin.h\
        litefind_global.h \
    findeditor.h \
    replaceeditor.h \
    filesearch.h
