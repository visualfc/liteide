TARGET = textoutput
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)

HEADERS += textoutput.h \
    terminaledit.h

SOURCES += textoutput.cpp \
    terminaledit.cpp
