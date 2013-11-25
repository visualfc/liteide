TARGET = goslide
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += GOSLIDE_LIBRARY

SOURCES += goslideplugin.cpp \
    goslideedit.cpp

HEADERS += goslideplugin.h\
        goslide_global.h \
    goslideedit.h

RESOURCES += \
    goslide.qrc
