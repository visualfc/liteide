TARGET = documentbrowser
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include(../../api/docbrowserapi/docbrowserapi.pri)
include (../extension/extension.pri)

SOURCES += documentbrowser.cpp \
    documentbrowserfactory.cpp

HEADERS += documentbrowser.h \
    documentbrowserfactory.h
