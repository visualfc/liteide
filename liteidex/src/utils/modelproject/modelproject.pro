TARGET = modelproject
TEMPLATE = lib

CONFIG += staticlib

include (../../liteideutils.pri)
include (../fileutil/fileutil.pri)

HEADERS += \
    modelprojectimpl.h \
    modelfileimpl.h

SOURCES += \
    modelprojectimpl.cpp \
    modelfileimpl.cpp
