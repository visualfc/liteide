TARGET = liteapi
TEMPLATE = lib

CONFIG += staticlib

include (../../liteideapi.pri)

DEFINES += LITEAPI_LIBRARY

HEADERS += liteobj.h \
           liteapi.h \
    litehtml.h \
    liteqt.h \
    liteutil.h \
    liteids.h
