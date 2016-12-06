TARGET = diff_match_pitch
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)

HEADERS += \
    diff_match_patch.h

SOURCES += \
    diff_match_patch.cpp

RESOURCES +=
