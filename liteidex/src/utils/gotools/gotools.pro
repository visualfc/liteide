TARGET = gotools
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)

HEADERS += gotools.h \
    libgopher.h

SOURCES += \
    gotools.cpp
