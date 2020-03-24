TARGET = terminal
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += TERMINAL_LIBRARY

SOURCES += terminalplugin.cpp

HEADERS += terminalplugin.h\
        terminal_global.h
