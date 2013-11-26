TARGET = golangpresent
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../utils/editorutil/editorutil.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/processex/processex.pri)

DEFINES += GOLANGPRESENT_LIBRARY

SOURCES += \
    golangpresentedit.cpp \
    golangpresentplugin.cpp

HEADERS +=\
    golangpresent_global.h \
    golangpresentedit.h \
    golangpresentplugin.h

RESOURCES += \
    golangpresent.qrc
