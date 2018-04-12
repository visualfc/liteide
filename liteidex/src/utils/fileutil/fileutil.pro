TARGET = fileutil
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)

HEADERS += fileutil.h \
    findfilesthread.h

SOURCES += fileutil.cpp \
    findfilesthread.cpp
