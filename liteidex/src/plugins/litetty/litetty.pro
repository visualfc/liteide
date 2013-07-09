TARGET = litetty
TEMPLATE = lib


    QT += network


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
    SOURCES += sockettty.cpp
    HEADERS += sockettty.h
    SOURCES += fifotty.cpp
    HEADERS += fifotty.h
}
