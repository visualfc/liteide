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
    core/winpty.h \
    core/winpty_constants.h
QT += network
}

win32-g++ {
HEADERS += \
    core/winptyprocess.h

SOURCES += \
    core/winptyprocess.cpp
}

win32-msvc* {
HEADERS += \
    core/conptyprocess.h

SOURCES += \
    core/conptyprocess.cpp
}

unix {
HEADERS += \
    core/unixptyprocess.h

SOURCES += \
    core/unixptyprocess.cpp
}
