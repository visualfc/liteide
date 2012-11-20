TARGET = litebuildapi
TEMPLATE = lib

CONFIG += staticlib

include (../../liteideapi.pri)
include (../liteapi/liteapi.pri)

HEADERS += litebuildapi.h
