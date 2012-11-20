TARGET = qtc_editutil
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)

HEADERS += \
    filterlineedit.h \
    fancylineedit.h \
    uncommentselection.h \
    utils_global.h

SOURCES += \
    filterlineedit.cpp \
    fancylineedit.cpp \
    uncommentselection.cpp

RESOURCES +=
