TARGET = golangapi
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include(../../api/golangdocapi/golangdocapi.pri)

SOURCES += golangapi.cpp

HEADERS += golangapi.h
