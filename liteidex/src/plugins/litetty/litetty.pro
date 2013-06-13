TARGET = litetty
TEMPLATE = lib

win32 {
    QT += network
}

include(../../liteideplugin.pri)
include(../../api/litettyapi/litettyapi.pri)

DEFINES += LITETTY_LIBRARY

SOURCES += litettyplugin.cpp \
    litetty.cpp

HEADERS += litettyplugin.h\
        litetty_global.h \
    litetty.h

win32 {
    SOURCES += sockettty.cpp
    HEADERS += sockettty.h
} else {
    SOURCES += fifotty.cpp
    HEADERS += fifotty.h
}
