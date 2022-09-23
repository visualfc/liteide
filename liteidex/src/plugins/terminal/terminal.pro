TARGET = terminal
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/terminalapi/terminalapi.pri)
include(../../utils/vterm/vterm.pri)
include(../../utils/tabwidget/tabwidget.pri)

DEFINES += TERMINAL_LIBRARY

win32 {
    QT += network
}

SOURCES += terminalplugin.cpp \
    terminal.cpp \
    terminaloption.cpp \
    terminaloptionfactory.cpp

HEADERS += terminalplugin.h\
        terminal.h \
        terminal_global.h \
        terminaloption.h \
        terminaloptionfactory.h

FORMS += \
    terminaloption.ui
