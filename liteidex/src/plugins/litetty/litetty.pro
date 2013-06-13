TARGET = litetty
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += LITETTY_LIBRARY

SOURCES += litettyplugin.cpp

HEADERS += litettyplugin.h\
        litetty_global.h
