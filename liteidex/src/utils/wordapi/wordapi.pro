TARGET = wordapi
TEMPLATE = lib

CONFIG += staticlib

include (../../liteideutils.pri)
include (../../api/liteeditorapi/liteeditorapi.pri)

HEADERS += wordapi.h

SOURCES += wordapi.cpp
