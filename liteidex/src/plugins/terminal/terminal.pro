TARGET = terminal
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/vterm/vterm.pri)

DEFINES += TERMINAL_LIBRARY

win32 {
    QT += network
}

SOURCES += terminalplugin.cpp \
    terminal.cpp

HEADERS += terminalplugin.h\
        terminal.h \
        terminal_global.h
