TARGET = dlvclient
TEMPLATE = lib
CONFIG += staticlib

QT += network

include (../../liteideutils.pri)
include (../../3rdparty/qjsonrpc/qjsonrpc.pri)

HEADERS += \
    dlvclient.h \
    dlvtypes.h \
    varianthelper.h

SOURCES += \
    dlvclient.cpp \
    varianthelper.cpp
