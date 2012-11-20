TARGET = hello
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += HELLO_LIBRARY

SOURCES += helloplugin.cpp

HEADERS += helloplugin.h\
        hello_global.h
