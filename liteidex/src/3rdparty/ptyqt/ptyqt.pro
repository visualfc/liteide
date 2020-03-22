TARGET = ptyqt
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)

HEADERS += \
    core/iptyprocess.h \
    core/ptyqt.h

SOURCES += \
    core/ptyqt.cpp

win32 {
HEADERS += \
    core/winptyprocess.h \
    core/winpty.h \
    core/winpty_constants.h \
    core/winptyprocess.h

SOURCES += \
    core/winptyprocess.cpp

QT += network
}

unix {
HEADERS += \
    core/unixptyprocess.h

SOURCES += \
    core/unixptyprocess.cpp
}
