TARGET = vterm
TEMPLATE = lib
CONFIG += staticlib

QT += xml

include (../../liteideutils.pri)
include (../../3rdparty/libvterm/libvterm.pri)
include (../../3rdparty/ptyqt/ptyqt.pri)


HEADERS += \
    vtermcolor.h \
    vtermwidget.h \
    vtermwidgetbase.h

SOURCES += \
    vtermwidget.cpp \
    vtermwidgetbase.cpp
