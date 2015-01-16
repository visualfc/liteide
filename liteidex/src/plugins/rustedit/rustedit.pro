TARGET = rustedit
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/fileutil/fileutil.pri)

DEFINES += RUSTEDIT_LIBRARY

SOURCES += rusteditplugin.cpp \
    rustedit.cpp

HEADERS += rusteditplugin.h\
        rustedit_global.h \
    rustedit.h
