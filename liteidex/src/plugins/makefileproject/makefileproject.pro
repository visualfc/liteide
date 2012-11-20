TARGET = makefileproject
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/modelproject/modelproject.pri)

DEFINES += MAKEFILEPROJECT_LIBRARY

SOURCES += makefileprojectplugin.cpp \
    makefilefilefactory.cpp \
    makefileproject.cpp \
    makefilefile.cpp

HEADERS += makefileprojectplugin.h\
        makefileproject_global.h \
    makefilefilefactory.h \
    makefileproject.h \
    makefilefile.h
