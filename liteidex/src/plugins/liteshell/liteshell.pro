TARGET = liteshell
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += LITESHELL_LIBRARY

SOURCES += liteshellplugin.cpp

HEADERS += liteshellplugin.h\
        liteshell_global.h
