TARGET = golangplay
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/textoutput/textoutput.pri)
include(../../utils/processex/processex.pri)
include (../../utils/colorstyle/colorstyle.pri)

DEFINES += GOLANGPLAY_LIBRARY

SOURCES += golangplayplugin.cpp \
    goplaybrowser.cpp

HEADERS += golangplayplugin.h\
        golangplay_global.h \
    goplaybrowser.h
