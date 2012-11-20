TARGET = golangdocapi
TEMPLATE = lib

CONFIG += staticlib

include (../../liteideapi.pri)
include (../liteapi/liteapi.pri)

DEFINES += DOCBROWSERAPI_LIBRARY

HEADERS += golangdocapi.h
