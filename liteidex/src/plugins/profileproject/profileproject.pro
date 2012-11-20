TARGET = profileproject
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/modelproject/modelproject.pri)

DEFINES += PROFILEPROJECT_LIBRARY

SOURCES += profileprojectplugin.cpp \
    profilefile.cpp \
    profileproject.cpp \
    profilefilefactory.cpp

HEADERS += profileprojectplugin.h\
        profileproject_global.h \
    profilefile.h \
    profileproject.h \
    profilefilefactory.h
