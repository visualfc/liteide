TARGET = textoutput
TEMPLATE = lib

CONFIG += staticlib

include (../../liteideutils.pri)
include (../colorstyle/colorstyle.pri)

HEADERS += textoutput.h \
    terminaledit.h

SOURCES += textoutput.cpp \
    terminaledit.cpp
